#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_install_candidate.h"

int main(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    CommonLibF4PlayerHookInstallCandidateStats stats;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };

    lpo_init(71);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 71;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;
    cfg.patch_policy = clf4_phpp_default_dry_run();
    cfg.site_prototype = clf4_phdsp_default();

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(!clf4_phic_install_with_provider(&ok, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.install_attempted);
    assert(!stats.install_ready);
    assert(!stats.adapter_installed);
    assert(!stats.path_ready);
    assert(stats.dry_run_resolve_only);
    assert(!stats.armed_install);
    assert(!stats.patch_step_attempted);
    assert(stats.patch_policy_name != NULL);
    assert(strcmp(stats.patch_policy_name, "dry_run_resolve_only") == 0);
    assert(stats.install_state == HOOK_INSTALL_PARTIAL);
    assert(stats.install_error == HOOK_INSTALL_ERR_NONE);
    assert(stats.site_candidate_update_resolved);
    assert(stats.site_candidate_input_resolved);

    cfg.patch_policy = clf4_phpp_default_armed();
    assert(clf4_phic_install_with_provider(&ok, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.install_ready);
    assert(stats.armed_install);
    assert(!stats.dry_run_resolve_only);
    assert(stats.patch_step_attempted);
    assert(strcmp(stats.patch_policy_name, "armed_install") == 0);
    clf4_phic_uninstall();
    return 0;
}
