#include "real_plugin_skeleton.h"
#include <cassert>
#include <cstdio>

int main() {
    rps_init();
    auto plan = rps_get_plan();
    assert(plan.uses_commonlibf4_ng);
    assert(plan.exports_plugin_query);
    assert(rps_get_hook_shim_count() >= 4);
    RealPluginHookShimInfo info{};
    assert(rps_get_hook_shim(0, &info));
    assert(info.name != nullptr);
    std::puts("real_plugin_skeleton_smoke_test: ok");
    return 0;
}
