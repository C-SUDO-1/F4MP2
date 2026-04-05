#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_hook_adapter_stub.h"
#include "plugin_startup_selfcheck.h"
#include "plugin_startup_report_accessor.h"
#include <cstring>

namespace {
CommonLibF4EntrypointState g_state{};
constexpr uint32_t kMinSupportedRuntime = 0x010A03D8u; /* symbolic Next-Gen-ish floor */
}

extern "C" {

void clf4_entrypoint_init(void) {
    std::memset(&g_state, 0, sizeof(g_state));
    plugin_startup_report_reset();
}

bool clf4_plugin_query(const F4SEInterfaceMock* f4se, PluginInfoMock* info) {
    g_state.query_called = true;
    g_state.last_runtime_version = f4se ? f4se->runtime_version : 0;
    g_state.runtime_supported = (f4se && !f4se->is_editor && f4se->runtime_version >= kMinSupportedRuntime);

    if (info) {
        info->info_version = 1;
        info->name = "fo4_coop_plugin_stub";
        info->version = 1;
    }

    return g_state.runtime_supported;
}

bool clf4_plugin_load_for_mode(const F4SEInterfaceMock* f4se, HookBringupMode mode) {
    PluginStartupReport report;
    g_state.load_called = true;
    g_state.last_requested_mode = mode;
    if (!g_state.query_called) {
        g_state.runtime_supported = false;
        g_state.last_readiness = HOOK_READINESS_UNAVAILABLE;
        return false;
    }
    if (!f4se || f4se->is_editor || f4se->runtime_version < kMinSupportedRuntime) {
        g_state.runtime_supported = false;
        g_state.last_readiness = HOOK_READINESS_UNAVAILABLE;
        return false;
    }

    clf4_hook_adapter_init_mode_with_plan(mode, nullptr);
    report = plugin_startup_selfcheck(mode);
    plugin_startup_report_store(report);
    g_state.last_readiness = report.readiness;
    g_state.fatal_block_count = report.fatal_block_count;
    g_state.degradable_block_count = report.degradable_block_count;
    return report.ready;
}

bool clf4_plugin_load(const F4SEInterfaceMock* f4se) {
    return clf4_plugin_load_for_mode(f4se, HOOK_BRINGUP_VANILLA_MIRROR);
}

CommonLibF4EntrypointState clf4_get_state(void) {
    return g_state;
}

PluginStartupReport clf4_get_last_startup_report(void) {
    return plugin_startup_report_get();
}

}
