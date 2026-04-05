#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "transport_loop.h"
#include "host_event_mirror.h"
#include "packet_codec.h"
#include "message_ids.h"

static size_t finalize(PacketWriter* w, size_t payload_start) {
    uint16_t payload_len = (uint16_t)(pw_bytes_written(w) - payload_start);
    memcpy(w->buffer + sizeof(PacketHeader) + offsetof(MessageHeader, length), &payload_len, sizeof(payload_len));
    return pw_bytes_written(w);
}

int main(void) {
    SessionState s;
    TransportLoopContext ctx;
    TransportEnvelope env;
    PacketWriter w;
    PacketHeader ph = {0};
    MessageHeader mh = {0};
    MsgDialogueSelect sel = {0};
    MsgDialogueSelect out = {0};
    PlayerId out_player = 0;
    size_t start;

    memset(&s, 0, sizeof(s));
    s.in_use = true;
    s.session_id = 1;
    s.tick_rate = 20;
    s.players[0].connected = true;
    s.players[0].player_id = 1;
    s.players[0].connection_id = 111;
    s.dialogue.active = true;
    s.dialogue.dialogue_id = 500;
    s.dialogue.choice_count = 1;
    s.dialogue.choices[0].choice_id = 2;
    s.dialogue.choices[0].enabled = 1;

    tl_init(&ctx, &s);

    memset(&env, 0, sizeof(env));
    env.connection_id = 111;
    ph.magic = F4MP_MAGIC;
    ph.version = F4MP_VERSION;
    ph.message_count = 1;
    mh.type = MSG_DIALOGUE_SELECT;
    mh.flags = MSGF_RELIABLE | MSGF_ORDERED | MSGF_CH4_UI;
    sel.dialogue_id = 500;
    sel.choice_id = 2;

    pw_init(&w, env.bytes, sizeof(env.bytes));
    assert(encode_packet_header(&w, &ph));
    assert(encode_message_header(&w, &mh));
    start = pw_bytes_written(&w);
    assert(encode_msg_dialogue_select(&w, &sel));
    env.length = finalize(&w, start);

    assert(tl_handle_envelope(&ctx, &env));
    assert(hem_try_pop_dialogue_select(&ctx, &out_player, &out));
    assert(out_player == 1);
    assert(out.dialogue_id == 500);
    assert(out.choice_id == 2);
    assert(s.dialogue.last_selected_choice_id == 2);
    assert(s.dialogue.last_selected_by_player == 1);
    assert(!hem_try_pop_dialogue_select(&ctx, &out_player, &out));
    return 0;
}
