#include "commonlibf4_ng_bridge.h"

#include <array>
#include <string_view>

namespace {
constexpr CLF4BridgeHookPlan kPlan{
    true,  // uses_commonlibf4_ng
    true,  // requires_f4se_runtime
    true,  // supports_papyrus_bridge
    true,  // supports_scaleform_bridge
    true   // supports_message_listener
};

constexpr std::array<CLF4BridgeIngressRoute, 6> kRoutes{{
    {CLF4_INGRESS_PLAYER_STATE, "player/update loop", "fbi_ingest_remote_player_state", "Thin adapter from vanilla player state extraction into protocol apply."},
    {CLF4_INGRESS_ACTOR_STATE, "actor lifecycle / combat observer", "fbi_ingest_actor_state", "Host-owned NPC and combat mirror ingress."},
    {CLF4_INGRESS_WORKSHOP_EVENT, "workshop place/move/scrap hooks", "fbi_ingest_object_update", "Workshop requests stay vanilla-first and host-authoritative."},
    {CLF4_INGRESS_DIALOGUE_EVENT, "dialogue scene / menu bridge", "fbi_ingest_dialogue_open", "Mirror dialogue UI state only; do not run guest quest logic."},
    {CLF4_INGRESS_QUEST_EVENT, "quest/objective observer", "fbi_ingest_quest_update", "Compact quest mirror for tracked UI state."},
    {CLF4_INGRESS_PAPYRUS_CALL, "quest script / papyrus bridge", "service dialogue select + host event mirror", "Use Papyrus or quest-script initiated calls for safe host-controlled progression."}
}};
}

extern "C" {

void clf4_bridge_init(void) {
    // Stub only; real implementation will bind CommonLibF4/F4SE entrypoints.
}

CLF4BridgeHookPlan clf4_bridge_get_plan(void) {
    return kPlan;
}

size_t clf4_bridge_get_route_count(void) {
    return kRoutes.size();
}

bool clf4_bridge_get_route(size_t index, CLF4BridgeIngressRoute* out_route) {
    if (!out_route || index >= kRoutes.size()) {
        return false;
    }
    *out_route = kRoutes[index];
    return true;
}

}
