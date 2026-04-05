#include "player_hook_discovery.h"

#include <string.h>

#include "commonlibf4_player_hook_runtime_profile.h"

#define F4MP_RUNTIME_1_11_191 0x010B0BF0u
#define F4MP_RUNTIME_1_11_191_LEGACY 0x010B00BFu

static const PlayerHookDiscoveryCandidate g_candidates[] = {
    {
        PLAYER_HOOK_CANDIDATE_UPDATE_BEFORE_INPUT,
        "movement_playercharacter_before_input_path",
        "PlayerCharacter::Update",
        "PlayerControls::Update",
        100u,
        true,
        false
    },
    {
        PLAYER_HOOK_CANDIDATE_UPDATE_AFTER_INPUT,
        "movement_playercharacter_after_input_path",
        "PlayerCharacter::Update",
        "PostInput",
        80u,
        true,
        false
    },
    {
        PLAYER_HOOK_CANDIDATE_ACTOR_UPDATE_PLAYER_FILTERED,
        "movement_actor_planner_family",
        "Actor::Update",
        "PlayerFilter",
        60u,
        true,
        false
    },
    {
        PLAYER_HOOK_CANDIDATE_PLAYER_CONTROLS_UPDATE,
        "movement_playercontrols_input_path_rejected",
        "PlayerControls::Update",
        "PlayerControlsMovementData",
        10u,
        false,
        true
    }
};

static bool slice_runtime_is_ae(const F4SEInterfaceMock* f4se) {
    if (!f4se || f4se->is_editor) {
        return false;
    }
    return f4se->runtime_version == F4MP_RUNTIME_1_11_191 ||
           f4se->runtime_version == F4MP_RUNTIME_1_11_191_LEGACY;
}

static HookAddressSource map_provider_source(CommonLibF4AddressSource source) {
    switch (source) {
        case CLF4_ADDR_SOURCE_STUB:
            return HOOK_ADDRESS_SOURCE_STUB;
        case CLF4_ADDR_SOURCE_ADDRESS_LIBRARY:
            return HOOK_ADDRESS_SOURCE_ADDRESS_LIBRARY;
        case CLF4_ADDR_SOURCE_PATTERN_SCAN:
            return HOOK_ADDRESS_SOURCE_PATTERN_SCAN;
        case CLF4_ADDR_SOURCE_MANUAL_OFFSET:
            return HOOK_ADDRESS_SOURCE_MANUAL_OFFSET;
        case CLF4_ADDR_SOURCE_TRUSTED_PROFILE:
            return HOOK_ADDRESS_SOURCE_TRUSTED_PROFILE;
        case CLF4_ADDR_SOURCE_UNRESOLVED:
        case CLF4_ADDR_SOURCE_UNKNOWN:
        default:
            return HOOK_ADDRESS_SOURCE_UNRESOLVED;
    }
}

static void fill_runtime_profile(
    const F4SEInterfaceMock* f4se,
    HookDiscoveryRuntimeProfile* out_profile
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;
    memset(&profile_resolution, 0, sizeof(profile_resolution));
    clf4_phrprof_resolve(NULL, f4se, &profile_resolution);

    memset(out_profile, 0, sizeof(*out_profile));
    out_profile->runtime_version = f4se ? f4se->runtime_version : 0u;
    out_profile->runtime_supported = profile_resolution.runtime_supported;
    out_profile->build_signature_supported = profile_resolution.build_signature_supported;
    out_profile->profile_name = profile_resolution.profile_name;
}

