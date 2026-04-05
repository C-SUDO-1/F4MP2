#include "packet_codec.h"
#include "protocol_constants.h"

#define RW_VEC3_WRITE(w, v) (pw_write_f32((w), (v).x) && pw_write_f32((w), (v).y) && pw_write_f32((w), (v).z))
#define RW_VEC3_READ(r, v)  (pr_read_f32((r), &(v).x) && pr_read_f32((r), &(v).y) && pr_read_f32((r), &(v).z))
#define RW_ROT3_WRITE(w, v) (pw_write_f32((w), (v).pitch) && pw_write_f32((w), (v).yaw) && pw_write_f32((w), (v).roll))
#define RW_ROT3_READ(r, v)  (pr_read_f32((r), &(v).pitch) && pr_read_f32((r), &(v).yaw) && pr_read_f32((r), &(v).roll))

bool encode_packet_header(PacketWriter* w, const PacketHeader* h) {
    return pw_write_u32(w, h->magic) &&
           pw_write_u16(w, h->version) &&
           pw_write_u16(w, h->flags) &&
           pw_write_u32(w, h->session_id) &&
           pw_write_u32(w, h->sequence) &&
           pw_write_u32(w, h->ack) &&
           pw_write_u32(w, h->ack_bits) &&
           pw_write_u8(w, h->message_count) &&
           pw_write_bytes(w, h->reserved, 3);
}

bool decode_packet_header(PacketReader* r, PacketHeader* h) {
    return pr_read_u32(r, &h->magic) &&
           pr_read_u16(r, &h->version) &&
           pr_read_u16(r, &h->flags) &&
           pr_read_u32(r, &h->session_id) &&
           pr_read_u32(r, &h->sequence) &&
           pr_read_u32(r, &h->ack) &&
           pr_read_u32(r, &h->ack_bits) &&
           pr_read_u8(r, &h->message_count) &&
           pr_read_bytes(r, h->reserved, 3);
}

bool encode_message_header(PacketWriter* w, const MessageHeader* h) {
    return pw_write_u16(w, h->type) && pw_write_u16(w, h->flags) && pw_write_u16(w, h->length);
}

bool decode_message_header(PacketReader* r, MessageHeader* h) {
    return pr_read_u16(r, &h->type) && pr_read_u16(r, &h->flags) && pr_read_u16(r, &h->length);
}

bool encode_msg_hello(PacketWriter* w, const MsgHello* m, const char* player_guid, const char* player_name) {
    return pw_write_u32(w, m->build_hash) && pw_write_u32(w, m->capabilities) &&
           pw_write_string_u16(w, player_guid) && pw_write_string_u16(w, player_name);
}

bool decode_msg_hello(PacketReader* r, MsgHello* m, char* player_guid, size_t player_guid_cap, char* player_name, size_t player_name_cap) {
    return pr_read_u32(r, &m->build_hash) && pr_read_u32(r, &m->capabilities) &&
           pr_read_string_u16(r, player_guid, player_guid_cap) && pr_read_string_u16(r, player_name, player_name_cap);
}


bool encode_msg_welcome(PacketWriter* w, const MsgWelcome* m) {
    return pw_write_u32(w, m->session_id) && pw_write_u16(w, m->player_id) && pw_write_u16(w, m->tick_rate) && pw_write_u32(w, m->rules);
}

bool decode_msg_welcome(PacketReader* r, MsgWelcome* m) {
    return pr_read_u32(r, &m->session_id) && pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->tick_rate) && pr_read_u32(r, &m->rules);
}

bool encode_msg_ping(PacketWriter* w, const MsgPing* m) {
    return pw_write_u32(w, m->time_ms);
}

bool decode_msg_ping(PacketReader* r, MsgPing* m) {
    return pr_read_u32(r, &m->time_ms);
}

bool encode_msg_pong(PacketWriter* w, const MsgPong* m) {
    return pw_write_u32(w, m->time_ms);
}

bool decode_msg_pong(PacketReader* r, MsgPong* m) {
    return pr_read_u32(r, &m->time_ms);
}

bool encode_msg_disconnect(PacketWriter* w, const MsgDisconnect* m) {
    return pw_write_u8(w, m->reason);
}

bool decode_msg_disconnect(PacketReader* r, MsgDisconnect* m) {
    return pr_read_u8(r, &m->reason);
}

