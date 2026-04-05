#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4PlayerHookDispatchKind {
    CLF4_PHDC_DISPATCH_NONE = 0,
    CLF4_PHDC_DISPATCH_SYNTHETIC_VALIDATION = 1,
    CLF4_PHDC_DISPATCH_REAL_HOOK = 2
} CommonLibF4PlayerHookDispatchKind;

void clf4_phdc_reset(void);
void clf4_phdc_set(CommonLibF4PlayerHookDispatchKind kind);
CommonLibF4PlayerHookDispatchKind clf4_phdc_get(void);
const char* clf4_phdc_label(CommonLibF4PlayerHookDispatchKind kind);

#ifdef __cplusplus
}
#endif
