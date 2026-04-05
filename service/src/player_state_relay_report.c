#include "player_state_relay_report.h"
#include "service_indices.h"

const PlayerStateRelayStats* psrr_get_player_stats(const SessionState* s, PlayerId player_id) {
    const PlayerRuntimeState* p = find_player_const(s, player_id);
    if (!p || !p->connected) return 0;
    return &p->relay_stats;
}
