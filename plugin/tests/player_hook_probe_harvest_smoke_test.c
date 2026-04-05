#include <assert.h>
#include <string.h>

#include "player_hook_probe_harvest.h"
#include "player_hook_discovery.h"

int main(void) {
    HookDiscoveryReport report;
    PlayerHookProbeHarvestSnapshot snap;

    memset(&report, 0, sizeof(report));
    report.assessment_count = 1;
    report.assessments[0].rejection_reason = HOOK_DISCOVERY_REJECT_UNRESOLVED_REQUIRED_ADDRESS;
    report.probe_family = "PlayerCharacter::Update";
    report.probe_reason = "unresolved_required_address";
    report.probe_mode = "probe_only";

    phh_reset();
    phh_begin_unresolved_session(&report);
    snap = phh_snapshot();
    assert(snap.active);
    assert(snap.session_id != 0u);
    assert(snap.sample_count == 0u);

    phh_note_explicit_callsite("unit_test", 0x1000u, 0x0800u, false, true, true);
    phh_note_explicit_callsite("unit_test", 0x1000u, 0x0800u, false, true, true);
    snap = phh_snapshot();
    assert(snap.sample_count == 2u);
    assert(snap.cluster_count >= 1u);
    assert(snap.promoted_candidate_count >= 1u);

    phh_refresh_report(&report);
    assert(report.probe_active);
    assert(report.probe_session_id == snap.session_id);
    assert(report.harvest_sample_count == snap.sample_count);
    assert(report.promoted_candidate_count == snap.promoted_candidate_count);
    return 0;
}
