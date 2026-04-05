#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "plugin_live_transport_config.h"

int main(void) {
    F4mpLiveTransportConfig cfg;
    char path[L_tmpnam + 32];
    FILE* f;
    bool ok;

    fltcfg_set_defaults(&cfg);
    assert(cfg.dev_enabled == false);
    assert(strcmp(cfg.host_ipv4, "127.0.0.1") == 0);
    assert(cfg.port == 7777u);
    assert(strcmp(cfg.player_guid, "dev-guid-local") == 0);
    assert(strcmp(cfg.player_name, "DevPlayer") == 0);
    assert(cfg.build_hash == 0x12345678u);
    assert(cfg.bridge.heartbeat_interval_ticks == 10u);
    assert(cfg.bridge.auto_step_remote_backend == true);
    assert(cfg.bridge.owns_remote_backend == true);
    assert(cfg.bridge.remote_backend_step_count == 2u);
    assert(cfg.bridge.apply_scene_backend_interp_config == false);
    assert(cfg.bridge.scene_interp_config.enabled == true);

    tmpnam(path);
    f = fopen(path, "wb");
    assert(f != NULL);
    fputs("[live_transport]\n", f);
    fputs("enabled = true\n", f);
    fputs("host = 10.20.30.40\n", f);
    fputs("port = 8123\n", f);
    fputs("player_guid = smoke-guid\n", f);
    fputs("player_name = SmokePlayer\n", f);
    fputs("build_hash = 0xBEEF\n", f);
    fputs("heartbeat_interval_ticks = 33\n", f);
    fputs("auto_step_remote_backend = false\n", f);
    fputs("owns_remote_backend = false\n", f);
    fputs("remote_backend_step_count = 7\n", f);
    fputs("apply_scene_backend_interp_config = true\n", f);
    fputs("scene_interp_enabled = false\n", f);
    fputs("scene_interp_position_alpha = 0.75\n", f);
    fputs("scene_interp_rotation_alpha = 0.25\n", f);
    fclose(f);

    ok = fltcfg_load_file(path, &cfg);
    remove(path);
    assert(ok);
    assert(cfg.dev_enabled == true);
    assert(strcmp(cfg.host_ipv4, "10.20.30.40") == 0);
    assert(cfg.port == 8123u);
    assert(strcmp(cfg.player_guid, "smoke-guid") == 0);
    assert(strcmp(cfg.player_name, "SmokePlayer") == 0);
    assert(cfg.build_hash == 0xBEEFu);
    assert(cfg.bridge.heartbeat_interval_ticks == 33u);
    assert(cfg.bridge.auto_step_remote_backend == false);
    assert(cfg.bridge.owns_remote_backend == false);
    assert(cfg.bridge.remote_backend_step_count == 7u);
    assert(cfg.bridge.apply_scene_backend_interp_config == true);
    assert(cfg.bridge.scene_interp_config.enabled == false);
    assert(cfg.bridge.scene_interp_config.position_alpha > 0.74f && cfg.bridge.scene_interp_config.position_alpha < 0.76f);
    assert(cfg.bridge.scene_interp_config.rotation_alpha > 0.24f && cfg.bridge.scene_interp_config.rotation_alpha < 0.26f);

    puts("plugin_live_transport_config_smoke_test: ok");
    return 0;
}
