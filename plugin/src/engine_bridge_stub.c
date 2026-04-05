#include "engine_bridge_stub.h"
#include "plugin_live_transport_dev.h"
#include "remote_proxy_manager.h"

void ebstub_init(void) {
    proxy_runtime_init();
    rpm_init(0);
    (void)fltdev_init();
}

void ebstub_shutdown(void) {
    fltdev_shutdown();
}

bool ebstub_live_tick(bool* out_sent, bool* out_applied) {
    return fltdev_tick(out_sent, out_applied);
}

bool ebstub_live_enabled(void) {
    return fltdev_enabled();
}

bool ebstub_live_connected(void) {
    return fltdev_connected();
}

const char* ebstub_live_config_path(void) {
    return fltdev_config_path();
}

size_t ebstub_player_count(void) { return proxy_runtime_player_count(); }
size_t ebstub_actor_count(void) { return proxy_runtime_actor_count(); }
size_t ebstub_object_count(void) { return proxy_runtime_object_count(); }

bool ebstub_get_player_snapshot(size_t index, EngineBridgePlayerSnapshot* out) {
    size_t i, seen = 0;
    if (!out) return false;
    for (i = 0; i < PLUGIN_MAX_PROXY_PLAYERS; ++i) {
        const ProxyPlayerRecord* p = proxy_runtime_get_player((PlayerId)(i + 1));
        if (!p) continue;
        if (seen == index) {
            out->player_id = p->player_id;
            out->position = p->position;
            out->rotation = p->rotation;
            out->velocity = p->velocity;
            out->stance = p->stance;
            out->anim_state = p->anim_state;
            out->equipped_form_id = p->equipped_form_id;
            return true;
        }
        ++seen;
    }
    return false;
}

bool ebstub_get_actor_snapshot(size_t index, EngineBridgeActorSnapshot* out) {
    size_t i, seen = 0;
    if (!out) return false;
    for (i = 0; i < PLUGIN_MAX_PROXY_ACTORS; ++i) {
        const ProxyActorRecord* a = proxy_runtime_get_actor((ActorNetId)(i + 1));
        if (!a) continue;
        if (seen == index) {
            out->actor_net_id = a->actor_net_id;
            out->base_form_id = a->base_form_id;
            out->position = a->position;
            out->rotation = a->rotation;
            out->health_norm = a->health_norm;
            out->actor_flags = a->actor_flags;
            out->is_dead = a->is_dead;
            return true;
        }
        ++seen;
    }
    return false;
}

bool ebstub_get_object_snapshot(size_t index, EngineBridgeObjectSnapshot* out) {
    size_t i, seen = 0;
    if (!out) return false;
    for (i = 0; i < PLUGIN_MAX_PROXY_OBJECTS; ++i) {
        const ProxyObjectRecord* o = proxy_runtime_get_object((ObjectNetId)(i + 1));
        if (!o) continue;
        if (seen == index) {
            out->object_net_id = o->object_net_id;
            out->form_id = o->form_id;
            out->position = o->position;
            out->rotation = o->rotation;
            out->state_flags = o->state_flags;
            return true;
        }
        ++seen;
    }
    return false;
}

bool ebstub_has_active_dialogue(void) { return proxy_runtime_get_dialogue()->active; }
const ProxyDialogueState* ebstub_get_dialogue(void) { return proxy_runtime_get_dialogue(); }
const ProxyQuestState* ebstub_get_quest(void) { return proxy_runtime_get_quest(); }
const ProxyProfileState* ebstub_get_profile(void) { return proxy_runtime_get_profile(); }
const ProxyCombatState* ebstub_get_combat(void) { return proxy_runtime_get_combat(); }
