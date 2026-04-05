#pragma once

#include <stdint.h>
#include "protocol_types.h"

#pragma pack(push, 1)
typedef struct MsgBuildRequest {
    RequestId request_id;
    PlayerId  player_id;
    uint16_t  reserved0;
    uint32_t  settlement_id;
    uint32_t  form_id;
    Vec3f     position;
    Rot3f     rotation;
    ObjectNetId snap_target_net_id;
} MsgBuildRequest;

typedef struct MsgBuildResult {
    RequestId   request_id;
    uint8_t     accepted;
    uint8_t     reason;
    uint16_t    reserved0;
    ObjectNetId object_net_id;
} MsgBuildResult;

typedef struct MsgObjectSpawn {
    ObjectNetId object_net_id;
    uint32_t    form_id;
    Vec3f       position;
    Rot3f       rotation;
    uint16_t    state_flags;
    uint16_t    reserved0;
} MsgObjectSpawn;

typedef struct MsgObjectMoveRequest {
    RequestId   request_id;
    PlayerId    player_id;
    uint16_t    reserved0;
    ObjectNetId object_net_id;
    Vec3f       position;
    Rot3f       rotation;
} MsgObjectMoveRequest;

typedef struct MsgObjectScrapRequest {
    RequestId   request_id;
    PlayerId    player_id;
    uint16_t    reserved0;
    ObjectNetId object_net_id;
} MsgObjectScrapRequest;

typedef struct MsgObjectUpdate {
    ObjectNetId object_net_id;
    uint32_t    fields_mask;
    Vec3f       position;
    Rot3f       rotation;
    uint16_t    state_flags;
    uint16_t    reserved0;
} MsgObjectUpdate;

typedef struct MsgObjectDespawn {
    ObjectNetId object_net_id;
    uint8_t     reason;
    uint8_t     reserved0[3];
} MsgObjectDespawn;
#pragma pack(pop)

enum BuildResultReason {
    BUILD_OK=0, BUILD_DENIED_GENERIC=1, BUILD_OUT_OF_BOUNDS=2, BUILD_INVALID_FORM=3,
    BUILD_INVALID_SNAP=4, BUILD_BUDGET_EXCEEDED=5, BUILD_COLLISION_REJECT=6
};

enum ObjectDespawnReason { ODESPAWN_SCRAPPED=0, ODESPAWN_OUT_OF_SCOPE=1, ODESPAWN_HOST_REMOVED=2, ODESPAWN_DESTROYED=3 };
enum ObjectUpdateFields { OUF_POSITION=1u<<0, OUF_ROTATION=1u<<1, OUF_STATE_FLAGS=1u<<2 };

enum ObjectStateFlags {
    OBJF_NONE=0, OBJF_PLACED_BY_PLAYER=1<<0, OBJF_MOVED=1<<1, OBJF_DAMAGED=1<<2, OBJF_DESTROYED=1<<3,
    OBJF_SNAP_CHILD=1<<4, OBJF_SNAP_PARENT=1<<5, OBJF_HOST_ONLY_STATE=1<<6, OBJF_RESERVED=1<<7
};

