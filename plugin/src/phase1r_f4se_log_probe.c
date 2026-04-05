#include "phase1r_f4se_log_probe.h"

#include <stddef.h>
#include <string.h>

static Phase1RF4SELogProbeState g_state;

static bool text_has(const char* haystack, const char* needle) {
    return haystack && needle && *needle && strstr(haystack, needle) != NULL;
}

static unsigned count_occurrences(const char* haystack, const char* needle) {
    unsigned count = 0;
    size_t step;
    const char* p;
    if (!haystack || !needle || !*needle) return 0;
    step = strlen(needle);
    p = haystack;
    while ((p = strstr(p, needle)) != NULL) {
        ++count;
        p += step;
    }
    return count;
}

void phase1r_f4selp_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
}

bool phase1r_f4selp_evaluate(const Phase1RF4SELogProbeConfig* config) {
    phase1r_f4selp_reset();
    if (!config) return false;

    g_state.evaluated = true;
    g_state.expected_runtime_dll_name = config->expected_runtime_dll_name;
    g_state.expected_plugin_dll_name = config->expected_plugin_dll_name;
    g_state.expected_f4se_build_text = config->expected_f4se_build_text;

    g_state.loader_log_present = config->loader_log_text && *config->loader_log_text;
    g_state.runtime_log_present = config->runtime_log_text && *config->runtime_log_text;
    g_state.expected_f4se_build_seen =
        text_has(config->loader_log_text, config->expected_f4se_build_text) ||
        text_has(config->runtime_log_text, config->expected_f4se_build_text);
    g_state.loader_runtime_dll_seen = text_has(config->loader_log_text, config->expected_runtime_dll_name);
    g_state.loader_launches_game = text_has(config->loader_log_text, "launching: Fallout4.exe") &&
                                   text_has(config->loader_log_text, "hook thread complete");
    g_state.loader_signature_match_seen = text_has(config->loader_log_text, "DLL and EXE signature/serial match");
    g_state.runtime_initialize_seen = text_has(config->runtime_log_text, "F4SE runtime: initialize");
    g_state.runtime_init_complete_seen = text_has(config->runtime_log_text, "init complete");
    g_state.plugin_directory_scanned = text_has(config->runtime_log_text, "scanning plugin directory");
    g_state.config_path_seen =
        text_has(config->loader_log_text, "config path =") ||
        text_has(config->runtime_log_text, "config path =");
    g_state.papyrus_hook_seen = text_has(config->runtime_log_text, "RegisterPapyrusFunctions_Hook");
    g_state.plugin_scan_count = count_occurrences(config->runtime_log_text, "checking plugin ");
    g_state.expected_plugin_seen = text_has(config->runtime_log_text, config->expected_plugin_dll_name);

    g_state.logs_look_healthy =
        g_state.loader_log_present &&
        g_state.runtime_log_present &&
        g_state.expected_f4se_build_seen &&
        g_state.loader_runtime_dll_seen &&
        g_state.loader_launches_game &&
        g_state.loader_signature_match_seen &&
        g_state.runtime_initialize_seen &&
        g_state.runtime_init_complete_seen &&
        g_state.plugin_directory_scanned &&
        g_state.config_path_seen;

    g_state.ready_for_plugin_drop = g_state.logs_look_healthy && !g_state.expected_plugin_seen;
    return g_state.logs_look_healthy;
}

Phase1RF4SELogProbeState phase1r_f4selp_state(void) {
    return g_state;
}
