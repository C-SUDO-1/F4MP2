#include "plugin_startup_selfcheck.h"
#include <assert.h>

int main(void) {
    PluginStartupReport r = plugin_startup_selfcheck(HOOK_BRINGUP_VANILLA_MIRROR);
    (void)r;
    return 0;
}
