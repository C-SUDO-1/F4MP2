#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "fo4_proxy_actor_spawn_contract.h"
#include "fo4_proxy_actor_live_candidate.h"
#include <string.h>

static struct {
    Fo4SceneProxyPlayerState players[FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS];
    uint64_t next_handle;
    size_t spawn_count;
    size_t update_count;
    size_t despawn_count;
    uint32_t total_command_sequence;
    Fo4SceneProxyInterpolationConfig interp;
} g_scene;

static Fo4SceneProxyPlayerState* find_slot(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS; ++i) {
        if (g_scene.players[i].in_use && g_scene.players[i].player_id == player_id) return &g_scene.players[i];
    }
    for (i = 0; i < FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS; ++i) {
        if (!g_scene.players[i].in_use) return &g_scene.players[i];
    }
    return 0;
}

static float clamp01(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

static float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

static bool same_vec3(Vec3f a, Vec3f b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static bool same_rot3(Rot3f a, Rot3f b) {
    return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll;
}

static void sync_scene_actor_handle(Fo4SceneProxyPlayerState* s) {
    const Fo4ProxyActorState* driver_state;
    if (!s) return;
    driver_state = fpad_get_state(s->player_id);
    if (driver_state && driver_state->actor_handle) {
        s->actor_handle = driver_state->actor_handle;
    }
}

static bool apply_driver_presented_state(Fo4SceneProxyPlayerState* s, const ProxyPlayerUpdateSpec* spec) {
    Fo4ProxyActorMoveSpec move_spec;
    Fo4ProxyActorRotateSpec rotate_spec;
    if (!s || !spec) return false;
    memset(&move_spec, 0, sizeof(move_spec));
    memset(&rotate_spec, 0, sizeof(rotate_spec));

    move_spec.player_id = s->player_id;
    move_spec.actor_handle = s->actor_handle;
    move_spec.position = s->presented_position;
    move_spec.velocity = spec->velocity;
    move_spec.stance = spec->stance;
    move_spec.anim_state = spec->anim_state;
    move_spec.equipped_form_id = spec->equipped_form_id;

    rotate_spec.player_id = s->player_id;
    rotate_spec.actor_handle = s->actor_handle;
    rotate_spec.rotation = s->presented_rotation;

    if (fpalc_enabled()) {
        MsgPlayerState msg;
        memset(&msg, 0, sizeof(msg));
        msg.player_id = s->player_id;
        msg.position = s->presented_position;
        msg.rotation = s->presented_rotation;
        msg.velocity = spec->velocity;
        msg.stance = spec->stance;
        msg.anim_state = spec->anim_state;
        msg.equipped_form_id = spec->equipped_form_id;
        if (!fpalc_apply_presented_state(&msg, s->actor_handle)) return false;
    } else {
        if (!fpad_move(&move_spec)) return false;
        if (!fpad_rotate(&rotate_spec)) return false;
    }
    sync_scene_actor_handle(s);
    return true;
}

static bool ensure_scene_player_from_spawn(Fo4SceneProxyPlayerState* s, const ProxyPlayerSpawnSpec* spec) {
    if (!s || !spec) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->player_id = spec->player_id;
    s->actor_handle = ++g_scene.next_handle;
    s->visible = true;
    s->last_spawn = *spec;
    s->presented_position = spec->position;
    s->presented_rotation = spec->rotation;
    s->target_position = spec->position;
    s->target_rotation = spec->rotation;
    s->interpolation_pending = false;

    if (fpalc_enabled()) {
        if (!fpalc_spawn_via_driver(spec, s->actor_handle)) return false;
    } else {
        if (!fpasc_spawn_via_driver(spec, s->actor_handle)) return false;
    }
    sync_scene_actor_handle(s);
    return true;
}

static bool scene_spawn(void* user, const ProxyPlayerSpawnSpec* spec) {
    Fo4SceneProxyPlayerState* s;
    (void)user;
    if (!spec) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    if (!ensure_scene_player_from_spawn(s, spec)) return false;
    if (!proxy_spawn_player(spec)) return false;
    s->last_command = FSP_CMD_SPAWN;
    s->command_sequence = ++g_scene.total_command_sequence;
    g_scene.spawn_count++;
    return true;
}

static bool scene_update(void* user, const ProxyPlayerUpdateSpec* spec) {
    Fo4SceneProxyPlayerState* s;
    ProxyPlayerSpawnSpec spawn_spec;
    (void)user;
    if (!spec) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    if (!s->in_use) {
        memset(&spawn_spec, 0, sizeof(spawn_spec));
        spawn_spec.player_id = spec->player_id;
        spawn_spec.proxy_net_id = spec->player_id;
        spawn_spec.position = spec->position;
        spawn_spec.rotation = spec->rotation;
        spawn_spec.equipped_form_id = spec->equipped_form_id;
        if (!ensure_scene_player_from_spawn(s, &spawn_spec)) return false;
    }
    s->last_update = *spec;
    s->target_position = spec->position;
    s->target_rotation = spec->rotation;
    s->interpolation_pending = false;
    if (s->last_update_teleported || !g_scene.interp.enabled) {
        s->presented_position = spec->position;
        s->presented_rotation = spec->rotation;
        s->interpolation_pending = false;
    } else {
        s->interpolation_pending = !same_vec3(s->presented_position, s->target_position) || !same_rot3(s->presented_rotation, s->target_rotation);
    }
    if (!proxy_update_player(spec)) return false;
    if (!apply_driver_presented_state(s, spec)) return false;
    s->transform_apply_count++;
    s->last_command = FSP_CMD_UPDATE;
    s->command_sequence = ++g_scene.total_command_sequence;
    g_scene.update_count++;
    return true;
}

static bool scene_despawn(void* user, PlayerId player_id) {
    Fo4SceneProxyPlayerState* s;
    (void)user;
    s = find_slot(player_id);
    if (!s || !s->in_use || s->player_id != player_id) return false;
    if (!proxy_despawn_player(player_id)) return false;
    if (fpalc_enabled()) {
        if (!fpalc_despawn(player_id)) return false;
    } else {
        if (!fpad_despawn(player_id)) return false;
    }
    sync_scene_actor_handle(s);
    s->visible = false;
    s->despawned = true;
    s->last_command = FSP_CMD_DESPAWN;
    s->command_sequence = ++g_scene.total_command_sequence;
    g_scene.despawn_count++;
    return true;
}

void fo4_scene_proxy_backend_stub_note_policy_result(PlayerId player_id, bool teleported, bool smoothed_position, bool smoothed_rotation, float source_distance) {
    Fo4SceneProxyPlayerState* s = find_slot(player_id);
    if (!s || !s->in_use || s->player_id != player_id) return;
    s->last_update_teleported = teleported;
    s->last_update_smoothed_position = smoothed_position;
    s->last_update_smoothed_rotation = smoothed_rotation;
    s->last_update_source_distance = source_distance;
    if (teleported) {
        s->presented_position = s->target_position;
        s->presented_rotation = s->target_rotation;
        s->interpolation_pending = false;
        (void)apply_driver_presented_state(s, &s->last_update);
    } else {
        s->interpolation_pending = !same_vec3(s->presented_position, s->target_position) || !same_rot3(s->presented_rotation, s->target_rotation);
    }
}

void fo4_scene_proxy_backend_stub_set_interpolation_config(const Fo4SceneProxyInterpolationConfig* cfg) {
    if (!cfg) {
        g_scene.interp.enabled = true;
        g_scene.interp.position_alpha = 0.25f;
        g_scene.interp.rotation_alpha = 0.35f;
        return;
    }
    g_scene.interp = *cfg;
    g_scene.interp.position_alpha = clamp01(g_scene.interp.position_alpha);
    g_scene.interp.rotation_alpha = clamp01(g_scene.interp.rotation_alpha);
}

Fo4SceneProxyInterpolationConfig fo4_scene_proxy_backend_stub_get_interpolation_config(void) {
    return g_scene.interp;
}

void fo4_scene_proxy_backend_stub_step(void) {
    size_t i;
    for (i = 0; i < FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS; ++i) {
        Fo4SceneProxyPlayerState* s = &g_scene.players[i];
        if (!s->in_use || !s->visible || s->despawned) continue;
        if (!g_scene.interp.enabled || s->last_update_teleported) continue;
        s->presented_position.x = lerpf(s->presented_position.x, s->target_position.x, g_scene.interp.position_alpha);
        s->presented_position.y = lerpf(s->presented_position.y, s->target_position.y, g_scene.interp.position_alpha);
        s->presented_position.z = lerpf(s->presented_position.z, s->target_position.z, g_scene.interp.position_alpha);
        s->presented_rotation.pitch = lerpf(s->presented_rotation.pitch, s->target_rotation.pitch, g_scene.interp.rotation_alpha);
        s->presented_rotation.yaw = lerpf(s->presented_rotation.yaw, s->target_rotation.yaw, g_scene.interp.rotation_alpha);
        s->presented_rotation.roll = lerpf(s->presented_rotation.roll, s->target_rotation.roll, g_scene.interp.rotation_alpha);
        s->interpolation_pending = !same_vec3(s->presented_position, s->target_position) || !same_rot3(s->presented_rotation, s->target_rotation);
        if (apply_driver_presented_state(s, &s->last_update)) {
            s->interpolation_tick_count++;
        }
    }
}

void fo4_scene_proxy_backend_stub_reset(void) {
    memset(&g_scene, 0, sizeof(g_scene));
    g_scene.interp.enabled = true;
    g_scene.interp.position_alpha = 0.25f;
    g_scene.interp.rotation_alpha = 0.35f;
}

void fo4_scene_proxy_backend_stub_install(void) {
    RemoteProxyBackend b;
    fo4_scene_proxy_backend_stub_reset();
    b.user = 0;
    b.spawn_player = scene_spawn;
    b.update_player = scene_update;
    b.despawn_player = scene_despawn;
    b.step = (void(*)(void*))fo4_scene_proxy_backend_stub_step;
    rpb_install_backend(&b);
}

size_t fo4_scene_proxy_backend_stub_spawn_count(void) { return g_scene.spawn_count; }
size_t fo4_scene_proxy_backend_stub_update_count(void) { return g_scene.update_count; }
size_t fo4_scene_proxy_backend_stub_despawn_count(void) { return g_scene.despawn_count; }
uint32_t fo4_scene_proxy_backend_stub_total_command_sequence(void) { return g_scene.total_command_sequence; }
const Fo4SceneProxyPlayerState* fo4_scene_proxy_backend_stub_get(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_SCENE_PROXY_BACKEND_MAX_PLAYERS; ++i) {
        if (g_scene.players[i].in_use && g_scene.players[i].player_id == player_id) return &g_scene.players[i];
    }
    return 0;
}
