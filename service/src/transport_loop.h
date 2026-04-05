#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "service_state_models.h"
#include "transport_session_state.h"
#include "packet_header.h"
#include "messages_ui.h"

typedef struct TransportEnvelope {
    uint32_t connection_id;
    size_t   length;
    uint8_t  bytes[1400];
} TransportEnvelope;

typedef struct TransportOutput {
    uint32_t connection_id;
    size_t   length;
    uint8_t  bytes[1400];
} TransportOutput;

typedef struct TransportLoopContext {
    TransportSessionState transport;
    SessionState*         session;
    TransportOutput       outputs[64];
    size_t                output_count;
    bool                  has_pending_dialogue_select;
    PlayerId              pending_dialogue_player_id;
    MsgDialogueSelect     pending_dialogue_select;
    uint32_t              current_tick;
    uint32_t              player_timeout_ticks;
} TransportLoopContext;

void tl_init(TransportLoopContext* ctx, SessionState* session);
bool tl_handle_envelope(TransportLoopContext* ctx, const TransportEnvelope* env);
void tl_tick(TransportLoopContext* ctx);
void tl_set_player_timeout_ticks(TransportLoopContext* ctx, uint32_t timeout_ticks);
uint32_t tl_get_current_tick(const TransportLoopContext* ctx);
size_t tl_get_output_count(const TransportLoopContext* ctx);
const TransportOutput* tl_get_output(const TransportLoopContext* ctx, size_t index);
void tl_clear_outputs(TransportLoopContext* ctx);


bool tl_broadcast_dialogue_open(TransportLoopContext* ctx, const MsgDialogueOpen* msg);
bool tl_broadcast_dialogue_line(TransportLoopContext* ctx, const MsgDialogueLine* msg, const char* text);
bool tl_broadcast_dialogue_choices(TransportLoopContext* ctx, const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, const char* const* texts);
bool tl_broadcast_dialogue_close(TransportLoopContext* ctx, const MsgDialogueClose* msg);
bool tl_broadcast_quest_update(TransportLoopContext* ctx, const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, const char* const* texts);


bool tl_try_pop_dialogue_select(TransportLoopContext* ctx, PlayerId* out_player_id, MsgDialogueSelect* out_select);
