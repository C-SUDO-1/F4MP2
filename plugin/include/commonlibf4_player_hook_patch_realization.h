#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4PlayerHookPatchRealizationError {
    CLF4_PATCH_REALIZATION_ERR_NONE = 0,
    CLF4_PATCH_REALIZATION_ERR_NOT_ATTEMPTED = 1,
    CLF4_PATCH_REALIZATION_ERR_TARGET_MISSING = 2,
    CLF4_PATCH_REALIZATION_ERR_DETOUR_MISSING = 3,
    CLF4_PATCH_REALIZATION_ERR_INCOMPATIBLE_DETOUR_ENTRY = 4,
    CLF4_PATCH_REALIZATION_ERR_PROTECT_FAILED = 5,
    CLF4_PATCH_REALIZATION_ERR_TRAMPOLINE_ALLOC_FAILED = 6,
    CLF4_PATCH_REALIZATION_ERR_WRITE_FAILED = 7,
    CLF4_PATCH_REALIZATION_ERR_UNSUPPORTED_PLATFORM = 8,
    CLF4_PATCH_REALIZATION_ERR_VERIFICATION_ONLY_SAFETY_ROLLBACK = 9,
    CLF4_PATCH_REALIZATION_ERR_PLANNER_UNSUPPORTED_INSTRUCTION = 10
} CommonLibF4PlayerHookPatchRealizationError;

typedef struct CommonLibF4PlayerHookPatchRealizationPlan {
    uintptr_t target_addr;
    const char* target_symbol;
    uintptr_t detour_destination_addr;
    const char* detour_destination_label;
} CommonLibF4PlayerHookPatchRealizationPlan;

typedef struct CommonLibF4PlayerHookPatchRealizationState {
    bool attempted;
    bool patch_write_realized;
    bool trampoline_realized;
    uintptr_t target_addr;
    const char* target_symbol;
    uintptr_t detour_destination_addr;
    const char* detour_destination_label;
    uintptr_t trampoline_addr;
    uint32_t bytes_overwritten;
    uint32_t platform_error_code;
    uint32_t gateway_marker_count;
    CommonLibF4PlayerHookPatchRealizationError error;
    const char* error_label;
} CommonLibF4PlayerHookPatchRealizationState;

void clf4_phpr_reset(void);
bool clf4_phpr_attempt(const CommonLibF4PlayerHookPatchRealizationPlan* plan);
CommonLibF4PlayerHookPatchRealizationState clf4_phpr_state(void);
const char* clf4_phpr_error_label(CommonLibF4PlayerHookPatchRealizationError error);

#ifdef __cplusplus
}
#endif
