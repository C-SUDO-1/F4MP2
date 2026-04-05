#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "plugin_exports.h"
#include "hook_bringup_config.h"
#include "plugin_startup_selfcheck.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct F4SEInterfaceMock {
    uint32_t runtime_version;
    uint32_t editor_version;
    uint32_t is_editor;
} F4SEInterfaceMock;

typedef struct PluginInfoMock {
    uint32_t info_version;
    const char* name;
    uint32_t version;
} PluginInfoMock;

typedef struct CommonLibF4EntrypointState {
    bool query_called;
    bool load_called;
    bool runtime_supported;
    uint32_t last_runtime_version;
    HookBringupMode last_requested_mode;
    uint32_t last_readiness;
    unsigned int fatal_block_count;
    unsigned int degradable_block_count;
} CommonLibF4EntrypointState;

void clf4_entrypoint_init(void);
F4MP_PLUGIN_EXPORT bool clf4_plugin_query(const F4SEInterfaceMock* f4se, PluginInfoMock* info);
F4MP_PLUGIN_EXPORT bool clf4_plugin_load(const F4SEInterfaceMock* f4se);
CommonLibF4EntrypointState clf4_get_state(void);
PluginStartupReport clf4_get_last_startup_report(void);
bool clf4_plugin_load_for_mode(const F4SEInterfaceMock* f4se, HookBringupMode mode);

#ifdef __cplusplus
}
#endif
