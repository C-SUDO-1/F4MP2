#include "fo4_proxy_actor_driver.h"
#include <string.h>

static struct {
    Fo4ProxyActorState players[FO4_PROXY_ACTOR_DRIVER_MAX_PLAYERS];
    size_t create_count;
    size_t move_count;
    size_t rotate_count;
    size_t despawn_count;
    uint32_t total_command_sequence;
    uint64_t next_handle;
} g_fpad;

static Fo4ProxyActorDriver g_driver;

static Fo4ProxyActorState* find_slot(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_PROXY_ACTOR_DRIVER_MAX_PLAYERS; ++i) {
        if (g_fpad.players[i].in_use && g_fpad.players[i].player_id == player_id) return &g_fpad.players[i];
    }
    for (i = 0; i < FO4_PROXY_ACTOR_DRIVER_MAX_PLAYERS; ++i) {
        if (!g_fpad.players[i].in_use) return &g_fpad.players[i];
    }
    return 0;
}

static bool default_create_actor(void* user, const Fo4ProxyActorCreateSpec* spec) {
    Fo4ProxyActorState* s;
    (void)user;
    if (!spec || spec->player_id == 0) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    memset(s, 0, sizeof(*s));
    s->in_use = true;
    s->visible = true;
    s->player_id = spec->player_id;
    s->proxy_net_id = spec->proxy_net_id;
    s->proxy_base_form_id = spec->proxy_base_form_id;
    s->actor_handle = spec->actor_handle ? spec->actor_handle : ++g_fpad.next_handle;
    s->position = spec->position;
    s->rotation = spec->rotation;
    s->equipped_form_id = spec->equipped_form_id;
    s->create_count++;
    s->last_command = FPAD_CMD_CREATE;
    s->command_sequence = ++g_fpad.total_command_sequence;
    g_fpad.create_count++;
    return true;
}

static bool default_move_actor(void* user, const Fo4ProxyActorMoveSpec* spec) {
    Fo4ProxyActorState* s;
    (void)user;
    if (!spec || spec->player_id == 0) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    if (!s->in_use) {
        memset(s, 0, sizeof(*s));
        s->in_use = true;
        s->visible = true;
        s->player_id = spec->player_id;
        s->actor_handle = spec->actor_handle ? spec->actor_handle : ++g_fpad.next_handle;
    }
    s->position = spec->position;
    s->velocity = spec->velocity;
    s->stance = spec->stance;
    s->anim_state = spec->anim_state;
    s->equipped_form_id = spec->equipped_form_id;
    if (spec->actor_handle) s->actor_handle = spec->actor_handle;
    s->despawned = false;
    s->visible = true;
    s->move_count++;
    s->last_command = FPAD_CMD_MOVE;
    s->command_sequence = ++g_fpad.total_command_sequence;
    g_fpad.move_count++;
    return true;
}

static bool default_rotate_actor(void* user, const Fo4ProxyActorRotateSpec* spec) {
    Fo4ProxyActorState* s;
    (void)user;
    if (!spec || spec->player_id == 0) return false;
    s = find_slot(spec->player_id);
    if (!s) return false;
    if (!s->in_use) {
        memset(s, 0, sizeof(*s));
        s->in_use = true;
        s->visible = true;
        s->player_id = spec->player_id;
        s->actor_handle = spec->actor_handle ? spec->actor_handle : ++g_fpad.next_handle;
    }
    s->rotation = spec->rotation;
    if (spec->actor_handle) s->actor_handle = spec->actor_handle;
    s->despawned = false;
    s->visible = true;
    s->rotate_count++;
    s->last_command = FPAD_CMD_ROTATE;
    s->command_sequence = ++g_fpad.total_command_sequence;
    g_fpad.rotate_count++;
    return true;
}

static bool default_despawn_actor(void* user, PlayerId player_id) {
    Fo4ProxyActorState* s;
    (void)user;
    if (player_id == 0) return false;
    s = find_slot(player_id);
    if (!s || !s->in_use || s->player_id != player_id) return false;
    s->visible = false;
    s->despawned = true;
    s->despawn_count++;
    s->last_command = FPAD_CMD_DESPAWN;
    s->command_sequence = ++g_fpad.total_command_sequence;
    g_fpad.despawn_count++;
    return true;
}

void fpad_reset_default_driver(void) {
    memset(&g_fpad, 0, sizeof(g_fpad));
    memset(&g_driver, 0, sizeof(g_driver));
    g_driver.create_actor = default_create_actor;
    g_driver.move_actor = default_move_actor;
    g_driver.rotate_actor = default_rotate_actor;
    g_driver.despawn_actor = default_despawn_actor;
}

void fpad_install_driver(const Fo4ProxyActorDriver* driver) {
    if (!driver) {
        fpad_reset_default_driver();
        return;
    }
    g_driver = *driver;
}

const Fo4ProxyActorDriver* fpad_get_driver(void) {
    if (!g_driver.create_actor || !g_driver.move_actor || !g_driver.rotate_actor || !g_driver.despawn_actor) {
        fpad_reset_default_driver();
    }
    return &g_driver;
}

bool fpad_create(const Fo4ProxyActorCreateSpec* spec) {
    const Fo4ProxyActorDriver* d = fpad_get_driver();
    return d && d->create_actor ? d->create_actor(d->user, spec) : false;
}

bool fpad_move(const Fo4ProxyActorMoveSpec* spec) {
    const Fo4ProxyActorDriver* d = fpad_get_driver();
    return d && d->move_actor ? d->move_actor(d->user, spec) : false;
}

bool fpad_rotate(const Fo4ProxyActorRotateSpec* spec) {
    const Fo4ProxyActorDriver* d = fpad_get_driver();
    return d && d->rotate_actor ? d->rotate_actor(d->user, spec) : false;
}

bool fpad_despawn(PlayerId player_id) {
    const Fo4ProxyActorDriver* d = fpad_get_driver();
    return d && d->despawn_actor ? d->despawn_actor(d->user, player_id) : false;
}

size_t fpad_create_count(void) { return g_fpad.create_count; }
size_t fpad_move_count(void) { return g_fpad.move_count; }
size_t fpad_rotate_count(void) { return g_fpad.rotate_count; }
size_t fpad_despawn_count(void) { return g_fpad.despawn_count; }
uint32_t fpad_total_command_sequence(void) { return g_fpad.total_command_sequence; }

const Fo4ProxyActorState* fpad_get_state(PlayerId player_id) {
    size_t i;
    for (i = 0; i < FO4_PROXY_ACTOR_DRIVER_MAX_PLAYERS; ++i) {
        if (g_fpad.players[i].in_use && g_fpad.players[i].player_id == player_id) return &g_fpad.players[i];
    }
    return 0;
}
