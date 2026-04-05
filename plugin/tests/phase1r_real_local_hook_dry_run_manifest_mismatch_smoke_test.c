#include <assert.h>
#include <string.h>

#include "phase1r_real_local_hook_dry_run_in_fo4.h"

int main(void) {
    Phase1RRealLocalHookDryRunInFo4Config cfg;
    Phase1RRealLocalHookDryRunInFo4State st;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B00BFu, 0, 0 };

    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 7160;
    cfg.observed_f4se_build_text = "0.7.6";
    cfg.plugins_directory_present = true;
    cfg.f4se_log_present = false;
    cfg.require_plugins_directory = true;
    cfg.require_manifest_runtime_match = true;
    cfg.require_manifest_f4se_match = true;

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(!phase1r_rlhdf_run(&f4se, &provider, &cfg));
    st = phase1r_rlhdf_state();
    assert(st.attempted);
    assert(st.runtime_matches_manifest);
    assert(!st.f4se_matches_manifest);
    assert(st.dry_run_resolve_only);
    assert(st.runtime_profile_supported);
    assert(st.update_address_resolved);
    assert(st.input_address_resolved);
    assert(!st.ready_for_armed_attempt);
    return 0;
}
