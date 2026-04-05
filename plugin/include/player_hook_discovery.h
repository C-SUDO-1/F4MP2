#pragma once

#include <stdbool.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_entrypoint_stub.h"
#include "hook_discovery_report.h"

#ifdef __cplusplus
extern "C" {
#endif

bool phd_run_player_discovery(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    HookDiscoveryReport* out_report
);

#ifdef __cplusplus
}
#endif
