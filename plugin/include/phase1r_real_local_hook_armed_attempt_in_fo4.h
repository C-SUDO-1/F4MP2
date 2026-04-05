#pragma once

#include <stdbool.h>

#include "phase1r_f4se_log_probe.h"
#include "phase1r_real_local_hook_dry_run_in_fo4.h"
#include "phase1r_real_local_hook_wiring.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RRealLocalHookArmedFo4Action {
    PHASE1R_RLHA_ACTION_HOLD_SCAFFOLD = 0,
    PHASE1R_RLHA_ACTION_BUILD_PLUGIN_DLL_DROP,
    PHASE1R_RLHA_ACTION_ATTEMPT_ARMED_HOOK_NOW
} Phase1RRealLocalHookArmedFo4Action;

typedef struct Phase1RRealLocalHookArmedAttemptInFo4Config {
    Phase1RRealLocalHookDryRunInFo4Config dry_run_config;
    Phase1RRealLocalHookWiringConfig wiring_config;
    Phase1RF4SELogProbeConfig log_probe_config;
    bool require_logs_healthy;
    bool require_plugin_absent_for_first_drop;
} Phase1RRealLocalHookArmedAttemptInFo4Config;

typedef struct Phase1RRealLocalHookArmedAttemptInFo4State {
    bool attempted;
    bool dry_run_ready;
    bool wiring_ready;
    bool logs_healthy;
    bool expected_plugin_seen;
    bool ready_for_first_plugin_drop;
    bool armed_attempt_preconditions_met;
    const char* runtime_profile_name;
    const char* site_prototype_name;
    const char* expected_plugin_dll_name;
    Phase1RRealLocalHookArmedFo4Action recommended_action;
    Phase1RRealLocalHookDryRunInFo4State dry_run_state;
    Phase1RRealLocalHookWiringState wiring_state;
    Phase1RF4SELogProbeState log_probe_state;
} Phase1RRealLocalHookArmedAttemptInFo4State;

void phase1r_rlhaf_reset(void);
bool phase1r_rlhaf_attempt(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const Phase1RRealLocalHookArmedAttemptInFo4Config* config
);
Phase1RRealLocalHookArmedAttemptInFo4State phase1r_rlhaf_state(void);
const char* phase1r_rlhaf_action_name(Phase1RRealLocalHookArmedFo4Action action);

#ifdef __cplusplus
}
#endif
