#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "protocol_apply.h"
#include "proxy_runtime.h"
#include "packet_codec.h"
#include "message_ids.h"

static size_t finalize(PacketWriter* w, size_t payload_start) {
    uint16_t payload_len = (uint16_t)(pw_bytes_written(w) - payload_start);
    memcpy(w->buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return pw_bytes_written(w);
}

static size_t make_packet(uint8_t* out, uint16_t type, uint16_t flags, const void* msg, bool (*enc)(PacketWriter*, const void*)) {
    PacketWriter w; PacketHeader ph={0}; MessageHeader mh={0}; size_t start;
    ph.magic = F4MP_MAGIC; ph.version = F4MP_VERSION; ph.message_count = 1;
    mh.type = type; mh.flags = flags;
    pw_init(&w, out, 1400);
    assert(encode_packet_header(&w, &ph));
    assert(encode_message_header(&w, &mh));
    start = pw_bytes_written(&w);
    assert(enc(&w, msg));
    return finalize(&w, start);
}
static bool enc_profile(PacketWriter* w, const void* m){return encode_msg_profile_snapshot(w,(const MsgProfileSnapshot*)m);}
static bool enc_profile_items(PacketWriter* w, const void* m){ const MsgProfileItemsHeader* h=(const MsgProfileItemsHeader*)m; const MsgProfileItemRecord* items=(const MsgProfileItemRecord*)(h+1); return encode_msg_profile_items(w,h,items);}
static bool enc_profile_perks(PacketWriter* w, const void* m){ const MsgProfilePerksHeader* h=(const MsgProfilePerksHeader*)m; const MsgProfilePerkRecord* perks=(const MsgProfilePerkRecord*)(h+1); return encode_msg_profile_perks(w,h,perks);}
static bool enc_profile_appearance(PacketWriter* w, const void* m){return encode_msg_profile_appearance(w,(const MsgProfileAppearance*)m);}
static bool enc_player(PacketWriter* w, const void* m){return encode_msg_player_state(w,(const MsgPlayerState*)m);}
static bool enc_actor_spawn_fn(PacketWriter* w, const void* m){return encode_msg_actor_spawn(w,(const MsgActorSpawn*)m);}
static bool enc_object_spawn_fn(PacketWriter* w, const void* m){return encode_msg_object_spawn(w,(const MsgObjectSpawn*)m);}
static bool enc_object_update_fn(PacketWriter* w, const void* m){return encode_msg_object_update(w,(const MsgObjectUpdate*)m);}
static bool enc_damage_fn(PacketWriter* w, const void* m){return encode_msg_damage_result(w,(const MsgDamageResult*)m);}
static bool enc_death_fn(PacketWriter* w, const void* m){return encode_msg_death_event(w,(const MsgDeathEvent*)m);}
static bool enc_dialogue_open_fn(PacketWriter* w, const void* m){return encode_msg_dialogue_open(w,(const MsgDialogueOpen*)m);}
static bool enc_dialogue_line_fn(PacketWriter* w, const void* m){ const MsgDialogueLine* h=(const MsgDialogueLine*)m; const char* txt=(const char*)(h+1); return encode_msg_dialogue_line(w,h,txt);}
static bool enc_dialogue_choices_fn(PacketWriter* w, const void* m){ const MsgDialogueChoicesHeader* h=(const MsgDialogueChoicesHeader*)m; const MsgDialogueChoiceRecord* c=(const MsgDialogueChoiceRecord*)(h+1); const char* const* texts=(const char* const*)(c+h->choice_count); return encode_msg_dialogue_choices(w,h,c,texts);}
static bool enc_dialogue_close_fn(PacketWriter* w, const void* m){return encode_msg_dialogue_close(w,(const MsgDialogueClose*)m);}
static bool enc_quest_update_fn(PacketWriter* w, const void* m){ const MsgQuestUpdateHeader* h=(const MsgQuestUpdateHeader*)m; const MsgQuestObjectiveRecord* o=(const MsgQuestObjectiveRecord*)(h+1); const char* const* texts=(const char* const*)(o+h->objective_count); return encode_msg_quest_update(w,h,o,texts);}

int main(void){
    uint8_t pkt[1400]; size_t len;
    proxy_runtime_init();
    MsgProfileSnapshot ps={0}; ps.level=7; ps.xp=42; ps.equipped_form_id=0x1234u; ps.caps=99; ps.str_stat=6;
    len = make_packet(pkt, MSG_PROFILE_SNAPSHOT, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH0_CONTROL, &ps, enc_profile);
    assert(plugin_apply_packet(pkt,len));
    assert(proxy_runtime_get_profile()->loaded);
    assert(proxy_runtime_get_profile()->snapshot.level==7);
    assert(proxy_runtime_get_profile()->snapshot.equipped_form_id==0x1234u);
    assert(proxy_runtime_get_profile()->snapshot.caps==99);

    { struct { MsgProfileItemsHeader h; MsgProfileItemRecord items[2]; } pim = {0};
      pim.h.item_count = 2;
      pim.items[0].form_id = 0x1000u; pim.items[0].count = 1; pim.items[0].equipped = 1;
      pim.items[1].form_id = 0x2000u; pim.items[1].count = 12; pim.items[1].equipped = 0;
      len = make_packet(pkt, MSG_PROFILE_ITEMS, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH0_CONTROL, &pim, enc_profile_items);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_profile()->item_count==2);
      assert(proxy_runtime_get_profile()->items[0].form_id==0x1000u);
      assert(proxy_runtime_get_profile()->items[1].count==12); }

    { struct { MsgProfilePerksHeader h; MsgProfilePerkRecord perks[2]; } ppk = {0};
      ppk.h.perk_count = 2;
      ppk.perks[0].perk_form_id = 0x3000u; ppk.perks[0].rank = 2;
      ppk.perks[1].perk_form_id = 0x4000u; ppk.perks[1].rank = 1;
      len = make_packet(pkt, MSG_PROFILE_PERKS, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH0_CONTROL, &ppk, enc_profile_perks);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_profile()->perk_count==2);
      assert(proxy_runtime_get_profile()->perks[0].perk_form_id==0x3000u); }

    { MsgProfileAppearance pa = {0}; pa.sex = 1; pa.body_preset = 7;
      len = make_packet(pkt, MSG_PROFILE_APPEARANCE, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH0_CONTROL, &pa, enc_profile_appearance);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_profile()->appearance.sex==1);
      assert(proxy_runtime_get_profile()->appearance.body_preset==7); }

    MsgPlayerState p={0}; p.player_id=2; p.position.x=1.0f; p.anim_state=9;
    len = make_packet(pkt, MSG_PLAYER_STATE, MSGF_CH1_WORLD, &p, enc_player);
    assert(plugin_apply_packet(pkt,len));
    assert(proxy_runtime_player_count()==1);
    assert(proxy_runtime_get_player(2)->anim_state==9);

    { MsgActorSpawn asp = {0}; ProxyActorSpawnSpec tmp = {0}; (void)tmp; asp.actor_net_id=77; asp.base_form_id=0x7777u;
      len = make_packet(pkt, MSG_ACTOR_SPAWN, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH1_WORLD, &asp, enc_actor_spawn_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_actor(77) != NULL); }

    MsgObjectSpawn os={0}; os.object_net_id=55; os.form_id=0xAAAAAAAAu; os.position.y=3.0f;
    len = make_packet(pkt, MSG_OBJECT_SPAWN, MSGF_CH3_WORK, &os, enc_object_spawn_fn);
    assert(plugin_apply_packet(pkt,len));
    assert(proxy_runtime_object_count()==1);

    MsgObjectUpdate ou={0}; ou.object_net_id=55; ou.fields_mask=OUF_POSITION|OUF_STATE_FLAGS; ou.position.y=9.0f; ou.state_flags=OBJF_MOVED;
    len = make_packet(pkt, MSG_OBJECT_UPDATE, MSGF_CH3_WORK, &ou, enc_object_update_fn);
    assert(plugin_apply_packet(pkt,len));
    assert(proxy_runtime_get_object(55)->position.y==9.0f);


    { MsgDamageResult dr={0}; dr.attacker_id=2; dr.victim_actor_net_id=77; dr.damage_amount=0.25f; dr.health_after=0.75f;
      len = make_packet(pkt, MSG_DAMAGE_RESULT, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH2_COMBAT, &dr, enc_damage_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_combat()->loaded);
      assert(proxy_runtime_get_combat()->last_damage.victim_actor_net_id==77);
      assert(proxy_runtime_get_actor(77)->last_damage_amount==0.25f); }

    { MsgDeathEvent de={0}; de.victim_actor_net_id=77; de.killer_id=2;
      len = make_packet(pkt, MSG_DEATH_EVENT, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH2_COMBAT, &de, enc_death_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_combat()->has_last_death);
      assert(proxy_runtime_get_combat()->last_death.victim_actor_net_id==77);
      assert(proxy_runtime_get_actor(77)->is_dead); }

    { MsgDialogueOpen d={0}; d.dialogue_id=500; d.npc_actor_net_id=77; d.speaker_actor_net_id=77;
      len = make_packet(pkt, MSG_DIALOGUE_OPEN, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &d, enc_dialogue_open_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_dialogue()->active);
      assert(proxy_runtime_get_dialogue()->dialogue_id==500); }

    { struct { MsgDialogueLine h; char text[32]; } dl = {0}; dl.h.dialogue_id=500; dl.h.speaker_actor_net_id=77; strcpy(dl.text, "Welcome home");
      len = make_packet(pkt, MSG_DIALOGUE_LINE, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &dl, enc_dialogue_line_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(strcmp(proxy_runtime_get_dialogue()->current_line, "Welcome home")==0); }

    { struct { MsgDialogueChoicesHeader h; MsgDialogueChoiceRecord c[2]; const char* texts[2]; } dc = {0};
      dc.h.dialogue_id=500; dc.h.choice_count=2; dc.c[0].choice_id=1; dc.c[0].enabled=1; dc.c[1].choice_id=2; dc.c[1].enabled=0; dc.texts[0]="Yes"; dc.texts[1]="No";
      len = make_packet(pkt, MSG_DIALOGUE_CHOICES, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &dc, enc_dialogue_choices_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_dialogue()->choice_count==2);
      assert(strcmp(proxy_runtime_get_dialogue()->choices[0].text, "Yes")==0); }

    { MsgDialogueClose dc={0}; dc.dialogue_id=500; dc.reason=DLG_CLOSE_ENDED;
      len = make_packet(pkt, MSG_DIALOGUE_CLOSE, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &dc, enc_dialogue_close_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(!proxy_runtime_get_dialogue()->active); }

    { MsgDialogueOpen d2={0}; d2.dialogue_id=501; d2.npc_actor_net_id=77; d2.speaker_actor_net_id=77;
      len = make_packet(pkt, MSG_DIALOGUE_OPEN, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &d2, enc_dialogue_open_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_dialogue()->active);
      MsgDeathEvent de2={0}; de2.victim_actor_net_id=77; de2.killer_id=2;
      len = make_packet(pkt, MSG_DEATH_EVENT, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH2_COMBAT, &de2, enc_death_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(!proxy_runtime_get_dialogue()->active);
      assert(proxy_runtime_get_dialogue()->close_reason==DLG_CLOSE_INTERRUPTED); }

    { struct { MsgQuestUpdateHeader h; MsgQuestObjectiveRecord o[2]; const char* texts[2]; } qu = {0};
      qu.h.quest_id=42; qu.h.stage=10; qu.h.tracked=1; qu.h.objective_count=2; qu.o[0].objective_id=100; qu.o[0].state=QOBJ_ACTIVE; qu.o[1].objective_id=101; qu.o[1].state=QOBJ_COMPLETED; qu.texts[0]="Go to Concord"; qu.texts[1]="Talk to Preston";
      len = make_packet(pkt, MSG_QUEST_UPDATE, MSGF_RELIABLE|MSGF_ORDERED|MSGF_CH4_UI, &qu, enc_quest_update_fn);
      assert(plugin_apply_packet(pkt,len));
      assert(proxy_runtime_get_quest()->loaded);
      assert(proxy_runtime_get_quest()->quest_id==42);
      assert(strcmp(proxy_runtime_get_quest()->objectives[0].text, "Go to Concord")==0); }

    return 0;
}


/* dialogue / quest mirror */
