#include "commonlibf4_player_hook_attempt_stub.h"
#include "commonlibf4_player_hook_patch_realization.h"
#include "player_hook_shim.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
static const char* TRACE_PATH = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_stub_hook_attempt_trace.txt";
static const char* TRUTH_TRACE_PATH = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_detour_truth_trace.txt";
static const CommonLibF4PlayerHookPatchPolicy* effective_policy(const CommonLibF4PlayerHookPatchPolicy* policy) { return policy ? policy : clf4_phpp_default_armed(); }

static bool resolution_is_stub(const CommonLibF4PlayerHookDetourSiteCandidateResolution* resolution) {
    uintptr_t max_addr = 0u;
    if (!resolution) {
        return false;
    }
    if (resolution->update_resolved && resolution->update_addr > max_addr) {
        max_addr = resolution->update_addr;
    }
    if (resolution->input_resolved && resolution->input_addr > max_addr) {
        max_addr = resolution->input_addr;
    }
    if (max_addr == 0u) {
        return false;
    }
    return max_addr < 0x100000u;
}


static void append_trace(
    const char* event_name,
    bool simulated_install,
    bool runtime_supported,
    bool build_signature_supported,
    const CommonLibF4PlayerHookDetourSiteCandidateResolution* resolution,
    bool patch_allowed,
    bool effective_patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy,
    const HookAttemptResult* result
) {
    const bool armed = policy && clf4_phpp_is_armed(policy);
    const bool dry_run = policy && clf4_phpp_is_dry_run(policy);
    const bool candidate_valid = resolution && resolution->valid;
    const bool candidate_runtime_supported = resolution && resolution->runtime_supported;
    const bool update_resolved = resolution && resolution->update_resolved;
    const bool input_resolved = resolution && resolution->input_resolved;
    const bool patch_step_attempted = result ? result->address_detail.patch_step_attempted : false;
    const bool patch_step_succeeded = result ? result->address_detail.patch_step_succeeded : false;
    const bool stub_resolution = resolution_is_stub(resolution);
    const bool prototype_only = simulated_install || stub_resolution;
    const bool real_arm_possible =
        armed && effective_patch_allowed && runtime_supported &&
        build_signature_supported && candidate_valid && candidate_runtime_supported &&
        !stub_resolution;
    FILE* f = NULL;
    fopen_s(&f, TRACE_PATH, "a");
    if (f) {
        fprintf(f,
                "event=%s simulated_install=%d armed=%d dry_run=%d patch_allowed=%d effective_patch_allowed=%d runtime_supported=%d build_signature_supported=%d candidate_valid=%d candidate_runtime_supported=%d update_resolved=%d input_resolved=%d patch_step_attempted=%d patch_step_succeeded=%d state=%d error=%d prototype_only=%d real_arm_possible=%d\n",
                event_name ? event_name : "unknown",
                simulated_install ? 1 : 0, armed ? 1 : 0, dry_run ? 1 : 0,
                patch_allowed ? 1 : 0, effective_patch_allowed ? 1 : 0,
                runtime_supported ? 1 : 0, build_signature_supported ? 1 : 0,
                candidate_valid ? 1 : 0, candidate_runtime_supported ? 1 : 0,
                update_resolved ? 1 : 0, input_resolved ? 1 : 0,
                patch_step_attempted ? 1 : 0, patch_step_succeeded ? 1 : 0,
                result ? (int)result->state : 0, result ? (int)result->error : 0,
                prototype_only ? 1 : 0, real_arm_possible ? 1 : 0);
        fclose(f);
    }
    f = NULL;
    fopen_s(&f, TRUTH_TRACE_PATH, "a");
    if (!f) return;
    fprintf(f,
            "event=%s prototype_only=%d real_arm_possible=%d armed=%d dry_run=%d effective_patch_allowed=%d runtime_supported=%d build_signature_supported=%d candidate_valid=%d candidate_runtime_supported=%d update_resolved=%d input_resolved=%d patch_step_attempted=%d patch_step_succeeded=%d install_state=%d install_error=%d\n",
            event_name ? event_name : "unknown",
            prototype_only ? 1 : 0, real_arm_possible ? 1 : 0,
            armed ? 1 : 0, dry_run ? 1 : 0, effective_patch_allowed ? 1 : 0,
            runtime_supported ? 1 : 0, build_signature_supported ? 1 : 0,
            candidate_valid ? 1 : 0, candidate_runtime_supported ? 1 : 0,
            update_resolved ? 1 : 0, input_resolved ? 1 : 0,
            patch_step_attempted ? 1 : 0, patch_step_succeeded ? 1 : 0,
            result ? (int)result->state : 0, result ? (int)result->error : 0);
    fclose(f);
}

