#pragma once
#include <stdbool.h>
#include "plugin_proxy_actor_contract.h"
#include "proxy_runtime.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct RemoteProxyUpdatePolicy {
    bool enabled;
    float teleport_distance;
    float position_lerp_alpha;
    float rotation_lerp_alpha;
} RemoteProxyUpdatePolicy;

typedef struct RemoteProxyUpdatePolicyResult {
    bool policy_enabled;
    bool teleported;
    bool smoothed_position;
    bool smoothed_rotation;
    float source_distance;
} RemoteProxyUpdatePolicyResult;

void rpup_init_default(void);
void rpup_set(const RemoteProxyUpdatePolicy* policy);
RemoteProxyUpdatePolicy rpup_get(void);
void rpup_apply_player_update(const ProxyPlayerRecord* current, ProxyPlayerUpdateSpec* update);
void rpup_apply_player_update_ex(const ProxyPlayerRecord* current, ProxyPlayerUpdateSpec* update, RemoteProxyUpdatePolicyResult* out_result);
#ifdef __cplusplus
}
#endif
