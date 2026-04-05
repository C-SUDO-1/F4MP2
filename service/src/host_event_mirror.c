#include "host_event_mirror.h"
#include "quest_mirror_policy.h"
#include "dialogue_progression_policy.h"
#include <string.h>

bool hem_dialogue_open(TransportLoopContext* ctx, DialogueId dialogue_id, ActorNetId npc_actor_net_id, ActorNetId speaker_actor_net_id) {
    MsgDialogueOpen msg;
    if (!ctx || !ctx->session) return false;
    memset(&msg, 0, sizeof(msg));
    msg.dialogue_id = dialogue_id;
    msg.npc_actor_net_id = npc_actor_net_id;
    msg.speaker_actor_net_id = speaker_actor_net_id;
    memset(&ctx->session->dialogue, 0, sizeof(ctx->session->dialogue));
    ctx->session->dialogue.active = true;
    ctx->session->dialogue.dialogue_id = dialogue_id;
    ctx->session->dialogue.npc_actor_net_id = npc_actor_net_id;
    ctx->session->dialogue.speaker_actor_net_id = speaker_actor_net_id;
    return tl_broadcast_dialogue_open(ctx, &msg);
}

bool hem_dialogue_line(TransportLoopContext* ctx, DialogueId dialogue_id, ActorNetId speaker_actor_net_id, const char* text) {
    MsgDialogueLine msg;
    if (!ctx || !ctx->session) return false;
    memset(&msg, 0, sizeof(msg));
    msg.dialogue_id = dialogue_id;
    msg.speaker_actor_net_id = speaker_actor_net_id;
    ctx->session->dialogue.active = true;
    ctx->session->dialogue.dialogue_id = dialogue_id;
    ctx->session->dialogue.speaker_actor_net_id = speaker_actor_net_id;
    strncpy(ctx->session->dialogue.current_line, text ? text : "", sizeof(ctx->session->dialogue.current_line) - 1);
    return tl_broadcast_dialogue_line(ctx, &msg, text);
}

bool hem_dialogue_choices(TransportLoopContext* ctx, DialogueId dialogue_id, const MsgDialogueChoiceRecord* choices, const char* const* texts, uint16_t choice_count) {
    MsgDialogueChoicesHeader h;
    uint16_t i, count;
    if (!ctx || !ctx->session) return false;
    memset(&h, 0, sizeof(h));
    h.dialogue_id = dialogue_id;
    h.choice_count = choice_count;
    count = (choice_count < F4MP_MAX_DIALOGUE_CHOICES) ? choice_count : F4MP_MAX_DIALOGUE_CHOICES;
    ctx->session->dialogue.dialogue_id = dialogue_id;
    ctx->session->dialogue.choice_count = count;
    for (i = 0; i < count; ++i) {
        ctx->session->dialogue.choices[i].choice_id = choices[i].choice_id;
        ctx->session->dialogue.choices[i].enabled = choices[i].enabled;
        strncpy(ctx->session->dialogue.choices[i].text, texts ? texts[i] : "", sizeof(ctx->session->dialogue.choices[i].text) - 1);
    }
    return tl_broadcast_dialogue_choices(ctx, &h, choices, texts);
}

bool hem_dialogue_close(TransportLoopContext* ctx, DialogueId dialogue_id, uint8_t reason) {
    MsgDialogueClose msg;
    if (!ctx || !ctx->session) return false;
    memset(&msg, 0, sizeof(msg));
    msg.dialogue_id = dialogue_id;
    msg.reason = reason;
    if (ctx->session->dialogue.dialogue_id == dialogue_id) {
        ctx->session->dialogue.active = false;
        ctx->session->dialogue.close_reason = reason;
        ctx->session->dialogue.choice_count = 0;
        ctx->session->dialogue.current_line[0] = '\0';
    }
    return tl_broadcast_dialogue_close(ctx, &msg);
}

bool hem_quest_update(TransportLoopContext* ctx, QuestId quest_id, uint16_t stage, uint8_t tracked, const MsgQuestObjectiveRecord* objs, const char* const* texts, uint8_t objective_count) {
    MsgQuestUpdateHeader h;
    QuestMirrorPolicyResult pr;
    uint8_t i, count;
    if (!ctx || !ctx->session) return false;
    memset(&h, 0, sizeof(h));
    h.quest_id = quest_id;
    h.stage = stage;
    h.tracked = tracked;
    h.objective_count = objective_count;
    if (!qmp_validate_update(ctx->session, &h, objs, &pr)) return false;
    memset(&ctx->session->quest, 0, sizeof(ctx->session->quest));
    ctx->session->quest.loaded = true;
    ctx->session->quest.quest_id = quest_id;
    ctx->session->quest.stage = stage;
    ctx->session->quest.tracked = tracked;
    count = (objective_count < F4MP_MAX_QUEST_OBJECTIVES) ? objective_count : F4MP_MAX_QUEST_OBJECTIVES;
    ctx->session->quest.objective_count = count;
    for (i = 0; i < count; ++i) {
        ctx->session->quest.objectives[i].objective_id = objs[i].objective_id;
        ctx->session->quest.objectives[i].state = objs[i].state;
        strncpy(ctx->session->quest.objectives[i].text, texts ? texts[i] : "", sizeof(ctx->session->quest.objectives[i].text) - 1);
    }
    return tl_broadcast_quest_update(ctx, &h, objs, texts);
}

bool hem_try_pop_dialogue_select(TransportLoopContext* ctx, PlayerId* out_player_id, MsgDialogueSelect* out_select) {
    return tl_try_pop_dialogue_select(ctx, out_player_id, out_select);
}

bool hem_apply_guest_dialogue_select(TransportLoopContext* ctx, PlayerId player_id, const MsgDialogueSelect* select) {
    DialogueProgressionOutput out;
    if (!ctx || !ctx->session || !select) return false;
    if (!dpp_apply_selection(ctx->session, player_id, select, &out)) return false;
    if (out.result == DPR_ADVANCED_LINE) {
        if (!hem_dialogue_line(ctx, out.line.dialogue_id, out.line.speaker_actor_net_id, out.line_text)) return false;
        return hem_dialogue_close(ctx, out.close.dialogue_id, out.close.reason);
    }
    if (out.result == DPR_CLOSE_DIALOGUE) {
        return hem_dialogue_close(ctx, out.close.dialogue_id, out.close.reason);
    }
    return false;
}
