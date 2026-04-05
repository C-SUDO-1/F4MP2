#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_local_player_probe_provider.h"
#include "commonlibf4_player_hook_callback_stub.h"
#include "commonlibf4_player_hook_dispatch_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookSourceCandidateAdapterConfig {
    PlayerId expected_local_player_id;
    bool require_hook_correct;
} CommonLibF4PlayerHookSourceCandidateAdapterConfig;

typedef struct CommonLibF4PlayerHookSourceCandidateAdapterStats {
    bool installed;
    bool callback_registered;
    bool provider_installed;
    uint32_t callback_forward_count;
    uint32_t callback_accept_count;
    uint32_t callback_reject_count;
    uint32_t real_callback_forward_count;
    uint32_t real_callback_accept_count;
    uint32_t real_callback_reject_count;
    uint32_t last_probe_callback_count;
    PlayerId expected_local_player_id;
    LocalPlayerObservedState last_state;
    bool has_state;
    CommonLibF4PlayerHookDispatchKind last_dispatch_kind;
} CommonLibF4PlayerHookSourceCandidateAdapterStats;

void clf4_phsca_install(const CommonLibF4PlayerHookSourceCandidateAdapterConfig* config);
void clf4_phsca_uninstall(void);
bool clf4_phsca_installed(void);
CommonLibF4PlayerHookSourceCandidateAdapterStats clf4_phsca_stats(void);

#ifdef __cplusplus
}
#endif
