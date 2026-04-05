#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_install_candidate.h"

int main(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    CommonLibF4PlayerHookInstallCandidateStats stats;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock supported = { 0x010A03D8u, 0, 0 };
    F4SEInterfaceMock unsupported = { 0x010A03D9u, 0, 0 };

    lpo_init(93);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 93;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;
    cfg.patch_policy = clf4_phpp_default_dry_run();
    cfg.site_prototype = clf4_phdsp_default();

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(!clf4_phic_install_with_provider(&unsupported, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.runtime_profile_name != NULL);
    assert(strcmp(stats.runtime_profile_name, "fo4_1_10_984_player_character_update_before_input") == 0);
    assert(stats.observed_runtime_version == 0x010A03D9u);
    assert(stats.required_runtime_version == 0x010A03D8u);
    assert(!stats.runtime_profile_supported);
    assert(!stats.site_candidate_update_resolved);
    assert(!stats.site_candidate_input_resolved);
    assert(!stats.patch_step_attempted);
    assert(stats.install_error == HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME);

    cfg.patch_policy = clf4_phpp_default_armed();
    assert(clf4_phic_install_with_provider(&supported, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.runtime_profile_supported);
    assert(stats.site_candidate_update_resolved);
    assert(stats.site_candidate_input_resolved);
    assert(stats.patch_step_attempted);
    clf4_phic_uninstall();
    return 0;
}