bool encode_msg_profile_snapshot(PacketWriter* w, const MsgProfileSnapshot* m) {
    return pw_write_u32(w, m->level) &&
           pw_write_u32(w, m->xp) &&
           pw_write_u32(w, m->equipped_form_id) &&
           pw_write_u32(w, m->caps) &&
           pw_write_u8(w, m->str_stat) &&
           pw_write_u8(w, m->per_stat) &&
           pw_write_u8(w, m->end_stat) &&
           pw_write_u8(w, m->cha_stat) &&
           pw_write_u8(w, m->int_stat) &&
           pw_write_u8(w, m->agi_stat) &&
           pw_write_u8(w, m->luk_stat) &&
           pw_write_u8(w, m->reserved0);
}

bool decode_msg_profile_snapshot(PacketReader* r, MsgProfileSnapshot* m) {
    return pr_read_u32(r, &m->level) &&
           pr_read_u32(r, &m->xp) &&
           pr_read_u32(r, &m->equipped_form_id) &&
           pr_read_u32(r, &m->caps) &&
           pr_read_u8(r, &m->str_stat) &&
           pr_read_u8(r, &m->per_stat) &&
           pr_read_u8(r, &m->end_stat) &&
           pr_read_u8(r, &m->cha_stat) &&
           pr_read_u8(r, &m->int_stat) &&
           pr_read_u8(r, &m->agi_stat) &&
           pr_read_u8(r, &m->luk_stat) &&
           pr_read_u8(r, &m->reserved0);
}

bool encode_msg_profile_items(PacketWriter* w, const MsgProfileItemsHeader* h, const MsgProfileItemRecord* items) {
    uint16_t i;
    if (!pw_write_u16(w, h->item_count) || !pw_write_u16(w, h->reserved0)) return false;
    for (i = 0; i < h->item_count; ++i) {
        if (!pw_write_u32(w, items[i].form_id) ||
            !pw_write_u32(w, items[i].count) ||
            !pw_write_u8(w, items[i].equipped) ||
            !pw_write_bytes(w, items[i].reserved0, 3)) return false;
    }
    return true;
}

bool decode_msg_profile_items(PacketReader* r, MsgProfileItemsHeader* h, MsgProfileItemRecord* items_out, uint16_t max_items) {
    uint16_t i;
    if (!pr_read_u16(r, &h->item_count) || !pr_read_u16(r, &h->reserved0)) return false;
    if (h->item_count > max_items) return false;
    for (i = 0; i < h->item_count; ++i) {
        if (!pr_read_u32(r, &items_out[i].form_id) ||
            !pr_read_u32(r, &items_out[i].count) ||
            !pr_read_u8(r, &items_out[i].equipped) ||
            !pr_read_bytes(r, items_out[i].reserved0, 3)) return false;
    }
    return true;
}

bool encode_msg_profile_perks(PacketWriter* w, const MsgProfilePerksHeader* h, const MsgProfilePerkRecord* perks) {
    uint16_t i;
    if (!pw_write_u16(w, h->perk_count) || !pw_write_u16(w, h->reserved0)) return false;
    for (i = 0; i < h->perk_count; ++i) {
        if (!pw_write_u32(w, perks[i].perk_form_id) ||
            !pw_write_u8(w, perks[i].rank) ||
            !pw_write_bytes(w, perks[i].reserved0, 3)) return false;
    }
    return true;
}

bool decode_msg_profile_perks(PacketReader* r, MsgProfilePerksHeader* h, MsgProfilePerkRecord* perks_out, uint16_t max_perks) {
    uint16_t i;
    if (!pr_read_u16(r, &h->perk_count) || !pr_read_u16(r, &h->reserved0)) return false;
    if (h->perk_count > max_perks) return false;
    for (i = 0; i < h->perk_count; ++i) {
        if (!pr_read_u32(r, &perks_out[i].perk_form_id) ||
            !pr_read_u8(r, &perks_out[i].rank) ||
            !pr_read_bytes(r, perks_out[i].reserved0, 3)) return false;
    }
    return true;
}

bool encode_msg_profile_appearance(PacketWriter* w, const MsgProfileAppearance* m) {
    return pw_write_u8(w, m->sex) &&
           pw_write_bytes(w, m->reserved0, 3) &&
           pw_write_u32(w, m->body_preset);
}

bool decode_msg_profile_appearance(PacketReader* r, MsgProfileAppearance* m) {
    return pr_read_u8(r, &m->sex) &&
           pr_read_bytes(r, m->reserved0, 3) &&
           pr_read_u32(r, &m->body_preset);
}

