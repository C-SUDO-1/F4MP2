#include "remote_proxy_manager.h"
#include "remote_proxy_backend.h"
#include "remote_proxy_update_policy.h"
#include "fo4_scene_proxy_backend_stub.h"
#include <string.h>

static PlayerId g_local_player_id = 0;
static uint32_t g_rejected_stale_updates = 0;

void rpm_init(PlayerId local_player_id) {
    g_local_player_id = local_player_id;
    rpb_reset_default_backend();
    rpup_init_default();
    g_rejected_stale_updates = 0;
}

bool rpm_apply_remote_player_state(const MsgPlayerState* msg) {
    ProxyPlayerSpawnSpec sp;
    ProxyPlayerUpdateSpec up;

    if (!msg) return false;
    if (msg->player_id == 0) return true;
    if (msg->player_id == g_local_player_id) return true;

    {
        const ProxyPlayerRecord* current = proxy_runtime_get_player(msg->player_id);
        if (!current) {
            memset(&sp, 0, sizeof(sp));
            sp.player_id = msg->player_id;
            sp.proxy_net_id = msg->player_id;
            sp.proxy_base_form_id = 0;
            sp.position = msg->position;
            sp.rotation = msg->rotation;
            sp.equipped_form_id = msg->equipped_form_id;
            if (!rpb_get_backend()->spawn_player(rpb_get_backend()->user, &sp)) return false;
            current = proxy_runtime_get_player(msg->player_id);
        }
        if (current) {
            if (msg->state_sequence != 0 && current->last_state_sequence != 0 && msg->state_sequence < current->last_state_sequence) {
                g_rejected_stale_updates++;
                return true;
            }
            if (msg->state_sequence == current->last_state_sequence && msg->observed_tick != 0 && current->last_observed_tick != 0 && msg->observed_tick <= current->last_observed_tick) {
                g_rejected_stale_updates++;
                return true;
            }
        }
    }

    memset(&up, 0, sizeof(up));
    up.player_id = msg->player_id;
    up.position = msg->position;
    up.rotation = msg->rotation;
    up.velocity = msg->velocity;
    up.stance = msg->stance;
    up.anim_state = msg->anim_state;
    up.equipped_form_id = msg->equipped_form_id;
    {
        RemoteProxyUpdatePolicyResult policy_result;
        const ProxyPlayerRecord* current = proxy_runtime_get_player(msg->player_id);
        memset(&policy_result, 0, sizeof(policy_result));
        if (current) {
            rpup_apply_player_update_ex(current, &up, &policy_result);
            fo4_scene_proxy_backend_stub_note_policy_result(msg->player_id,
                policy_result.teleported,
                policy_result.smoothed_position,
                policy_result.smoothed_rotation,
                policy_result.source_distance);
        }
    }
    if (!rpb_get_backend()->update_player(rpb_get_backend()->user, &up)) return false;
    {
        const ProxyPlayerRecord* updated = proxy_runtime_get_player(msg->player_id);
        if (updated) {
            ProxyPlayerRecord* mutable_updated = (ProxyPlayerRecord*)updated;
            mutable_updated->last_state_sequence = msg->state_sequence;
            mutable_updated->last_observed_tick = msg->observed_tick;
        }
    }
    return true;
}

bool rpm_remove_remote_player(PlayerId player_id) {
    if (player_id == g_local_player_id) return true;
    return rpb_get_backend()->despawn_player(rpb_get_backend()->user, player_id);
}

const ProxyPlayerRecord* rpm_get_remote_player(PlayerId player_id) {
    if (player_id == g_local_player_id) return NULL;
    return proxy_runtime_get_player(player_id);
}

uint32_t rpm_rejected_stale_updates(void) { return g_rejected_stale_updates; }
void rpm_reset_rejected_stale_updates(void) { g_rejected_stale_updates = 0; }
