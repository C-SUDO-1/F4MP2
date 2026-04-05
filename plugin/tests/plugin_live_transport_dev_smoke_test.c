#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "plugin_live_transport_dev.h"

#ifdef _WIN32
#include <stdlib.h>
static void set_cfg_env(const char* value) { _putenv_s("F4MP_LIVE_CONFIG", value ? value : ""); }
#else
#include <stdlib.h>
static void set_cfg_env(const char* value) {
    if (value && value[0]) {
        setenv("F4MP_LIVE_CONFIG", value, 1);
    } else {
        unsetenv("F4MP_LIVE_CONFIG");
    }
}
#endif

int main(void) {
    char path[L_tmpnam + 32];
    FILE* f;
    F4mpLiveTransportDevState state;
    bool sent = true;
    bool applied = true;

    tmpnam(path);
    f = fopen(path, "wb");
    assert(f != NULL);
    fputs("[live_transport]\n", f);
    fputs("enabled = false\n", f);
    fputs("host = 127.0.0.1\n", f);
    fputs("port = 7777\n", f);
    fputs("player_guid = smoke-guid\n", f);
    fputs("player_name = SmokePlayer\n", f);
    fclose(f);

    set_cfg_env(path);
    assert(fltdev_init() == true);
    assert(fltdev_enabled() == false);
    assert(fltdev_connected() == false);
    assert(fltdev_tick(&sent, &applied) == true);
    assert(sent == false);
    assert(applied == false);

    state = fltdev_get_state();
    assert(state.config_loaded == true);
    assert(state.enabled == false);
    assert(state.started == false);
    assert(strcmp(state.config.host_ipv4, "127.0.0.1") == 0);
    assert(state.config.port == 7777u);
    assert(strcmp(state.config.player_guid, "smoke-guid") == 0);
    assert(strcmp(state.config.player_name, "SmokePlayer") == 0);
    assert(strcmp(fltdev_config_path(), path) == 0);

    fltdev_shutdown();
    set_cfg_env(NULL);
    remove(path);

    puts("plugin_live_transport_dev_smoke_test: ok");
    return 0;
}