bool encode_msg_input_state(PacketWriter* w, const MsgInputState* m) {
    return pw_write_u32(w, m->client_tick) && pw_write_i8(w, m->move_x) && pw_write_i8(w, m->move_y) &&
           pw_write_i16(w, m->look_yaw) && pw_write_i16(w, m->look_pitch) && pw_write_u32(w, m->buttons) && pw_write_u32(w, m->equipped_form_id);
}

bool decode_msg_input_state(PacketReader* r, MsgInputState* m) {
    return pr_read_u32(r, &m->client_tick) && pr_read_i8(r, &m->move_x) && pr_read_i8(r, &m->move_y) &&
           pr_read_i16(r, &m->look_yaw) && pr_read_i16(r, &m->look_pitch) && pr_read_u32(r, &m->buttons) && pr_read_u32(r, &m->equipped_form_id);
}

bool encode_msg_player_state(PacketWriter* w, const MsgPlayerState* m) {
    return pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved) &&
           RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && RW_VEC3_WRITE(w, m->velocity) &&
           pw_write_u8(w, m->stance) && pw_write_bytes(w, m->reserved2, 3) &&
           pw_write_u16(w, m->anim_state) && pw_write_u16(w, m->reserved3) && pw_write_u32(w, m->equipped_form_id) &&
           pw_write_u32(w, m->state_sequence) && pw_write_u32(w, m->observed_tick);
}

bool decode_msg_player_state(PacketReader* r, MsgPlayerState* m) {
    return pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved) &&
           RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && RW_VEC3_READ(r, m->velocity) &&
           pr_read_u8(r, &m->stance) && pr_read_bytes(r, m->reserved2, 3) &&
           pr_read_u16(r, &m->anim_state) && pr_read_u16(r, &m->reserved3) && pr_read_u32(r, &m->equipped_form_id) &&
           pr_read_u32(r, &m->state_sequence) && pr_read_u32(r, &m->observed_tick);
}

bool encode_msg_player_left(PacketWriter* w, const MsgPlayerLeft* m) {
    return pw_write_u16(w, m->player_id) &&
           pw_write_u16(w, m->reserved);
}

bool decode_msg_player_left(PacketReader* r, MsgPlayerLeft* m) {
    return pr_read_u16(r, &m->player_id) &&
           pr_read_u16(r, &m->reserved);
}

bool encode_msg_interest_set(PacketWriter* w, const MsgInterestSet* m, const CellId* cells) {
    uint16_t i;
    if (!pw_write_f32(w, m->bubble_center.x) || !pw_write_f32(w, m->bubble_center.y) || !pw_write_f32(w, m->bubble_center.z) ||
        !pw_write_f32(w, m->bubble_radius) || !pw_write_u16(w, m->cell_count) || !pw_write_u16(w, m->reserved)) return false;
    for (i = 0; i < m->cell_count; ++i) if (!pw_write_u32(w, cells[i])) return false;
    return true;
}

bool decode_msg_interest_set(PacketReader* r, MsgInterestSet* m, CellId* cells_out, uint16_t max_cells) {
    uint16_t i;
    if (!pr_read_f32(r, &m->bubble_center.x) || !pr_read_f32(r, &m->bubble_center.y) || !pr_read_f32(r, &m->bubble_center.z) ||
        !pr_read_f32(r, &m->bubble_radius) || !pr_read_u16(r, &m->cell_count) || !pr_read_u16(r, &m->reserved)) return false;
    if (m->cell_count > max_cells) return false;
    for (i = 0; i < m->cell_count; ++i) if (!pr_read_u32(r, &cells_out[i])) return false;
    return true;
}

bool encode_msg_bubble_violation(PacketWriter* w, const MsgBubbleViolation* m) {
    return pw_write_u16(w, m->player_id) && pw_write_u8(w, m->mode) && pw_write_u8(w, m->reserved) && RW_VEC3_WRITE(w, m->safe_position);
}

bool decode_msg_bubble_violation(PacketReader* r, MsgBubbleViolation* m) {
    return pr_read_u16(r, &m->player_id) && pr_read_u8(r, &m->mode) && pr_read_u8(r, &m->reserved) && RW_VEC3_READ(r, m->safe_position);
}

bool encode_msg_actor_spawn(PacketWriter* w, const MsgActorSpawn* m) {
    return pw_write_u32(w, m->actor_net_id) && pw_write_u32(w, m->base_form_id) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && pw_write_u8(w, m->actor_kind) && pw_write_u8(w, m->reserved0) && pw_write_u16(w, m->flags);
}

