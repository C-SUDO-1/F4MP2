#include <cassert>
#include <cstdio>
#include "commonlibf4_entrypoint_stub.h"
#include "hook_bringup_policy.h"

int main() {
    clf4_entrypoint_init();

    F4SEInterfaceMock bad{0x01000000u, 0, 0};
    PluginInfoMock info{};
    assert(!clf4_plugin_query(&bad, &info));
    auto st = clf4_get_state();
    assert(st.query_called);
    assert(!st.runtime_supported);

    clf4_entrypoint_init();
    F4SEInterfaceMock good{0x010A03D8u, 0, 0};
    assert(clf4_plugin_query(&good, &info));
    assert(clf4_plugin_load_for_mode(&good, HOOK_BRINGUP_BASIC_SYNC));
    st = clf4_get_state();
    assert(st.query_called);
    assert(st.load_called);
    assert(st.runtime_supported);
    assert(st.last_requested_mode == HOOK_BRINGUP_BASIC_SYNC);
    assert(st.last_readiness == HOOK_READINESS_READY);
    assert(st.fatal_block_count == 0);

    std::puts("commonlibf4_entrypoint_smoke_test: ok");
    return 0;
}
