#include "plugin_startup_report_accessor.h"
#include <string.h>

static PluginStartupReport g_report;
static bool g_has_report = false;

void plugin_startup_report_reset(void) {
    memset(&g_report, 0, sizeof(g_report));
    g_has_report = false;
}

void plugin_startup_report_store(PluginStartupReport report) {
    g_report = report;
    g_has_report = true;
}

bool plugin_startup_report_has_value(void) {
    return g_has_report;
}

PluginStartupReport plugin_startup_report_get(void) {
    return g_report;
}
