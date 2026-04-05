#include <assert.h>
#include <string.h>

#include "phase1r_f4se_log_probe.h"

static const char* kRuntimeLog =
    "F4SE runtime: initialize (version = 0.7.7 010B0BF0 01DCB5D9E10E0B01, os = 6.2 (9200))\n"
    "plugin directory = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\\n"
    "scanning plugin directory C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\\n"
    "checking plugin GFXBooster.dll\n"
    "config path = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\f4se.ini\n"
    "init complete\n"
    "RegisterPapyrusFunctions_Hook\n";

static const char* kLoaderLog =
    "f4se loader 00070070 01DCB5D9E0D9DA16 6.2 (9200)\n"
    "config path = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\f4se.ini\n"
    "launching: Fallout4.exe (C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Fallout4.exe)\n"
    "dll = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\f4se_1_11_191.dll\n"
    "DLL and EXE signature/serial match\n"
    "hook thread complete\n"
    "launching\n";

int main(void) {
    Phase1RF4SELogProbeConfig cfg;
    Phase1RF4SELogProbeState st;
    memset(&cfg, 0, sizeof(cfg));
    cfg.loader_log_text = kLoaderLog;
    cfg.runtime_log_text = kRuntimeLog;
    cfg.expected_f4se_build_text = "0.7.7";
    cfg.expected_runtime_dll_name = "f4se_1_11_191.dll";
    cfg.expected_plugin_dll_name = "fo4_coop_plugin_stub.dll";

    assert(phase1r_f4selp_evaluate(&cfg));
    st = phase1r_f4selp_state();
    assert(st.loader_log_present);
    assert(st.runtime_log_present);
    assert(st.expected_f4se_build_seen);
    assert(st.loader_runtime_dll_seen);
    assert(st.plugin_directory_scanned);
    assert(st.runtime_init_complete_seen);
    assert(st.papyrus_hook_seen);
    assert(st.plugin_scan_count == 1u);
    assert(!st.expected_plugin_seen);
    assert(st.logs_look_healthy);
    assert(st.ready_for_plugin_drop);
    assert(strcmp(st.expected_runtime_dll_name, "f4se_1_11_191.dll") == 0);
    return 0;
}