bool decode_msg_actor_spawn(PacketReader* r, MsgActorSpawn* m) {
    return pr_read_u32(r, &m->actor_net_id) && pr_read_u32(r, &m->base_form_id) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && pr_read_u8(r, &m->actor_kind) && pr_read_u8(r, &m->reserved0) && pr_read_u16(r, &m->flags);
}

bool encode_msg_actor_state(PacketWriter* w, const MsgActorState* m) {
    return pw_write_u32(w, m->actor_net_id) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && pw_write_f32(w, m->health_norm) && pw_write_u16(w, m->anim_state) && pw_write_u16(w, m->actor_flags) && pw_write_u32(w, m->target_net_id);
}

bool decode_msg_actor_state(PacketReader* r, MsgActorState* m) {
    return pr_read_u32(r, &m->actor_net_id) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && pr_read_f32(r, &m->health_norm) && pr_read_u16(r, &m->anim_state) && pr_read_u16(r, &m->actor_flags) && pr_read_u32(r, &m->target_net_id);
}

bool encode_msg_actor_despawn(PacketWriter* w, const MsgActorDespawn* m) { return pw_write_u32(w, m->actor_net_id) && pw_write_u8(w, m->reason) && pw_write_bytes(w, m->reserved0, 3); }
bool decode_msg_actor_despawn(PacketReader* r, MsgActorDespawn* m) { return pr_read_u32(r, &m->actor_net_id) && pr_read_u8(r, &m->reason) && pr_read_bytes(r, m->reserved0, 3); }

bool encode_msg_fire_intent(PacketWriter* w, const MsgFireIntent* m) {
    return pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->weapon_form_id) && RW_VEC3_WRITE(w, m->origin) && RW_VEC3_WRITE(w, m->direction) && pw_write_u8(w, m->mode) && pw_write_bytes(w, m->reserved1, 3) && pw_write_u32(w, m->fire_flags);
}

bool decode_msg_fire_intent(PacketReader* r, MsgFireIntent* m) {
    return pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->weapon_form_id) && RW_VEC3_READ(r, m->origin) && RW_VEC3_READ(r, m->direction) && pr_read_u8(r, &m->mode) && pr_read_bytes(r, m->reserved1, 3) && pr_read_u32(r, &m->fire_flags);
}

bool encode_msg_melee_intent(PacketWriter* w, const MsgMeleeIntent* m) { return pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->weapon_form_id) && pw_write_u32(w, m->target_hint_net_id) && pw_write_u32(w, m->melee_flags); }
bool decode_msg_melee_intent(PacketReader* r, MsgMeleeIntent* m) { return pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->weapon_form_id) && pr_read_u32(r, &m->target_hint_net_id) && pr_read_u32(r, &m->melee_flags); }

bool encode_msg_damage_result(PacketWriter* w, const MsgDamageResult* m) { return pw_write_u32(w, m->attacker_id) && pw_write_u32(w, m->victim_actor_net_id) && pw_write_f32(w, m->damage_amount) && pw_write_u8(w, m->damage_type) && pw_write_u8(w, m->reserved0) && pw_write_u16(w, m->flags) && pw_write_f32(w, m->health_after); }
bool decode_msg_damage_result(PacketReader* r, MsgDamageResult* m) { return pr_read_u32(r, &m->attacker_id) && pr_read_u32(r, &m->victim_actor_net_id) && pr_read_f32(r, &m->damage_amount) && pr_read_u8(r, &m->damage_type) && pr_read_u8(r, &m->reserved0) && pr_read_u16(r, &m->flags) && pr_read_f32(r, &m->health_after); }
bool encode_msg_death_event(PacketWriter* w, const MsgDeathEvent* m) { return pw_write_u32(w, m->victim_actor_net_id) && pw_write_u32(w, m->killer_id) && pw_write_u32(w, m->ragdoll_seed); }
bool decode_msg_death_event(PacketReader* r, MsgDeathEvent* m) { return pr_read_u32(r, &m->victim_actor_net_id) && pr_read_u32(r, &m->killer_id) && pr_read_u32(r, &m->ragdoll_seed); }

