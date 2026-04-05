#include "hook_discovery_report.h"

#include <stdio.h>
#include <string.h>

static const char* action_name(HookDiscoveryRecommendedAction action) {
    switch (action) {
        case HOOK_DISCOVERY_ACTION_INSTALL_LIVE: return "install_live";
        case HOOK_DISCOVERY_ACTION_DRY_RUN_ONLY: return "dry_run_only";
        case HOOK_DISCOVERY_ACTION_REJECT_STUB_PROVIDER: return "reject_stub_provider";
        case HOOK_DISCOVERY_ACTION_COLLECT_REAL_ENTRY_PROBE: return "collect_real_entry_probe";
        case HOOK_DISCOVERY_ACTION_TRY_NEXT_CANDIDATE: return "try_next_candidate";
        case HOOK_DISCOVERY_ACTION_UNSUPPORTED_RUNTIME: return "unsupported_runtime";
        case HOOK_DISCOVERY_ACTION_NONE:
        default:
            return "none";
    }
}

static const char* source_name(HookAddressSource source) {
    switch (source) {
        case HOOK_ADDRESS_SOURCE_STUB: return "stub";
        case HOOK_ADDRESS_SOURCE_ADDRESS_LIBRARY: return "address_library";
        case HOOK_ADDRESS_SOURCE_PATTERN_SCAN: return "pattern_scan";
        case HOOK_ADDRESS_SOURCE_MANUAL_OFFSET: return "manual_offset";
        case HOOK_ADDRESS_SOURCE_TRUSTED_PROFILE: return "trusted_profile";
        case HOOK_ADDRESS_SOURCE_UNRESOLVED:
        default:
            return "unresolved";
    }
}

static const char* rejection_name(HookDiscoveryRejectionReason reason) {
    switch (reason) {
        case HOOK_DISCOVERY_REJECT_UNSUPPORTED_RUNTIME: return "unsupported_runtime";
        case HOOK_DISCOVERY_REJECT_UNSUPPORTED_BUILD: return "unsupported_build";
        case HOOK_DISCOVERY_REJECT_UNRESOLVED_REQUIRED_ADDRESS: return "unresolved_required_address";
        case HOOK_DISCOVERY_REJECT_STUB_ADDRESS_SOURCE: return "stub_address_source";
        case HOOK_DISCOVERY_REJECT_NON_EXECUTABLE_TARGET: return "non_executable_target";
        case HOOK_DISCOVERY_REJECT_WEAK_CONFIDENCE: return "weak_confidence";
        case HOOK_DISCOVERY_REJECT_PATCH_POLICY: return "patch_policy";
        case HOOK_DISCOVERY_REJECT_NONE:
        default:
            return "none";
    }
}

void hdr_reset(HookDiscoveryReport* report) {
    if (!report) {
        return;
    }
    memset(report, 0, sizeof(*report));
}

void hdr_append_trace(const HookDiscoveryReport* report) {
    FILE* f = NULL;
    unsigned int i;
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_hook_discovery_trace.txt";

    if (!report) {
        return;
    }

    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    fprintf(f,
            "runtime=0x%08X runtime_supported=%d build_signature_supported=%d profile=%s selected_candidate=%u action=%s simulated_only=%d real_execution_evidence=%d assessment_count=%u\n",
            (unsigned int)report->runtime_profile.runtime_version,
            report->runtime_profile.runtime_supported ? 1 : 0,
            report->runtime_profile.build_signature_supported ? 1 : 0,
            report->runtime_profile.profile_name ? report->runtime_profile.profile_name : "",
            (unsigned int)report->selected_candidate_id,
            action_name(report->recommended_action),
            report->simulated_only ? 1 : 0,
            report->real_execution_evidence ? 1 : 0,
            (unsigned int)report->assessment_count);

    for (i = 0; i < report->assessment_count; ++i) {
        const PlayerHookDiscoveryAssessment* a = &report->assessments[i];
        fprintf(f,
                "candidate=%u name=%s source_symbol=%s guard_symbol=%s selected=%d score=%u armable=%d dry_run_only=%d resolved=%d address_source=%s update=0x%llX input=0x%llX exec_valid=%d rejection=%s\n",
                (unsigned int)a->candidate.candidate_id,
                a->candidate.candidate_name ? a->candidate.candidate_name : "",
                a->candidate.source_symbol ? a->candidate.source_symbol : "",
                a->candidate.guard_symbol ? a->candidate.guard_symbol : "",
                a->selected ? 1 : 0,
                (unsigned int)a->score,
                a->armable ? 1 : 0,
                a->dry_run_only ? 1 : 0,
                a->resolution.resolved ? 1 : 0,
                source_name(a->resolution.address_source),
                (unsigned long long)a->resolution.update_addr,
                (unsigned long long)a->resolution.input_addr,
                a->resolution.executable_memory_valid ? 1 : 0,
                rejection_name(a->rejection_reason));
    }

    fclose(f);
}
