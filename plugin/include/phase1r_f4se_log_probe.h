#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Phase1RF4SELogProbeConfig {
    const char* loader_log_text;
    const char* runtime_log_text;
    const char* expected_f4se_build_text;
    const char* expected_runtime_dll_name;
    const char* expected_plugin_dll_name;
} Phase1RF4SELogProbeConfig;

typedef struct Phase1RF4SELogProbeState {
    bool evaluated;
    bool loader_log_present;
    bool runtime_log_present;
    bool expected_f4se_build_seen;
    bool loader_runtime_dll_seen;
    bool loader_launches_game;
    bool loader_signature_match_seen;
    bool runtime_initialize_seen;
    bool runtime_init_complete_seen;
    bool plugin_directory_scanned;
    bool config_path_seen;
    bool papyrus_hook_seen;
    bool expected_plugin_seen;
    unsigned plugin_scan_count;
    bool logs_look_healthy;
    bool ready_for_plugin_drop;
    const char* expected_runtime_dll_name;
    const char* expected_plugin_dll_name;
    const char* expected_f4se_build_text;
} Phase1RF4SELogProbeState;

void phase1r_f4selp_reset(void);
bool phase1r_f4selp_evaluate(const Phase1RF4SELogProbeConfig* config);
Phase1RF4SELogProbeState phase1r_f4selp_state(void);

#ifdef __cplusplus
}
#endif