static HookAttemptResult install_with_resolution(
    const CommonLibF4PlayerHookRuntimeProfileResolution* profile_resolution,
    const CommonLibF4PlayerHookDetourSiteCandidateResolution* resolution,
    uintptr_t effective_patch_target_addr,
    const char* effective_patch_target_symbol,
    uintptr_t detour_destination_addr,
    const char* detour_destination_label,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    const CommonLibF4PlayerHookPatchPolicy* active_policy = effective_policy(policy);
    const bool armed = clf4_phpp_is_armed(active_policy);
    const bool effective_patch_allowed = clf4_phpp_patch_step_allowed(active_policy, patch_allowed);
    const bool need_update = resolution && resolution->descriptor && resolution->descriptor->require_update_address;
    const bool need_input = resolution && resolution->descriptor && resolution->descriptor->require_input_address;
    const uint32_t resolved_count = ((need_update && resolution->update_resolved) ? 1u : 0u) + ((need_input && resolution->input_resolved) ? 1u : 0u);
    const uint32_t total_required = (need_update ? 1u : 0u) + (need_input ? 1u : 0u);
    HookAddressAttemptDetail detail = hook_address_attempt_detail_make(resolved_count, total_required, false, false);
    HookAttemptResult result;
    if (!profile_resolution || !profile_resolution->runtime_supported || !profile_resolution->build_signature_supported) {
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_FATAL, true, detail);
        append_trace("install_with_resolution_unsupported_runtime", true, false, false, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    if (!resolution || !resolution->valid || !resolution->runtime_supported) {
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_FATAL, true, detail);
        append_trace("install_with_resolution_invalid_candidate", true, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    if ((need_update && !resolution->update_resolved) || (need_input && !resolution->input_resolved)) {
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_FATAL, true, detail);
        append_trace("install_with_resolution_address_missing", true, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    if (resolution_is_stub(resolution)) {
        result = hook_attempt_result_make_detail(
            HOOK_FAMILY_PLAYER,
            HOOK_INSTALL_FAILED,
            HOOK_INSTALL_ERR_ADDRESS_MISSING,
            HOOK_BLOCKING_FATAL,
            true,
            detail
        );
        append_trace("install_with_resolution_stub_rejected", true, true, true, resolution, patch_allowed, false, active_policy, &result);
        return result;
    }

    if (!armed) {
        detail.patch_step_attempted = false;
        detail.patch_step_succeeded = false;
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_DEGRADABLE, true, detail);
        append_trace("install_with_resolution_dry_run", true, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    detail.patch_step_attempted = true;
    if (!effective_patch_allowed) {
        detail.patch_step_succeeded = false;
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_FATAL, true, detail);
        append_trace("install_with_resolution_patch_rejected", true, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    {
        CommonLibF4PlayerHookPatchRealizationPlan patch_plan;
        memset(&patch_plan, 0, sizeof(patch_plan));
        patch_plan.target_addr = effective_patch_target_addr;
        patch_plan.target_symbol = effective_patch_target_symbol;
        patch_plan.detour_destination_addr = detour_destination_addr;
        patch_plan.detour_destination_label = detour_destination_label;
        detail.patch_step_succeeded = clf4_phpr_attempt(&patch_plan);
    }
    if (!detail.patch_step_succeeded) {
        result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_FATAL, true, detail);
        append_trace("install_with_resolution_patch_not_realized", true, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
        return result;
    }
    result = hook_attempt_result_make_detail(HOOK_FAMILY_PLAYER, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true, detail);
    append_trace("install_with_resolution_installed_realized", false, true, true, resolution, patch_allowed, effective_patch_allowed, active_policy, &result);
    return result;
}

HookAttemptResult clf4_attempt_install_player_hook_stub(
    const F4SEInterfaceMock* f4se, bool address_available, bool patch_allowed, const CommonLibF4PlayerHookPatchPolicy* policy
) {
    const CommonLibF4PlayerHookPatchPolicy* active_policy = effective_policy(policy);
    HookAttemptResult result;
    if (!f4se || f4se->is_editor) {
        result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_FATAL, true);
        append_trace("legacy_stub_editor_or_null", true, false, false, NULL, patch_allowed, clf4_phpp_patch_step_allowed(active_policy, patch_allowed), active_policy, &result);
        return result;
    }
    if (f4se->runtime_version < 0x010A03D8u) {
        result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME, HOOK_BLOCKING_FATAL, true);
        append_trace("legacy_stub_old_runtime", true, false, false, NULL, patch_allowed, clf4_phpp_patch_step_allowed(active_policy, patch_allowed), active_policy, &result);
        return result;
    }
    if (!address_available) {
        result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_ADDRESS_MISSING, HOOK_BLOCKING_FATAL, true);
        append_trace("legacy_stub_address_missing", true, true, true, NULL, patch_allowed, clf4_phpp_patch_step_allowed(active_policy, patch_allowed), active_policy, &result);
        return result;
    }
    if (clf4_phpp_is_dry_run(active_policy)) {
        result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_PARTIAL, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_DEGRADABLE, true);
        append_trace("legacy_stub_dry_run", true, true, true, NULL, patch_allowed, clf4_phpp_patch_step_allowed(active_policy, patch_allowed), active_policy, &result);
        return result;
    }
    if (!clf4_phpp_patch_step_allowed(active_policy, patch_allowed)) {
        result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_PATCH_REJECTED, HOOK_BLOCKING_FATAL, true);
        append_trace("legacy_stub_patch_rejected", true, true, true, NULL, patch_allowed, false, active_policy, &result);
        return result;
    }
    result = hook_attempt_result_make(HOOK_FAMILY_PLAYER, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE, HOOK_BLOCKING_NONE, true);
    append_trace("legacy_stub_installed", true, true, true, NULL, patch_allowed, true, active_policy, &result);
    return result;
}

