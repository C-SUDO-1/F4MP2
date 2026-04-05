#include <assert.h>
#include <string.h>

#include "player_hook_discovery.h"
#include "commonlibf4_address_provider.h"

int main(void) {
    HookDiscoveryReport report;
    F4SEInterfaceMock f4se;
    CommonLibF4AddressProvider provider;

    memset(&f4se, 0, sizeof(f4se));
    f4se.runtime_version = 0x010B0BF0u;

    provider = clf4_address_provider_make_fixed(false, false, false, false, true, true, true, true);
    assert(!phd_run_player_discovery_sequence(&f4se, &provider, &report));
    assert(report.recommended_action == HOOK_DISCOVERY_ACTION_COLLECT_REAL_ENTRY_PROBE);
    assert(report.probe_active);
    assert(report.probe_session_id != 0u);
    assert(report.selected_candidate_id == PLAYER_HOOK_CANDIDATE_INVALID);
    return 0;
}
