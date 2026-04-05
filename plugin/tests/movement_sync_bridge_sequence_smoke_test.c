
#include <assert.h>
#include <string.h>
#include "movement_sync_bridge.h"
#include "local_player_observer.h"
int main(void){
    LocalPlayerObservedState s; MsgPlayerState msg;
    memset(&s,0,sizeof(s)); s.valid=true; s.player_id=1; s.position.x=1.0f;
    lpo_init(1); lpo_set_synthetic_state(&s); msb_init(NULL);
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.state_sequence==1); assert(msg.observed_tick==1);
    s.position.x=2.0f; lpo_set_synthetic_state(&s);
    assert(msb_build_player_state_if_changed(&msg));
    assert(msg.state_sequence==2); assert(msg.observed_tick==2);
    assert(msb_last_state_sequence()==2); assert(msb_last_observed_tick()==2);
    return 0;
}
