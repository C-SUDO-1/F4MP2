#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "plugin_proxy_actor_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FO4_PROXY_ACTOR_DRIVER_MAX_PLAYERS 8

typedef enum Fo4ProxyActorDriverCommand {
    FPAD_CMD_NONE = 0,
    FPAD_CMD_CREATE = 1,
    FPAD_CMD_MOVE = 2,
    FPAD_CMD_ROTATE = 3,
    FPAD_CMD_DESPAWN = 4
} Fo4ProxyActorDriverCommand;

typedef struct Fo4ProxyActorCreateSpec {
    PlayerId player_id;
    NetId proxy_net_id;
    uint32_t proxy_base_form_id;
    uint64_t actor_handle;
    Vec3f position;
    Rot3f rotation;
    uint32_t equipped_form_id;
} Fo4ProxyActorCreateSpec;

typedef struct Fo4ProxyActorMoveSpec {
    PlayerId player_id;
    uint64_t actor_handle;
    Vec3f position;
    Vec3f velocity;
    uint8_t stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
} Fo4ProxyActorMoveSpec;

typedef struct Fo4ProxyActorRotateSpec {
    PlayerId player_id;
    uint64_t actor_handle;
    Rot3f rotation;
} Fo4ProxyActorRotateSpec;

typedef struct Fo4ProxyActorState {
    bool in_use;
    bool visible;
    bool despawned;
    PlayerId player_id;
    NetId proxy_net_id;
    uint32_t proxy_base_form_id;
    uint64_t actor_handle;
    Vec3f position;
    Rot3f rotation;
    Vec3f velocity;
    uint8_t stance;
    uint16_t anim_state;
    uint32_t equipped_form_id;
    uint32_t create_count;
    uint32_t move_count;
    uint32_t rotate_count;
    uint32_t despawn_count;
    Fo4ProxyActorDriverCommand last_command;
    uint32_t command_sequence;
} Fo4ProxyActorState;

typedef struct Fo4ProxyActorDriver {
    void* user;
    bool (*create_actor)(void* user, const Fo4ProxyActorCreateSpec* spec);
    bool (*move_actor)(void* user, const Fo4ProxyActorMoveSpec* spec);
    bool (*rotate_actor)(void* user, const Fo4ProxyActorRotateSpec* spec);
    bool (*despawn_actor)(void* user, PlayerId player_id);
} Fo4ProxyActorDriver;

void fpad_reset_default_driver(void);
void fpad_install_driver(const Fo4ProxyActorDriver* driver);
const Fo4ProxyActorDriver* fpad_get_driver(void);

bool fpad_create(const Fo4ProxyActorCreateSpec* spec);
bool fpad_move(const Fo4ProxyActorMoveSpec* spec);
bool fpad_rotate(const Fo4ProxyActorRotateSpec* spec);
bool fpad_despawn(PlayerId player_id);

size_t fpad_create_count(void);
size_t fpad_move_count(void);
size_t fpad_rotate_count(void);
size_t fpad_despawn_count(void);
uint32_t fpad_total_command_sequence(void);
const Fo4ProxyActorState* fpad_get_state(PlayerId player_id);

#ifdef __cplusplus
}
#endif
