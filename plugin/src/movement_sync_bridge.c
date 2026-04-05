#include "movement_sync_bridge.h"
#include <math.h>
#include <string.h>
#include "packet_writer.h"
#include "packet_codec.h"
#include "message_ids.h"

static MovementSyncBridgeConfig g_cfg;
static bool g_have_last = false;
static LocalPlayerObservedState g_last;
static uint32_t g_quiet_ticks = 0;
static uint32_t g_state_sequence = 0;
static uint32_t g_observed_tick = 0;
static uint32_t g_last_emit_tick = 0;

static float sqrf(float v) { return v * v; }
static float pos_dist_sq(Vec3f a, Vec3f b) {
    return sqrf(a.x - b.x) + sqrf(a.y - b.y) + sqrf(a.z - b.z);
}

void msb_init(const MovementSyncBridgeConfig* cfg) {
    memset(&g_cfg, 0, sizeof(g_cfg));
    g_cfg.position_epsilon = 0.02f;
    g_cfg.rotation_epsilon = 1.0f;
    g_cfg.send_velocity = true;
    g_cfg.force_resend_ticks = 20;
    g_cfg.min_send_interval_ticks = 2;
    if (cfg) g_cfg = *cfg;
    g_have_last = false;
    g_quiet_ticks = 0;
    g_state_sequence = 0;
    g_observed_tick = 0;
    g_last_emit_tick = 0;
    memset(&g_last, 0, sizeof(g_last));
}

bool msb_build_player_state_if_changed(MsgPlayerState* out_msg) {
    LocalPlayerObservedState cur;
    float pos_eps2;
    if (!out_msg) return false;
    if (!lpo_capture(&cur)) return false;
    g_observed_tick++;

    pos_eps2 = g_cfg.position_epsilon * g_cfg.position_epsilon;
    if (g_have_last) {
        const bool pos_changed = pos_dist_sq(cur.position, g_last.position) > pos_eps2;
        const bool rot_changed = fabsf(cur.rotation.yaw - g_last.rotation.yaw) > g_cfg.rotation_epsilon ||
                                 fabsf(cur.rotation.pitch - g_last.rotation.pitch) > g_cfg.rotation_epsilon ||
                                 fabsf(cur.rotation.roll - g_last.rotation.roll) > g_cfg.rotation_epsilon;
        const bool stance_changed = cur.stance != g_last.stance || cur.anim_state != g_last.anim_state;
        const bool equip_changed = cur.equipped_form_id != g_last.equipped_form_id;
        const bool changed = pos_changed || rot_changed || stance_changed || equip_changed;
        if (changed && g_cfg.min_send_interval_ticks > 0 && g_last_emit_tick != 0 && (g_observed_tick - g_last_emit_tick) < g_cfg.min_send_interval_ticks) {
            return false;
        }
        if (!changed) {
            if (g_cfg.force_resend_ticks == 0) return false;
            g_quiet_ticks++;
            if (g_quiet_ticks < g_cfg.force_resend_ticks) return false;
        }
    }

    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->player_id = cur.player_id;
    out_msg->position = cur.position;
    out_msg->rotation = cur.rotation;
    out_msg->velocity = g_cfg.send_velocity ? cur.velocity : (Vec3f){0,0,0};
    out_msg->stance = cur.stance;
    out_msg->anim_state = cur.anim_state;
    out_msg->equipped_form_id = cur.equipped_form_id;

    g_state_sequence++;
    out_msg->state_sequence = g_state_sequence;
    out_msg->observed_tick = g_observed_tick;
    g_last = cur;
    g_have_last = true;
    g_quiet_ticks = 0;
    g_last_emit_tick = g_observed_tick;
    return true;
}

MovementSyncBridgeConfig msb_get_config(void) { return g_cfg; }

uint32_t msb_last_state_sequence(void) { return g_state_sequence; }
uint32_t msb_last_observed_tick(void) { return g_observed_tick; }

size_t msb_encode_player_state_packet(
    uint8_t* buffer,
    size_t capacity,
    uint32_t session_id,
    uint32_t sequence,
    const MsgPlayerState* msg
) {
    PacketWriter w;
    PacketHeader ph;
    MessageHeader mh;
    size_t header_pos;
    size_t payload_start;

    if (!buffer || !msg || capacity < sizeof(PacketHeader) + sizeof(MessageHeader) + sizeof(MsgPlayerState)) return 0;

    pw_init(&w, buffer, capacity);
    memset(&ph, 0, sizeof(ph));
    ph.magic = F4MP_MAGIC;
    ph.version = F4MP_VERSION;
    ph.session_id = session_id;
    ph.sequence = sequence;
    ph.message_count = 1;
    if (!encode_packet_header(&w, &ph)) return 0;

    header_pos = pw_bytes_written(&w);
    memset(&mh, 0, sizeof(mh));
    mh.type = MSG_PLAYER_STATE;
    mh.flags = MSGF_RELIABLE | MSGF_CH1_WORLD;
    mh.length = 0;
    if (!encode_message_header(&w, &mh)) return 0;

    payload_start = pw_bytes_written(&w);
    if (!encode_msg_player_state(&w, msg)) return 0;

    mh.length = (uint16_t)(pw_bytes_written(&w) - payload_start);
    memcpy(buffer + header_pos, &mh, sizeof(mh));
    return pw_bytes_written(&w);
}
