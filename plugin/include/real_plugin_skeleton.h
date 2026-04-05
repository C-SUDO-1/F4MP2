#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RealPluginSkeletonPlan {
    bool uses_commonlibf4_ng;
    bool requires_f4se_runtime;
    bool exports_plugin_query;
    bool exports_plugin_load;
    bool has_message_listener;
    bool has_papyrus_bindings;
} RealPluginSkeletonPlan;

typedef struct RealPluginHookShimInfo {
    const char* name;
    const char* ingress_function;
    const char* source_event;
} RealPluginHookShimInfo;

void rps_init(void);
RealPluginSkeletonPlan rps_get_plan(void);
size_t rps_get_hook_shim_count(void);
bool rps_get_hook_shim(size_t index, RealPluginHookShimInfo* out_info);

#ifdef __cplusplus
}
#endif
