#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_install_candidate.h"

int main(void) {
    CommonLibF4PlayerHookInstallCandidateConfig cfg;
    CommonLibF4PlayerHookInstallCandidateStats stats;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    CommonLibF4PlayerHookArgs args;

    lpo_init(51);
    memset(&cfg, 0, sizeof(cfg));
    cfg.expected_local_player_id = 51;
    cfg.require_hook_correct = true;
    cfg.patch_allowed = true;
    cfg.require_update_address = true;
    cfg.require_input_address = true;
    cfg.require_callback_after_install = true;
    cfg.site_prototype = clf4_phdsp_default();

    provider = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
    assert(!clf4_phic_install_with_provider(&ok, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(stats.site_prototype_name != NULL);
    assert(strcmp(stats.site_prototype_name, "player_character_update_before_input") == 0);
    assert(strcmp(stats.update_symbol_name, "PlayerCharacter::Update") == 0);
    assert(strcmp(stats.input_symbol_name, "PlayerControls::Update") == 0);
    assert(stats.site_candidate_valid);
    assert(stats.site_candidate_runtime_supported);
    assert(stats.site_candidate_update_resolved);
    assert(!stats.site_candidate_input_resolved);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phic_install_with_provider(&ok, &provider, &cfg));
    stats = clf4_phic_stats();
    assert(strcmp(stats.site_prototype_name, "player_character_update_before_input") == 0);
    assert(stats.install_ready);

    memset(&args, 0, sizeof(args));
    args.event.player_id = 51;
    args.event.position.x = 3.0f;
    args.event.rotation.yaw = 42.0f;
    assert(clf4_player_hook_callback_stub_emit(&args));

    stats = clf4_phic_stats();
    assert(stats.hook_correct);
    assert(stats.callback_accept_count == 1);
    clf4_phic_uninstall();
    return 0;
}
