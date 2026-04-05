#include "commonlibf4_hook_attempt_executor.h"
#include "hook_bringup_config.h"
#include "commonlibf4_hook_install_registry.h"
#include "startup_blocking_policy.h"
#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_player_hook_attempt_stub.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "commonlibf4_actor_hook_attempt_stub.h"
#include "commonlibf4_workshop_hook_attempt_stub.h"
#include "commonlibf4_dialogue_quest_hook_attempt_stub.h"
#include "commonlibf4_address_provider.h"

static HookInstallRecord rec_for_family(const HookInstallPlan* p, HookFamily family) {
    switch (family) {
        case HOOK_FAMILY_PLAYER: return p->player;
        case HOOK_FAMILY_ACTOR: return p->actor;
        case HOOK_FAMILY_WORKSHOP: return p->workshop;
        case HOOK_FAMILY_DIALOGUE_QUEST: return p->dialogue_quest;
        default: {
            HookInstallRecord r = { HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNKNOWN };
            return r;
        }
    }
}

static bool family_enabled(HookBringupConfig cfg, HookFamily family) {
    switch (family) {
        case HOOK_FAMILY_PLAYER: return cfg.enable_player;
        case HOOK_FAMILY_ACTOR: return cfg.enable_actor;
        case HOOK_FAMILY_WORKSHOP: return cfg.enable_workshop;
        case HOOK_FAMILY_DIALOGUE_QUEST: return cfg.enable_dialogue_quest;
        default: return false;
    }
}

static HookAttemptResult runtime_attempt_for_family(const F4SEInterfaceMock* f4se, HookFamily family) {
    switch (family) {
        case HOOK_FAMILY_PLAYER: {
            CommonLibF4AddressProvider provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
            return clf4_attempt_install_player_hook_with_provider(f4se, &provider, true, clf4_phpp_default_armed());
        }
        case HOOK_FAMILY_ACTOR: {
            CommonLibF4AddressProvider provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
            return clf4_attempt_install_actor_hook_with_provider(f4se, &provider, true, true);
        }
        case HOOK_FAMILY_WORKSHOP: {
            CommonLibF4AddressProvider provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
            return clf4_attempt_install_workshop_hook_with_provider(f4se, &provider, true);
        }
        case HOOK_FAMILY_DIALOGUE_QUEST: {
            CommonLibF4AddressProvider provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
            return clf4_attempt_install_dialogue_quest_hook_with_provider(f4se, &provider, true, true);
        }
        default:
            return hook_attempt_result_make(family, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNKNOWN, HOOK_BLOCKING_DEGRADABLE, true);
    }
}

bool hook_attempt_executor_apply_mode_plan(
    HookBringupMode mode,
    const HookInstallPlan* plan,
    HookAttemptResult* out_results,
    unsigned int max_results,
    unsigned int* out_count
) {
    HookBringupConfig cfg;
    unsigned int n = 0;
    HookInstallPlan local;
    if (!out_results || max_results < 4) return false;
    cfg = hook_bringup_config_for_mode(mode);
    local = plan ? *plan : hook_install_plan_success_for_mode(mode);

    hook_install_registry_reset();
    for (int f = 0; f < HOOK_FAMILY_COUNT; ++f) {
        HookFamily family = (HookFamily)f;
        bool attempted = family_enabled(cfg, family);
        HookInstallRecord rec = attempted ? rec_for_family(&local, family) : (HookInstallRecord){ HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE };
        HookInstallBlocking blocking = startup_blocking_for_family(mode, family, rec.state, rec.error);
        hook_install_registry_mark_state(family, rec.state, rec.error);
        out_results[n++] = hook_attempt_result_make(family, rec.state, rec.error, blocking, attempted);
    }
    if (out_count) *out_count = n;
    return true;
}

bool hook_attempt_executor_apply_mode_runtime_stub(
    const F4SEInterfaceMock* f4se,
    HookBringupMode mode,
    HookAttemptResult* out_results,
    unsigned int max_results,
    unsigned int* out_count
) {
    HookBringupConfig cfg;
    unsigned int n = 0;
    if (!out_results || max_results < 4) return false;
    cfg = hook_bringup_config_for_mode(mode);
    hook_install_registry_reset();

    for (int f = 0; f < HOOK_FAMILY_COUNT; ++f) {
        HookFamily family = (HookFamily)f;
        HookAttemptResult r;
        if (!family_enabled(cfg, family)) {
            r = hook_attempt_result_make(family, HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, false);
        } else {
            r = runtime_attempt_for_family(f4se, family);
            r.blocking = startup_blocking_for_family(mode, family, r.state, r.error);
        }
        hook_install_registry_mark_state(family, r.state, r.error);
        out_results[n++] = r;
    }
    if (out_count) *out_count = n;
    return true;
}
