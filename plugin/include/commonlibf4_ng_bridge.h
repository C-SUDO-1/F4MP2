#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CLF4BridgeIngressKind {
    CLF4_INGRESS_NONE = 0,
    CLF4_INGRESS_PLAYER_STATE = 1,
    CLF4_INGRESS_ACTOR_STATE = 2,
    CLF4_INGRESS_WORKSHOP_EVENT = 3,
    CLF4_INGRESS_DIALOGUE_EVENT = 4,
    CLF4_INGRESS_QUEST_EVENT = 5,
    CLF4_INGRESS_PAPYRUS_CALL = 6
} CLF4BridgeIngressKind;

typedef struct CLF4BridgeHookPlan {
    bool uses_commonlibf4_ng;
    bool requires_f4se_runtime;
    bool supports_papyrus_bridge;
    bool supports_scaleform_bridge;
    bool supports_message_listener;
} CLF4BridgeHookPlan;

typedef struct CLF4BridgeIngressRoute {
    CLF4BridgeIngressKind kind;
    const char* hook_source;
    const char* ingress_function;
    const char* notes;
} CLF4BridgeIngressRoute;

void clf4_bridge_init(void);
CLF4BridgeHookPlan clf4_bridge_get_plan(void);
size_t clf4_bridge_get_route_count(void);
bool clf4_bridge_get_route(size_t index, CLF4BridgeIngressRoute* out_route);

#ifdef __cplusplus
}
#endif
