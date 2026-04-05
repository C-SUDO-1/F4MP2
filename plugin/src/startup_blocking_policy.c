#include "startup_blocking_policy.h"
#include "commonlibf4_hook_install_registry.h"

static bool family_required_for_mode(HookBringupMode mode, HookFamily family) {
    HookBringupConfig cfg = hook_bringup_config_for_mode(mode);
    switch (family) {
        case HOOK_FAMILY_PLAYER: return cfg.enable_player;
        case HOOK_FAMILY_ACTOR: return cfg.enable_actor;
        case HOOK_FAMILY_WORKSHOP: return cfg.enable_workshop;
        case HOOK_FAMILY_DIALOGUE_QUEST: return cfg.enable_dialogue_quest;
        default: return false;
    }
}

HookInstallBlocking startup_blocking_for_family(
    HookBringupMode mode,
    HookFamily family,
    HookInstallState state,
    HookInstallError error
) {
    (void)error;
    if (!family_required_for_mode(mode, family)) return HOOK_BLOCKING_NONE;
    if (state == HOOK_INSTALL_INSTALLED) return HOOK_BLOCKING_NONE;
    if (state == HOOK_INSTALL_NOT_ATTEMPTED) return HOOK_BLOCKING_FATAL;

    switch (mode) {
        case HOOK_BRINGUP_BASIC_SYNC:
            if (family == HOOK_FAMILY_PLAYER || family == HOOK_FAMILY_WORKSHOP) {
                return HOOK_BLOCKING_FATAL;
            }
            return HOOK_BLOCKING_DEGRADABLE;
        case HOOK_BRINGUP_VANILLA_MIRROR:
            if (family == HOOK_FAMILY_PLAYER || family == HOOK_FAMILY_WORKSHOP) {
                return HOOK_BLOCKING_FATAL;
            }
            if (family == HOOK_FAMILY_ACTOR || family == HOOK_FAMILY_DIALOGUE_QUEST) {
                return HOOK_BLOCKING_DEGRADABLE;
            }
            return HOOK_BLOCKING_DEGRADABLE;
        case HOOK_BRINGUP_NONE:
        default:
            return HOOK_BLOCKING_NONE;
    }
}

StartupBlockingSummary startup_blocking_summary_for_mode(HookBringupMode mode) {
    StartupBlockingSummary s;
    HookReadiness base;
    s.fatal_count = 0;
    s.degradable_count = 0;
    s.fatal_blocked = false;
    switch (mode) {
        case HOOK_BRINGUP_BASIC_SYNC:
            base = hook_bringup_basic_sync_readiness();
            break;
        case HOOK_BRINGUP_VANILLA_MIRROR:
            base = hook_bringup_vanilla_mirror_readiness();
            break;
        case HOOK_BRINGUP_NONE:
        default:
            base = HOOK_READINESS_UNAVAILABLE;
            break;
    }

    for (int f = 0; f < HOOK_FAMILY_COUNT; ++f) {
        HookInstallBlocking b = startup_blocking_for_family(
            mode, (HookFamily)f,
            hook_install_registry_get_state((HookFamily)f),
            hook_install_registry_get_error((HookFamily)f));
        if (b == HOOK_BLOCKING_FATAL) {
            s.fatal_count++;
            s.fatal_blocked = true;
        } else if (b == HOOK_BLOCKING_DEGRADABLE) {
            s.degradable_count++;
        }
    }

    if (s.fatal_blocked) s.adjusted_readiness = HOOK_READINESS_UNAVAILABLE;
    else if (s.degradable_count > 0 && base == HOOK_READINESS_READY) s.adjusted_readiness = HOOK_READINESS_PARTIAL;
    else s.adjusted_readiness = base;
    return s;
}
