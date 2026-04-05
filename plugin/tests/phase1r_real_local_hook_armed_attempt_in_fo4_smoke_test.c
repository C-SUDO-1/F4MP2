#include <assert.h>
#include <string.h>

#include "commonlibf4_address_provider.h"
#include "phase1r_real_local_hook_armed_attempt_in_fo4.h"

static const char* kRuntimeLog =
    "F4SE runtime: initialize (version = 0.7.7 010B0BF0 01DCB5D9E10E0B01, os = 6.2 (9200))\n"
    "plugin directory = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\\n"
    "scanning plugin directory C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\\n"
    "checking plugin GFXBooster.dll\n"
    "config path = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\f4se.ini\n"
    "init complete\n"
    "RegisterPapyrusFunctions_Hook\n";

static const char* kLoaderLog =
    "f4se loader 00070070 01DCB5D9E0D9DA16 6.2 (9200)\n"
    "config path = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\f4se.ini\n"
    "launching: Fallout4.exe (C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Fallout4.exe)\n"
    "dll = C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\f4se_1_11_191.dll\n"
    "DLL and EXE signature/serial match\n"
    "hook thread complete\n"
    "launching\n";

int main(void) {
    F4SEInterfaceMock f4se;
    CommonLibF4AddressProvider provider;
    Phase1RRealLocalHookArmedAttemptInFo4Config cfg;
    Phase1RRealLocalHookArmedAttemptInFo4State st;

    memset(&f4se, 0, sizeof(f4se));
    memset(&cfg, 0, sizeof(cfg));

    f4se.runtime_version = 0x010B00BFu;
    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);

    lpo_init(501);
    msb_init(NULL);

    cfg.dry_run_config.expected_local_player_id = 501;
    cfg.dry_run_config.observed_f4se_build_text = "0.7.7";
    cfg.dry_run_config.plugins_directory_present = true;
    cfg.dry_run_config.f4se_log_present = true;

    cfg.wiring_config.require_wire_ready_after_activation = false;
    cfg.wiring_config.emit_validation_sample = true;
    cfg.wiring_config.build_movement_message = true;
    cfg.wiring_config.candidate_config.install_config.expected_local_player_id = 501;
    cfg.wiring_config.candidate_config.install_config.require_hook_correct = true;
    cfg.wiring_config.candidate_config.install_config.patch_allowed = true;
    cfg.wiring_config.candidate_config.install_config.require_update_address = true;
    cfg.wiring_config.candidate_config.install_config.require_input_address = true;
    cfg.wiring_config.candidate_config.install_config.require_callback_after_install = true;
    cfg.wiring_config.candidate_config.install_config.patch_policy = clf4_phpp_default_armed();
    cfg.wiring_config.candidate_config.install_config.site_prototype = clf4_phdsp_default();
    cfg.wiring_config.candidate_config.callback_source_symbol = "PlayerCharacter::Update";
    cfg.wiring_config.candidate_config.callback_guard_symbol = "PlayerControls::Update";
    cfg.wiring_config.candidate_config.require_supported_runtime_profile = true;
    cfg.wiring_config.callback_validation_config.expected_local_player_id = 501;
    cfg.wiring_config.callback_validation_config.min_callback_forward_count = 1;
    cfg.wiring_config.callback_validation_config.min_callback_accept_count = 1;
    cfg.wiring_config.callback_validation_config.require_runtime_profile_supported = true;
    cfg.wiring_config.callback_validation_config.require_callback_registered = true;
    cfg.wiring_config.callback_validation_config.require_provider_ready = true;
    cfg.wiring_config.callback_validation_config.require_captured_state = true;
    cfg.wiring_config.callback_validation_config.require_expected_player_id_match = true;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.local_player_id = 501;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.remote_player_id = 501;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_toolchain_match = true;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_callback_validation = true;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_proxy_validation = false;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_remote_runtime_present = false;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_scene_present = false;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.require_driver_present = false;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.evidence_config.validation_config.callback_validation_config = cfg.wiring_config.callback_validation_config;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.require_candidate_activated = true;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.require_runtime_profile_supported = true;
    cfg.wiring_config.readiness_config.wire_plan_config.fallback_config.arming_config.require_patch_policy_armed = true;
    cfg.wiring_config.validation_sample.event.player_id = 501;
    cfg.wiring_config.validation_sample.event.position.x = 12.0f;
    cfg.wiring_config.validation_sample.event.position.y = 24.0f;
    cfg.wiring_config.validation_sample.event.position.z = 36.0f;
    cfg.wiring_config.validation_sample.event.rotation.yaw = 45.0f;
    cfg.wiring_config.validation_sample.event.velocity.x = 1.5f;

    cfg.log_probe_config.loader_log_text = kLoaderLog;
    cfg.log_probe_config.runtime_log_text = kRuntimeLog;
    cfg.log_probe_config.expected_f4se_build_text = "0.7.7";
    cfg.log_probe_config.expected_runtime_dll_name = "f4se_1_11_191.dll";
    cfg.log_probe_config.expected_plugin_dll_name = "fo4_coop_plugin_stub.dll";
    cfg.require_logs_healthy = true;
    cfg.require_plugin_absent_for_first_drop = true;

    assert(!phase1r_rlhaf_attempt(&f4se, &provider, &cfg));
    st = phase1r_rlhaf_state();
    assert(st.attempted);
    assert(st.dry_run_ready);
    assert(st.wiring_ready);
    assert(st.logs_healthy);
    assert(!st.expected_plugin_seen);
    assert(st.ready_for_first_plugin_drop);
    assert(!st.armed_attempt_preconditions_met);
    assert(st.recommended_action == PHASE1R_RLHA_ACTION_BUILD_PLUGIN_DLL_DROP);
    assert(strcmp(phase1r_rlhaf_action_name(st.recommended_action), "build_plugin_dll_drop") == 0);
    assert(strcmp(st.runtime_profile_name, "fo4_1_11_191_player_character_update_before_input") == 0);
    return 0;
}