static uintptr_t patch_target_addr_for_prototype(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const CommonLibF4PlayerHookDetourSitePrototypeResolution* resolution
) {
    const CommonLibF4PlayerHookDetourSitePrototype* effective = prototype_site ? prototype_site : clf4_phdsp_default();
    if (effective && effective->require_update_address) {
        return resolution ? resolution->candidate_resolution.update_addr : 0u;
    }
    return resolution ? resolution->candidate_resolution.input_addr : 0u;
}

static const char* patch_target_symbol_for_prototype(const CommonLibF4PlayerHookDetourSitePrototype* prototype_site) {
    const CommonLibF4PlayerHookDetourSitePrototype* effective = prototype_site ? prototype_site : clf4_phdsp_default();
    if (effective && effective->require_update_address) {
        return effective->update_symbol_name;
    }
    return effective ? effective->input_symbol_name : NULL;
}

static uintptr_t patch_target_addr_for_descriptor(
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    const CommonLibF4PlayerHookDetourSiteCandidateResolution* resolution
) {
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* effective = descriptor ? descriptor : clf4_phdsc_default();
    if (effective && effective->require_update_address) {
        return resolution ? resolution->update_addr : 0u;
    }
    return resolution ? resolution->input_addr : 0u;
}

static const char* patch_target_symbol_for_descriptor(const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor) {
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* effective = descriptor ? descriptor : clf4_phdsc_default();
    if (effective && effective->require_update_address) {
        return "PlayerCharacter::Update";
    }
    return "PlayerControls::Update";
}

