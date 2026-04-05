
#include <assert.h>
#include <string.h>
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"
int main(void){
    MsgPlayerState msg;
    proxy_runtime_init(); rpm_init(1); rpm_reset_rejected_stale_updates();
    memset(&msg,0,sizeof(msg)); msg.player_id=2; msg.position.x=10.0f; msg.rotation.yaw=10.0f; msg.state_sequence=5; msg.observed_tick=100;
    assert(rpm_apply_remote_player_state(&msg));
    assert(rpm_get_remote_player(2));
    assert(rpm_get_remote_player(2)->last_state_sequence==5);
    msg.position.x=20.0f; msg.state_sequence=4; msg.observed_tick=101;
    assert(rpm_apply_remote_player_state(&msg));
    assert(rpm_rejected_stale_updates()==1);
    assert(rpm_get_remote_player(2)->position.x!=20.0f);
    msg.position.x=30.0f; msg.state_sequence=5; msg.observed_tick=100;
    assert(rpm_apply_remote_player_state(&msg));
    assert(rpm_rejected_stale_updates()==2);
    msg.position.x=40.0f; msg.state_sequence=6; msg.observed_tick=102;
    assert(rpm_apply_remote_player_state(&msg));
    assert(rpm_get_remote_player(2)->last_state_sequence==6);
    return 0;
}
