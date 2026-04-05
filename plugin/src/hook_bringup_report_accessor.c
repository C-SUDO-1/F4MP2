#include "hook_bringup_report_accessor.h"
#include <string.h>

static HookAttemptResult g_results[HOOK_FAMILY_COUNT];
static unsigned int g_count = 0;
static bool g_has_value = false;

void hook_bringup_report_reset(void) {
    memset(g_results, 0, sizeof(g_results));
    g_count = 0;
    g_has_value = false;
}

void hook_bringup_report_store(const HookAttemptResult* results, unsigned int count) {
    unsigned int i;
    if (!results) {
        hook_bringup_report_reset();
        return;
    }
    if (count > HOOK_FAMILY_COUNT) count = HOOK_FAMILY_COUNT;
    for (i = 0; i < count; ++i) g_results[i] = results[i];
    for (; i < HOOK_FAMILY_COUNT; ++i) memset(&g_results[i], 0, sizeof(g_results[i]));
    g_count = count;
    g_has_value = true;
}

bool hook_bringup_report_has_value(void) {
    return g_has_value;
}

unsigned int hook_bringup_report_count(void) {
    return g_has_value ? g_count : 0u;
}

bool hook_bringup_report_get_result(unsigned int index, HookAttemptResult* out_result) {
    if (!g_has_value || !out_result || index >= g_count) return false;
    *out_result = g_results[index];
    return true;
}