static void fill_resolution_pair(
    const CommonLibF4AddressProvider* provider,
    CommonLibF4AddressKey update_key,
    CommonLibF4AddressKey input_key,
    bool need_update,
    bool need_input,
    PlayerHookDiscoveryResolution* out_resolution
) {
    CommonLibF4AddressResolutionInfo update_info;
    CommonLibF4AddressResolutionInfo input_info;
    bool ok_update = false;
    bool ok_input = false;

    bool runtime_supported = out_resolution->runtime_supported;
    bool build_signature_supported = out_resolution->build_signature_supported;

    memset(&update_info, 0, sizeof(update_info));
    memset(&input_info, 0, sizeof(input_info));
    memset(out_resolution, 0, sizeof(*out_resolution));

    out_resolution->runtime_supported = runtime_supported;
    out_resolution->build_signature_supported = build_signature_supported;

    if (provider && need_update) {
        ok_update = clf4_address_provider_describe(provider, update_key, &update_info);
    }
    if (provider && need_input) {
        ok_input = clf4_address_provider_describe(provider, input_key, &input_info);
    }

    out_resolution->update_addr = ok_update ? update_info.addr : 0u;
    out_resolution->input_addr = ok_input ? input_info.addr : 0u;
    out_resolution->resolved = (!need_update || ok_update) && (!need_input || ok_input);

    if (need_update && ok_update) {
        out_resolution->address_source = map_provider_source(update_info.source);
        out_resolution->executable_memory_valid = update_info.executable_memory_valid;
    } else if (need_input && ok_input) {
        out_resolution->address_source = map_provider_source(input_info.source);
        out_resolution->executable_memory_valid = input_info.executable_memory_valid;
    } else {
        out_resolution->address_source = HOOK_ADDRESS_SOURCE_UNRESOLVED;
        out_resolution->executable_memory_valid = false;
    }

    if (need_update && need_input && ok_update && ok_input) {
        if (update_info.source == CLF4_ADDR_SOURCE_STUB || input_info.source == CLF4_ADDR_SOURCE_STUB) {
            out_resolution->address_source = HOOK_ADDRESS_SOURCE_STUB;
        }
        out_resolution->executable_memory_valid =
            update_info.executable_memory_valid && input_info.executable_memory_valid;
    }
}

static void finalize_assessment(PlayerHookDiscoveryAssessment* out_assessment, uint32_t score_if_armable) {
    if (!out_assessment->resolution.runtime_supported) {
        out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_UNSUPPORTED_RUNTIME;
    } else if (!out_assessment->resolution.build_signature_supported) {
        out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_UNSUPPORTED_BUILD;
    } else if (!out_assessment->resolution.resolved) {
        out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_UNRESOLVED_REQUIRED_ADDRESS;
    } else if (out_assessment->resolution.address_source == HOOK_ADDRESS_SOURCE_STUB) {
        out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE;
    } else if (!out_assessment->resolution.executable_memory_valid) {
        out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_NON_EXECUTABLE_TARGET;
    } else {
        out_assessment->armable = true;
        out_assessment->score = score_if_armable;
    }
}

static void assess_controls_input(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    PlayerHookDiscoveryAssessment* out_assessment
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;

    memset(&profile_resolution, 0, sizeof(profile_resolution));
    clf4_phrprof_resolve(NULL, f4se, &profile_resolution);

    memset(out_assessment, 0, sizeof(*out_assessment));
    out_assessment->candidate = g_candidates[1];
    out_assessment->resolution.runtime_supported = profile_resolution.runtime_supported;
    out_assessment->resolution.build_signature_supported = profile_resolution.build_signature_supported;

    fill_resolution_pair(
        provider,
        CLF4_ADDR_PLAYER_UPDATE,
        CLF4_ADDR_PLAYER_INPUT,
        false,
        true,
        &out_assessment->resolution);

    finalize_assessment(out_assessment, 10u);
    out_assessment->armable = false;
    out_assessment->score = 0u;
    out_assessment->dry_run_only = true;
    out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_WEAK_CONFIDENCE;
}

static void assess_commit_family(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    PlayerHookDiscoveryAssessment* out_assessment
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;

    memset(&profile_resolution, 0, sizeof(profile_resolution));
    clf4_phrprof_resolve(NULL, f4se, &profile_resolution);

    memset(out_assessment, 0, sizeof(*out_assessment));
    out_assessment->candidate = g_candidates[0];
    out_assessment->resolution.runtime_supported = profile_resolution.runtime_supported;
    out_assessment->resolution.build_signature_supported = profile_resolution.build_signature_supported;

    fill_resolution_pair(
        provider,
        CLF4_ADDR_PLAYER_UPDATE,
        CLF4_ADDR_PLAYER_INPUT,
        true,
        false,
        &out_assessment->resolution);

    finalize_assessment(out_assessment, 100u);
}

