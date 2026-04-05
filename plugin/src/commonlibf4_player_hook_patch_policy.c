#include "commonlibf4_player_hook_patch_policy.h"

static const CommonLibF4PlayerHookPatchPolicy k_default_armed = {
    "armed_install",
    CLF4_PHPP_MODE_ARMED_INSTALL,
    true
};

static const CommonLibF4PlayerHookPatchPolicy k_default_dry_run = {
    "dry_run_resolve_only",
    CLF4_PHPP_MODE_DRY_RUN_RESOLVE_ONLY,
    false
};

static const CommonLibF4PlayerHookPatchPolicy* effective_policy(const CommonLibF4PlayerHookPatchPolicy* policy) {
    return policy ? policy : &k_default_armed;
}

const CommonLibF4PlayerHookPatchPolicy* clf4_phpp_default_armed(void) {
    return &k_default_armed;
}

const CommonLibF4PlayerHookPatchPolicy* clf4_phpp_default_dry_run(void) {
    return &k_default_dry_run;
}

bool clf4_phpp_is_armed(const CommonLibF4PlayerHookPatchPolicy* policy) {
    return effective_policy(policy)->mode == CLF4_PHPP_MODE_ARMED_INSTALL;
}

bool clf4_phpp_is_dry_run(const CommonLibF4PlayerHookPatchPolicy* policy) {
    return effective_policy(policy)->mode == CLF4_PHPP_MODE_DRY_RUN_RESOLVE_ONLY;
}

bool clf4_phpp_patch_step_allowed(const CommonLibF4PlayerHookPatchPolicy* policy, bool caller_patch_allowed) {
    const CommonLibF4PlayerHookPatchPolicy* effective = effective_policy(policy);
    if (effective->mode == CLF4_PHPP_MODE_DRY_RUN_RESOLVE_ONLY) {
        return false;
    }
    return effective->patch_step_allowed && caller_patch_allowed;
}
