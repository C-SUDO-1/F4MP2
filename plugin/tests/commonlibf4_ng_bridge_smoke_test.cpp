#include "commonlibf4_ng_bridge.h"

#include <cstdio>

int main() {
    clf4_bridge_init();
    const auto plan = clf4_bridge_get_plan();
    if (!plan.uses_commonlibf4_ng) return 1;
    if (!plan.requires_f4se_runtime) return 2;
    if (!plan.supports_papyrus_bridge) return 3;

    const size_t count = clf4_bridge_get_route_count();
    if (count < 5) return 4;

    CLF4BridgeIngressRoute route{};
    if (!clf4_bridge_get_route(0, &route)) return 5;
    if (route.kind != CLF4_INGRESS_PLAYER_STATE) return 6;
    if (!route.ingress_function) return 7;

    std::puts("commonlibf4_ng_bridge_smoke_test: ok");
    return 0;
}
