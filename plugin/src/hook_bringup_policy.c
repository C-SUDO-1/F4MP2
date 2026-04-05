#include "hook_bringup_policy.h"

static bool family_installed(HookFamily family) {
    return hook_install_registry_get_state(family) == HOOK_INSTALL_INSTALLED;
}

static bool family_attempted_but_not_ready(HookFamily family) {
    HookInstallState s = hook_install_registry_get_state(family);
    return s == HOOK_INSTALL_FAILED || s == HOOK_INSTALL_PARTIAL;
}

static HookReadiness classify_readiness(bool a, bool b, bool c, bool d,
    bool aa, bool bb, bool cc, bool dd,
    bool use_c, bool use_d) {
    const bool all_ready = a && b && (!use_c || c) && (!use_d || d);
    const bool any_signal = a || b || c || d || aa || bb || cc || dd;
    if (all_ready) return HOOK_READINESS_READY;
    if (any_signal) return HOOK_READINESS_PARTIAL;
    return HOOK_READINESS_UNAVAILABLE;
}

void hook_bringup_status(HookBringupStatus* out_status) {
    if (!out_status) return;
    out_status->player_ready = family_installed(HOOK_FAMILY_PLAYER);
    out_status->actor_ready = family_installed(HOOK_FAMILY_ACTOR);
    out_status->workshop_ready = family_installed(HOOK_FAMILY_WORKSHOP);
    out_status->dialogue_quest_ready = family_installed(HOOK_FAMILY_DIALOGUE_QUEST);
    out_status->installed_count = hook_install_registry_installed_count();
    out_status->attempted_count = hook_install_registry_attempted_count();
    out_status->failed_count = hook_install_registry_failed_count();
    out_status->partial_count = hook_install_registry_partial_count();
    out_status->required_count = 2;
    out_status->core_runtime_ready = out_status->player_ready && out_status->workshop_ready;

    out_status->basic_sync_readiness = classify_readiness(
        out_status->player_ready,
        out_status->workshop_ready,
        false,
        false,
        family_attempted_but_not_ready(HOOK_FAMILY_PLAYER),
        family_attempted_but_not_ready(HOOK_FAMILY_WORKSHOP),
        false,
        false,
        false,
        false
    );

    out_status->vanilla_mirror_readiness = classify_readiness(
        out_status->player_ready,
        out_status->actor_ready,
        out_status->workshop_ready,
        out_status->dialogue_quest_ready,
        family_attempted_but_not_ready(HOOK_FAMILY_PLAYER),
        family_attempted_but_not_ready(HOOK_FAMILY_ACTOR),
        family_attempted_but_not_ready(HOOK_FAMILY_WORKSHOP),
        family_attempted_but_not_ready(HOOK_FAMILY_DIALOGUE_QUEST),
        true,
        true
    );
}

bool hook_bringup_ready_for_basic_sync(void) {
    return hook_bringup_basic_sync_readiness() == HOOK_READINESS_READY;
}

bool hook_bringup_ready_for_vanilla_mirror(void) {
    return hook_bringup_vanilla_mirror_readiness() == HOOK_READINESS_READY;
}

HookReadiness hook_bringup_basic_sync_readiness(void) {
    HookBringupStatus s;
    hook_bringup_status(&s);
    return s.basic_sync_readiness;
}

HookReadiness hook_bringup_vanilla_mirror_readiness(void) {
    HookBringupStatus s;
    hook_bringup_status(&s);
    return s.vanilla_mirror_readiness;
}