static uintptr_t detour_destination_addr_for_patch_target_symbol(const char* patch_target_symbol) {
    if (patch_target_symbol && strcmp(patch_target_symbol, "CreatePlayerControlHandlers") == 0) {
        return (uintptr_t)&phs_create_player_controls_manual_detour_thunk;
    }
    return (uintptr_t)&phs_note_manual_detour_entry;
}

static const char* detour_destination_label_for_patch_target_symbol(const char* patch_target_symbol) {
    if (patch_target_symbol && strcmp(patch_target_symbol, "CreatePlayerControlHandlers") == 0) {
        return "phs_create_player_controls_manual_detour_thunk";
    }
    return "phs_note_manual_detour_entry";
}


HookAttemptResult clf4_attempt_install_player_hook_with_prototype(
    const F4SEInterfaceMock* f4se, const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site, bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;
    CommonLibF4PlayerHookDetourSitePrototypeResolution resolution;
    clf4_phrprof_resolve(prototype_site, f4se, &profile_resolution);
    if (!profile_resolution.runtime_supported || !profile_resolution.build_signature_supported) {
        return install_with_resolution(&profile_resolution, NULL, 0u, NULL, detour_destination_addr_for_patch_target_symbol(NULL), detour_destination_label_for_patch_target_symbol(NULL), patch_allowed, policy);
    }
    clf4_phdsp_resolve(prototype_site, f4se, provider, &resolution);
    return install_with_resolution(&profile_resolution, &resolution.candidate_resolution, patch_target_addr_for_prototype(prototype_site, &resolution), patch_target_symbol_for_prototype(prototype_site), detour_destination_addr_for_patch_target_symbol(patch_target_symbol_for_prototype(prototype_site)), detour_destination_label_for_patch_target_symbol(patch_target_symbol_for_prototype(prototype_site)), patch_allowed, policy);
}

HookAttemptResult clf4_attempt_install_player_hook_with_candidate_descriptor(
    const F4SEInterfaceMock* f4se, const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor, bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
) {
    CommonLibF4PlayerHookRuntimeProfileResolution profile_resolution;
    CommonLibF4PlayerHookDetourSiteCandidateResolution resolution;
    clf4_phrprof_resolve(clf4_phdsp_default(), f4se, &profile_resolution);
    if (!profile_resolution.runtime_supported || !profile_resolution.build_signature_supported) {
        return install_with_resolution(&profile_resolution, NULL, 0u, NULL, detour_destination_addr_for_patch_target_symbol(NULL), detour_destination_label_for_patch_target_symbol(NULL), patch_allowed, policy);
    }
    clf4_phdsc_resolve(descriptor, f4se, provider, &resolution);
    return install_with_resolution(&profile_resolution, &resolution, patch_target_addr_for_descriptor(descriptor, &resolution), patch_target_symbol_for_descriptor(descriptor), detour_destination_addr_for_patch_target_symbol(patch_target_symbol_for_descriptor(descriptor)), detour_destination_label_for_patch_target_symbol(patch_target_symbol_for_descriptor(descriptor)), patch_allowed, policy);
}

HookAttemptResult clf4_attempt_install_player_hook_with_provider(
    const F4SEInterfaceMock* f4se, const CommonLibF4AddressProvider* provider,
    bool patch_allowed, const CommonLibF4PlayerHookPatchPolicy* policy
) {
    return clf4_attempt_install_player_hook_with_prototype(f4se, provider, clf4_phdsp_default(), patch_allowed, policy);
}
