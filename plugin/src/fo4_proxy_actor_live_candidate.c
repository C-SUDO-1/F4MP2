#include "fo4_proxy_actor_live_candidate.h"

#include <string.h>

#include "fo4_proxy_actor_driver.h"

static Fo4ProxyActorLiveCandidateState g_state;

static Fo4ProxyActorLiveCandidateConfig default_config(void) {
    Fo4ProxyActorLiveCandidateConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.actor_class_name = "Actor";
    cfg.create_api_symbol = "TESObjectREFR::Create";
    cfg.move_api_symbol = "TESObjectREFR::SetPosition";
    cfg.rotate_api_symbol = "TESObjectREFR::SetRotation";
    cfg.despawn_api_symbol = "TESObjectREFR::Disable";
    return cfg;
}

void fpalc_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

void fpalc_configure(const Fo4ProxyActorLiveCandidateConfig* config) {
    Fo4ProxyActorSpawnContractConfig spawn_cfg;
    Fo4ProxyActorLiveCandidateConfig active = default_config();
    fpalc_reset();
    if (config) active = *config;
    if (!active.actor_class_name) active.actor_class_name = "Actor";
    if (!active.create_api_symbol) active.create_api_symbol = "TESObjectREFR::Create";
    if (!active.move_api_symbol) active.move_api_symbol = "TESObjectREFR::SetPosition";
    if (!active.rotate_api_symbol) active.rotate_api_symbol = "TESObjectREFR::SetRotation";
    if (!active.despawn_api_symbol) active.despawn_api_symbol = "TESObjectREFR::Disable";

    memset(&spawn_cfg, 0, sizeof(spawn_cfg));
    spawn_cfg.local_player_id = active.local_player_id;
    spawn_cfg.default_proxy_base_form_id = active.default_proxy_base_form_id;
    spawn_cfg.suppress_self_spawn = active.suppress_self_spawn;
    spawn_cfg.require_base_form_id = active.require_base_form_id;
    fpasc_configure(&spawn_cfg);

    g_state.configured = true;
    g_state.enabled = true;
    g_state.actor_class_name = active.actor_class_name;
    g_state.create_api_symbol = active.create_api_symbol;
    g_state.move_api_symbol = active.move_api_symbol;
    g_state.rotate_api_symbol = active.rotate_api_symbol;
    g_state.despawn_api_symbol = active.despawn_api_symbol;
}

bool fpalc_enabled(void) {
    return g_state.enabled;
}

bool fpalc_spawn_via_driver(const ProxyPlayerSpawnSpec* spec, uint64_t actor_handle) {
    const Fo4ProxyActorState* driver_state;
    if (!g_state.enabled || !spec) return false;
    if (!fpasc_spawn_via_driver(spec, actor_handle)) return false;
    driver_state = fpad_get_state(spec->player_id);
    g_state.spawn_count++;
    g_state.last_player_id = spec->player_id;
    g_state.last_actor_handle = driver_state ? driver_state->actor_handle : actor_handle;
    g_state.last_proxy_base_form_id = driver_state ? driver_state->proxy_base_form_id : 0;
    g_state.has_driver_state = driver_state != 0;
    return true;
}

bool fpalc_apply_presented_state(const MsgPlayerState* msg, uint64_t actor_handle) {
    Fo4ProxyActorMoveSpec move_spec;
    Fo4ProxyActorRotateSpec rotate_spec;
    const Fo4ProxyActorState* driver_state;
    if (!g_state.enabled || !msg || msg->player_id == 0) return false;
    memset(&move_spec, 0, sizeof(move_spec));
    memset(&rotate_spec, 0, sizeof(rotate_spec));
    move_spec.player_id = msg->player_id;
    move_spec.actor_handle = actor_handle;
    move_spec.position = msg->position;
    move_spec.velocity = msg->velocity;
    move_spec.stance = msg->stance;
    move_spec.anim_state = msg->anim_state;
    move_spec.equipped_form_id = msg->equipped_form_id;
    rotate_spec.player_id = msg->player_id;
    rotate_spec.actor_handle = actor_handle;
    rotate_spec.rotation = msg->rotation;
    if (!fpad_move(&move_spec)) return false;
    if (!fpad_rotate(&rotate_spec)) return false;
    driver_state = fpad_get_state(msg->player_id);
    g_state.move_count++;
    g_state.rotate_count++;
    g_state.last_player_id = msg->player_id;
    g_state.last_actor_handle = driver_state ? driver_state->actor_handle : actor_handle;
    g_state.last_proxy_base_form_id = driver_state ? driver_state->proxy_base_form_id : 0;
    g_state.has_driver_state = driver_state != 0;
    return true;
}

bool fpalc_despawn(PlayerId player_id) {
    const Fo4ProxyActorState* driver_state;
    if (!g_state.enabled || player_id == 0) return false;
    if (!fpad_despawn(player_id)) return false;
    driver_state = fpad_get_state(player_id);
    g_state.despawn_count++;
    g_state.last_player_id = player_id;
    g_state.last_actor_handle = driver_state ? driver_state->actor_handle : 0;
    g_state.last_proxy_base_form_id = driver_state ? driver_state->proxy_base_form_id : 0;
    g_state.has_driver_state = driver_state != 0;
    return true;
}

Fo4ProxyActorLiveCandidateState fpalc_state(void) {
    return g_state;
}
