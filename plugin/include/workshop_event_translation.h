#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_workshop.h"

typedef enum WorkshopHostEventKind {
    WHE_PLACE = 0,
    WHE_MOVE = 1,
    WHE_SCRAP = 2
} WorkshopHostEventKind;

typedef struct WorkshopHostEvent {
    WorkshopHostEventKind kind;
    ObjectNetId object_net_id;
    uint32_t settlement_id;
    uint32_t form_id;
    Vec3f position;
    Rot3f rotation;
    uint16_t state_flags;
} WorkshopHostEvent;

bool whe_translate_spawn(const WorkshopHostEvent* ev, MsgObjectSpawn* out_msg);
bool whe_translate_update(const WorkshopHostEvent* ev, MsgObjectUpdate* out_msg);
bool whe_translate_despawn(const WorkshopHostEvent* ev, MsgObjectDespawn* out_msg);