bool encode_msg_build_request(PacketWriter* w, const MsgBuildRequest* m) { return pw_write_u32(w, m->request_id) && pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->settlement_id) && pw_write_u32(w, m->form_id) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && pw_write_u32(w, m->snap_target_net_id); }
bool decode_msg_build_request(PacketReader* r, MsgBuildRequest* m) { return pr_read_u32(r, &m->request_id) && pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->settlement_id) && pr_read_u32(r, &m->form_id) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && pr_read_u32(r, &m->snap_target_net_id); }

bool encode_msg_build_result(PacketWriter* w, const MsgBuildResult* m) { return pw_write_u32(w, m->request_id) && pw_write_bool(w, m->accepted != 0) && pw_write_u8(w, m->reason) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->object_net_id); }
bool decode_msg_build_result(PacketReader* r, MsgBuildResult* m) { bool b=false; return pr_read_u32(r, &m->request_id) && pr_read_bool(r, &b) && ((m->accepted = b ? 1 : 0), true) && pr_read_u8(r, &m->reason) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->object_net_id); }

bool encode_msg_object_spawn(PacketWriter* w, const MsgObjectSpawn* m) { return pw_write_u32(w, m->object_net_id) && pw_write_u32(w, m->form_id) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && pw_write_u16(w, m->state_flags) && pw_write_u16(w, m->reserved0); }
bool decode_msg_object_spawn(PacketReader* r, MsgObjectSpawn* m) { return pr_read_u32(r, &m->object_net_id) && pr_read_u32(r, &m->form_id) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && pr_read_u16(r, &m->state_flags) && pr_read_u16(r, &m->reserved0); }
bool encode_msg_object_move_request(PacketWriter* w, const MsgObjectMoveRequest* m) { return pw_write_u32(w, m->request_id) && pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->object_net_id) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation); }
bool decode_msg_object_move_request(PacketReader* r, MsgObjectMoveRequest* m) { return pr_read_u32(r, &m->request_id) && pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->object_net_id) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation); }
bool encode_msg_object_scrap_request(PacketWriter* w, const MsgObjectScrapRequest* m) { return pw_write_u32(w, m->request_id) && pw_write_u16(w, m->player_id) && pw_write_u16(w, m->reserved0) && pw_write_u32(w, m->object_net_id); }
bool decode_msg_object_scrap_request(PacketReader* r, MsgObjectScrapRequest* m) { return pr_read_u32(r, &m->request_id) && pr_read_u16(r, &m->player_id) && pr_read_u16(r, &m->reserved0) && pr_read_u32(r, &m->object_net_id); }
bool encode_msg_object_update(PacketWriter* w, const MsgObjectUpdate* m) { return pw_write_u32(w, m->object_net_id) && pw_write_u32(w, m->fields_mask) && RW_VEC3_WRITE(w, m->position) && RW_ROT3_WRITE(w, m->rotation) && pw_write_u16(w, m->state_flags) && pw_write_u16(w, m->reserved0); }
bool decode_msg_object_update(PacketReader* r, MsgObjectUpdate* m) { return pr_read_u32(r, &m->object_net_id) && pr_read_u32(r, &m->fields_mask) && RW_VEC3_READ(r, m->position) && RW_ROT3_READ(r, m->rotation) && pr_read_u16(r, &m->state_flags) && pr_read_u16(r, &m->reserved0); }
bool encode_msg_object_despawn(PacketWriter* w, const MsgObjectDespawn* m) { return pw_write_u32(w, m->object_net_id) && pw_write_u8(w, m->reason) && pw_write_bytes(w, m->reserved0, 3); }
bool decode_msg_object_despawn(PacketReader* r, MsgObjectDespawn* m) { return pr_read_u32(r, &m->object_net_id) && pr_read_u8(r, &m->reason) && pr_read_bytes(r, m->reserved0, 3); }


