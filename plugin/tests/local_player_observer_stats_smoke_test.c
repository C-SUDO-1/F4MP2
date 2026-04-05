#include <assert.h>
#include <string.h>
#include "local_player_observer.h"
#include "local_player_observer_provider.h"
#include "local_player_observer_stats.h"
typedef struct ProviderState { LocalPlayerObservedState state; int should_fail; } ProviderState;
static bool provider_capture(void* user, LocalPlayerObservedState* out_state) { ProviderState* p = (ProviderState*)user; if (p->should_fail) return false; *out_state = p->state; return true; }
int main(void) {
    ProviderState ps; LocalPlayerObserverProvider provider; LocalPlayerObservedState out; LocalPlayerObserverStats stats;
    memset(&ps,0,sizeof(ps)); memset(&provider,0,sizeof(provider)); memset(&out,0,sizeof(out));
    lpo_init(42); lpo_stats_reset();
    assert(!lpo_capture(&out)); stats = lpo_stats_get(); assert(stats.capture_attempts == 1); assert(stats.capture_failures == 1); assert(!stats.provider_installed);
    memset(&out,0,sizeof(out)); out.valid=true; out.player_id=42; out.position.x=1.0f; lpo_set_synthetic_state(&out);
    assert(lpo_capture(&out)); stats = lpo_stats_get(); assert(stats.capture_attempts == 2); assert(stats.synthetic_hits == 1); assert(!stats.last_capture_from_provider);
    memset(&ps,0,sizeof(ps)); ps.state.player_id=42; ps.state.position.x=7.0f; provider.user=&ps; provider.capture=provider_capture; lpo_install_provider(&provider);
    assert(lpo_capture(&out)); stats = lpo_stats_get(); assert(stats.provider_installed); assert(stats.provider_hits == 1); assert(stats.last_capture_from_provider); assert(out.position.x == 7.0f);
    lpo_clear_provider(); stats = lpo_stats_get(); assert(!stats.provider_installed); return 0;
}
