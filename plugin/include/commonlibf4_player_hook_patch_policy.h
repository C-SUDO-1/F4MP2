#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4PlayerHookPatchPolicyMode {
    CLF4_PHPP_MODE_DRY_RUN_RESOLVE_ONLY = 0,
    CLF4_PHPP_MODE_ARMED_INSTALL = 1
} CommonLibF4PlayerHookPatchPolicyMode;

typedef struct CommonLibF4PlayerHookPatchPolicy {
    const char* name;
    CommonLibF4PlayerHookPatchPolicyMode mode;
    bool patch_step_allowed;
} CommonLibF4PlayerHookPatchPolicy;

const CommonLibF4PlayerHookPatchPolicy* clf4_phpp_default_armed(void);
const CommonLibF4PlayerHookPatchPolicy* clf4_phpp_default_dry_run(void);
bool clf4_phpp_is_armed(const CommonLibF4PlayerHookPatchPolicy* policy);
bool clf4_phpp_is_dry_run(const CommonLibF4PlayerHookPatchPolicy* policy);
bool clf4_phpp_patch_step_allowed(const CommonLibF4PlayerHookPatchPolicy* policy, bool caller_patch_allowed);

#ifdef __cplusplus
}
#endif
