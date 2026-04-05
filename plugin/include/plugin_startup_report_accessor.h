#pragma once

#include <stdbool.h>
#include "plugin_startup_selfcheck.h"

#ifdef __cplusplus
extern "C" {
#endif

void plugin_startup_report_reset(void);
void plugin_startup_report_store(PluginStartupReport report);
bool plugin_startup_report_has_value(void);
PluginStartupReport plugin_startup_report_get(void);

#ifdef __cplusplus
}
#endif