bool encode_msg_dialogue_open(PacketWriter* w, const MsgDialogueOpen* m) {
    return pw_write_u32(w, m->dialogue_id) && pw_write_u32(w, m->npc_actor_net_id) && pw_write_u32(w, m->speaker_actor_net_id);
}
bool decode_msg_dialogue_open(PacketReader* r, MsgDialogueOpen* m) {
    return pr_read_u32(r, &m->dialogue_id) && pr_read_u32(r, &m->npc_actor_net_id) && pr_read_u32(r, &m->speaker_actor_net_id);
}
bool encode_msg_dialogue_line(PacketWriter* w, const MsgDialogueLine* m, const char* text) {
    return pw_write_u32(w, m->dialogue_id) && pw_write_u32(w, m->speaker_actor_net_id) && pw_write_string_u16(w, text ? text : "");
}
bool decode_msg_dialogue_line(PacketReader* r, MsgDialogueLine* m, char* text_out, size_t text_cap) {
    return pr_read_u32(r, &m->dialogue_id) && pr_read_u32(r, &m->speaker_actor_net_id) && pr_read_string_u16(r, text_out, text_cap);
}
bool encode_msg_dialogue_choices(PacketWriter* w, const MsgDialogueChoicesHeader* h, const MsgDialogueChoiceRecord* choices, const char* const* texts) {
    uint16_t i;
    if (!pw_write_u32(w, h->dialogue_id) || !pw_write_u16(w, h->choice_count) || !pw_write_u16(w, h->reserved0)) return false;
    for (i = 0; i < h->choice_count; ++i) {
        if (!pw_write_u32(w, choices[i].choice_id) || !pw_write_u8(w, choices[i].enabled) || !pw_write_bytes(w, choices[i].reserved0, 3) || !pw_write_string_u16(w, texts && texts[i] ? texts[i] : "")) return false;
    }
    return true;
}
bool decode_msg_dialogue_choices(PacketReader* r, MsgDialogueChoicesHeader* h, MsgDialogueChoiceRecord* choices_out, char texts_out[][256], uint16_t max_choices) {
    uint16_t i;
    if (!pr_read_u32(r, &h->dialogue_id) || !pr_read_u16(r, &h->choice_count) || !pr_read_u16(r, &h->reserved0)) return false;
    if (h->choice_count > max_choices) return false;
    for (i = 0; i < h->choice_count; ++i) {
        if (!pr_read_u32(r, &choices_out[i].choice_id) || !pr_read_u8(r, &choices_out[i].enabled) || !pr_read_bytes(r, choices_out[i].reserved0, 3) || !pr_read_string_u16(r, texts_out[i], 256)) return false;
    }
    return true;
}
bool encode_msg_dialogue_select(PacketWriter* w, const MsgDialogueSelect* m) {
    return pw_write_u32(w, m->dialogue_id) && pw_write_u32(w, m->choice_id);
}
bool decode_msg_dialogue_select(PacketReader* r, MsgDialogueSelect* m) {
    return pr_read_u32(r, &m->dialogue_id) && pr_read_u32(r, &m->choice_id);
}
bool encode_msg_dialogue_close(PacketWriter* w, const MsgDialogueClose* m) {
    return pw_write_u32(w, m->dialogue_id) && pw_write_u8(w, m->reason) && pw_write_bytes(w, m->reserved0, 3);
}
bool decode_msg_dialogue_close(PacketReader* r, MsgDialogueClose* m) {
    return pr_read_u32(r, &m->dialogue_id) && pr_read_u8(r, &m->reason) && pr_read_bytes(r, m->reserved0, 3);
}
bool encode_msg_quest_update(PacketWriter* w, const MsgQuestUpdateHeader* h, const MsgQuestObjectiveRecord* objs, const char* const* texts) {
    uint8_t i;
    if (!pw_write_u32(w, h->quest_id) || !pw_write_u16(w, h->stage) || !pw_write_u8(w, h->tracked) || !pw_write_u8(w, h->objective_count)) return false;
    for (i = 0; i < h->objective_count; ++i) {
        if (!pw_write_u32(w, objs[i].objective_id) || !pw_write_u8(w, objs[i].state) || !pw_write_bytes(w, objs[i].reserved0, 3) || !pw_write_string_u16(w, texts && texts[i] ? texts[i] : "")) return false;
    }
    return true;
}
bool decode_msg_quest_update(PacketReader* r, MsgQuestUpdateHeader* h, MsgQuestObjectiveRecord* objs_out, char texts_out[][256], uint8_t max_objectives) {
    uint8_t i;
    if (!pr_read_u32(r, &h->quest_id) || !pr_read_u16(r, &h->stage) || !pr_read_u8(r, &h->tracked) || !pr_read_u8(r, &h->objective_count)) return false;
    if (h->objective_count > max_objectives) return false;
    for (i = 0; i < h->objective_count; ++i) {
        if (!pr_read_u32(r, &objs_out[i].objective_id) || !pr_read_u8(r, &objs_out[i].state) || !pr_read_bytes(r, objs_out[i].reserved0, 3) || !pr_read_string_u16(r, texts_out[i], 256)) return false;
    }
    return true;
}
