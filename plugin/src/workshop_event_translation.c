#include "workshop_event_translation.h"
#include <string.h>

bool whe_translate_spawn(const WorkshopHostEvent* ev, MsgObjectSpawn* out_msg) {
    if (!ev || !out_msg || ev->kind != WHE_PLACE) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->object_net_id = ev->object_net_id;
    out_msg->form_id = ev->form_id;
    out_msg->position = ev->position;
    out_msg->rotation = ev->rotation;
    out_msg->state_flags = ev->state_flags;
    return true;
}

bool whe_translate_update(const WorkshopHostEvent* ev, MsgObjectUpdate* out_msg) {
    if (!ev || !out_msg || ev->kind != WHE_MOVE) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->object_net_id = ev->object_net_id;
    out_msg->fields_mask = OUF_POSITION | OUF_ROTATION | OUF_STATE_FLAGS;
    out_msg->position = ev->position;
    out_msg->rotation = ev->rotation;
    out_msg->state_flags = ev->state_flags;
    return true;
}

bool whe_translate_despawn(const WorkshopHostEvent* ev, MsgObjectDespawn* out_msg) {
    if (!ev || !out_msg || ev->kind != WHE_SCRAP) return false;
    memset(out_msg, 0, sizeof(*out_msg));
    out_msg->object_net_id = ev->object_net_id;
    out_msg->reason = ODESPAWN_SCRAPPED;
    return true;
}