static void assess_actor_planner(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    PlayerHookDiscoveryAssessment* out_assessment
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;

    memset(&profile_resolution, 0, sizeof(profile_resolution));
    clf4_phrprof_resolve(NULL, f4se, &profile_resolution);

    memset(out_assessment, 0, sizeof(*out_assessment));
    out_assessment->candidate = g_candidates[2];
    out_assessment->resolution.runtime_supported = profile_resolution.runtime_supported;
    out_assessment->resolution.build_signature_supported = profile_resolution.build_signature_supported;

    fill_resolution_pair(
        provider,
        CLF4_ADDR_ACTOR_UPDATE,
        CLF4_ADDR_PLAYER_INPUT,
        true,
        false,
        &out_assessment->resolution);

    finalize_assessment(out_assessment, 60u);
}

static void assess_post_input_placeholder(
    const F4SEInterfaceMock* f4se,
    PlayerHookDiscoveryAssessment* out_assessment
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;

    memset(&profile_resolution, 0, sizeof(profile_resolution));
    clf4_phrprof_resolve(NULL, f4se, &profile_resolution);

    memset(out_assessment, 0, sizeof(*out_assessment));
    out_assessment->candidate = g_candidates[1];
    out_assessment->resolution.runtime_supported = profile_resolution.runtime_supported;
    out_assessment->resolution.build_signature_supported = profile_resolution.build_signature_supported;
    out_assessment->resolution.resolved = false;
    out_assessment->rejection_reason = HOOK_DISCOVERY_REJECT_WEAK_CONFIDENCE;
    out_assessment->dry_run_only = true;
}

static void force_slice_runtime_gate(PlayerHookDiscoveryAssessment* assessment) {
    if (!assessment) {
        return;
    }
    assessment->armable = false;
    assessment->score = 0u;
    assessment->resolution.runtime_supported = false;
    assessment->resolution.build_signature_supported = false;
    assessment->rejection_reason = HOOK_DISCOVERY_REJECT_UNSUPPORTED_RUNTIME;
}

bool phd_run_player_discovery(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    HookDiscoveryReport* out_report
) {
    HookDiscoveryReport report;
    uint32_t i;
    uint32_t best_index = 0u;
    uint32_t best_score = 0u;
    bool found_armable = false;
    const bool ae_runtime = slice_runtime_is_ae(f4se);

    memset(&report, 0, sizeof(report));
    fill_runtime_profile(f4se, &report.runtime_profile);
    report.assessment_count = 4u;
    report.selected_candidate_id = PLAYER_HOOK_CANDIDATE_INVALID;
    report.real_execution_evidence = false;

    assess_commit_family(f4se, provider, &report.assessments[0]);
    assess_post_input_placeholder(f4se, &report.assessments[1]);
    assess_actor_planner(f4se, provider, &report.assessments[2]);
    assess_controls_input(f4se, provider, &report.assessments[3]);

    if (!ae_runtime) {
        report.runtime_profile.runtime_supported = false;
        report.runtime_profile.build_signature_supported = false;
        for (i = 0; i < report.assessment_count; ++i) {
            force_slice_runtime_gate(&report.assessments[i]);
        }
        report.recommended_action = HOOK_DISCOVERY_ACTION_UNSUPPORTED_RUNTIME;
        if (out_report) {
            *out_report = report;
        }
        return false;
    }

    for (i = 0; i < report.assessment_count; ++i) {
        if (report.assessments[i].armable && (!found_armable || report.assessments[i].score > best_score)) {
            found_armable = true;
            best_index = i;
            best_score = report.assessments[i].score;
        }
    }

    if (found_armable) {
        report.assessments[best_index].selected = true;
        report.selected_candidate_id = report.assessments[best_index].candidate.candidate_id;
        report.recommended_action = HOOK_DISCOVERY_ACTION_INSTALL_LIVE;
    } else if (report.assessments[0].rejection_reason == HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE ||
               report.assessments[1].rejection_reason == HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE ||
               report.assessments[2].rejection_reason == HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE ||
               report.assessments[3].rejection_reason == HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE) {
        report.recommended_action = HOOK_DISCOVERY_ACTION_REJECT_STUB_PROVIDER;
        report.simulated_only = true;
    } else {
        report.recommended_action = HOOK_DISCOVERY_ACTION_TRY_NEXT_CANDIDATE;
    }

    if (out_report) {
        *out_report = report;
    }
    return found_armable;
}
