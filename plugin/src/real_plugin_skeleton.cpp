#include "real_plugin_skeleton.h"
#include <array>

namespace {
constexpr RealPluginSkeletonPlan kPlan{
    true, true, true, true, true, true
};

constexpr std::array<RealPluginHookShimInfo, 4> kShims{{
    {"player_hook_shim", "phs_ingest_player_snapshot", "player/update loop"},
    {"actor_hook_shim", "ahs_ingest_actor_snapshot", "actor lifecycle/combat"},
    {"workshop_hook_shim", "whs_ingest_workshop_event", "workshop place/move/scrap"},
    {"dialogue_quest_hook_shim", "dqhs_ingest_dialogue_event", "dialogue menus / quest observer"}
}};
}

extern "C" {

void rps_init(void) {
    // Stub only; real DLL export layer will attach to CommonLibF4/F4SE entrypoints.
}

RealPluginSkeletonPlan rps_get_plan(void) {
    return kPlan;
}

size_t rps_get_hook_shim_count(void) {
    return kShims.size();
}

bool rps_get_hook_shim(size_t index, RealPluginHookShimInfo* out_info) {
    if (!out_info || index >= kShims.size()) return false;
    *out_info = kShims[index];
    return true;
}

}
