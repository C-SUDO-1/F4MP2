#include <cassert>
#include <cstring>
extern "C" {
#include "commonlibf4_hook_adapter_stub.h"
#include "hook_bringup_policy.h"
#include "commonlibf4_hook_install_registry.h"
}

static CommonLibF4PlayerHookArgs make_player_args() {
    CommonLibF4PlayerHookArgs args{};
    args.event.player_id = 7;
    args.event.position.x = 10.0f;
    args.event.position.y = 20.0f;
    args.event.position.z = 30.0f;
    args.event.rotation.pitch = 1.0f;
    args.event.rotation.yaw = 2.0f;
    args.event.rotation.roll = 3.0f;
    args.event.velocity.x = 0.5f;
    args.event.velocity.y = 0.0f;
    args.event.velocity.z = 0.0f;
    args.event.stance = 2;
    args.event.anim_state = 9;
    args.event.equipped_form_id = 0x1234u;
    return args;
}

int main() {
    HookInstallPlan plan = hook_install_plan_success_for_mode(HOOK_BRINGUP_VANILLA_MIRROR);
    plan.actor.state = HOOK_INSTALL_FAILED;
    plan.actor.error = HOOK_INSTALL_ERR_PATCH_REJECTED;

    clf4_hook_adapter_init_mode_with_plan(HOOK_BRINGUP_VANILLA_MIRROR, &plan);
    assert(hook_install_registry_is_installed(HOOK_FAMILY_PLAYER));
    assert(!hook_install_registry_is_installed(HOOK_FAMILY_ACTOR));
    assert(hook_install_registry_get_error(HOOK_FAMILY_ACTOR) == HOOK_INSTALL_ERR_PATCH_REJECTED);
    assert(hook_bringup_vanilla_mirror_readiness() == HOOK_READINESS_PARTIAL);

    auto p = make_player_args();
    assert(clf4_submit_player_hook(&p));
    auto stats = clf4_hook_adapter_stats();
    assert(stats.player_events == 1u);

    HookInstallPlan ok = hook_install_plan_success_for_mode(HOOK_BRINGUP_BASIC_SYNC);
    clf4_hook_adapter_init_mode_with_plan(HOOK_BRINGUP_BASIC_SYNC, &ok);
    assert(hook_bringup_basic_sync_readiness() == HOOK_READINESS_READY);
    return 0;
}
