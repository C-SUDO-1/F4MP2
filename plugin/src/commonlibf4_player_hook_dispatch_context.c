#include "commonlibf4_player_hook_dispatch_context.h"

static CommonLibF4PlayerHookDispatchKind g_kind = CLF4_PHDC_DISPATCH_NONE;

void clf4_phdc_reset(void) {
    g_kind = CLF4_PHDC_DISPATCH_NONE;
}

void clf4_phdc_set(CommonLibF4PlayerHookDispatchKind kind) {
    g_kind = kind;
}

CommonLibF4PlayerHookDispatchKind clf4_phdc_get(void) {
    return g_kind;
}

const char* clf4_phdc_label(CommonLibF4PlayerHookDispatchKind kind) {
    switch (kind) {
        case CLF4_PHDC_DISPATCH_SYNTHETIC_VALIDATION:
            return "synthetic_validation";
        case CLF4_PHDC_DISPATCH_REAL_HOOK:
            return "real_hook";
        case CLF4_PHDC_DISPATCH_NONE:
        default:
            return "none";
    }
}
