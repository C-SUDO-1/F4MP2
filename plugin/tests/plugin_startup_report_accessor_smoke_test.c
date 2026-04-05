#include <assert.h>
#include "plugin_startup_report_accessor.h"

int main(void) {
    PluginStartupReport r;
    plugin_startup_report_reset();
    assert(!plugin_startup_report_has_value());

    r.requested_mode = HOOK_BRINGUP_BASIC_SYNC;
    r.readiness = HOOK_READINESS_PARTIAL;
    r.fatal_block_count = 0;
    r.degradable_block_count = 1;
    r.ready = false;
    plugin_startup_report_store(r);

    assert(plugin_startup_report_has_value());
    {
        PluginStartupReport got = plugin_startup_report_get();
        assert(got.requested_mode == HOOK_BRINGUP_BASIC_SYNC);
        assert(got.readiness == HOOK_READINESS_PARTIAL);
        assert(got.degradable_block_count == 1);
    }
    return 0;
}
