#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "packet_codec.h"

static void test_dialogue_and_quest_codec(void) {
    uint8_t buf[1400];
    PacketWriter w; PacketReader r;
    MsgDialogueLine dl={0}, dl2={0}; char line[256]={0};
    MsgDialogueChoicesHeader ch={0}, ch2={0}; MsgDialogueChoiceRecord cr[2]={{0}}, cr2[2]={{0}}; char ctexts[2][256]={{0}}; const char* csrc[2]={"Yes","No"};
    MsgQuestUpdateHeader qh={0}, qh2={0}; MsgQuestObjectiveRecord qo[2]={{0}}, qo2[2]={{0}}; char qtexts[2][256]={{0}}; const char* qsrc[2]={"Go there","Talk to NPC"};
    pw_init(&w, buf, sizeof(buf)); dl.dialogue_id=77; dl.speaker_actor_net_id=9; assert(encode_msg_dialogue_line(&w,&dl,"Hello there")); pr_init(&r, buf, pw_bytes_written(&w)); assert(decode_msg_dialogue_line(&r,&dl2,line,sizeof(line))); assert(dl2.dialogue_id==77 && strcmp(line,"Hello there")==0);
    pw_init(&w, buf, sizeof(buf)); ch.dialogue_id=88; ch.choice_count=2; cr[0].choice_id=1; cr[0].enabled=1; cr[1].choice_id=2; cr[1].enabled=0; assert(encode_msg_dialogue_choices(&w,&ch,cr,csrc)); pr_init(&r, buf, pw_bytes_written(&w)); assert(decode_msg_dialogue_choices(&r,&ch2,cr2,ctexts,2)); assert(ch2.choice_count==2 && cr2[0].choice_id==1 && strcmp(ctexts[1],"No")==0);
    pw_init(&w, buf, sizeof(buf)); qh.quest_id=12; qh.stage=3; qh.tracked=1; qh.objective_count=2; qo[0].objective_id=100; qo[0].state=QOBJ_ACTIVE; qo[1].objective_id=101; qo[1].state=QOBJ_COMPLETED; assert(encode_msg_quest_update(&w,&qh,qo,qsrc)); pr_init(&r, buf, pw_bytes_written(&w)); assert(decode_msg_quest_update(&r,&qh2,qo2,qtexts,2)); assert(qh2.quest_id==12 && qh2.objective_count==2 && strcmp(qtexts[0],"Go there")==0);
}

int main(void) {
    test_dialogue_and_quest_codec();
    uint8_t buf[256]; PacketWriter w; PacketReader r; MsgDamageResult a = {0}, b = {0};
    a.attacker_id = 7; a.victim_actor_net_id = 2001; a.damage_amount = 0.25f; a.damage_type = DMG_BALLISTIC; a.flags = DMGF_KILLED; a.health_after = 0.0f;
    pw_init(&w, buf, sizeof(buf));
    assert(encode_msg_damage_result(&w, &a));
    pr_init(&r, buf, pw_bytes_written(&w));
    assert(decode_msg_damage_result(&r, &b));
    assert(a.attacker_id == b.attacker_id);
    assert(a.victim_actor_net_id == b.victim_actor_net_id);
    assert(a.flags == b.flags);
    assert(a.health_after == b.health_after);
    printf("codec_roundtrip_test passed\n");
    return 0;
}
