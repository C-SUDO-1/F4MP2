#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "commonlibf4_local_player_provider_stub.h"
#include "fo4_proxy_actor_backend_stub.h"
#include "local_player_observer_provider.h"
#include "movement_live_client_bridge.h"
#include "proxy_runtime.h"
#include "test_service_process.h"
int main(void) {
    F4mpServiceProcess svc; MovementLiveClientBridge a, b; bool sent=false, applied=false; int i; ProviderState ps; LocalPlayerObserverProvider provider; const ProxyPlayerRecord* remote;
    setenv("F4MP_PLAYER_TIMEOUT_TICKS", "5", 1); assert(f4mp_spawn_service(&svc, "7792")); usleep(200000);
    proxy_runtime_init(); assert(mlcb_open(&a,"127.0.0.1",7792,"guid-life-a","AliceLife",0x1234u)); assert(mlcb_open(&b,"127.0.0.1",7792,"guid-life-b","BobLife",0x1234u));
    for(i=0;i<120;++i){ assert(mlcb_tick(&a,&sent,&applied)); assert(mlcb_tick(&b,&sent,&applied)); if(mlcb_local_player_id(&a)&&mlcb_local_player_id(&b)) break; usleep(10000);} assert(mlcb_local_player_id(&a)!=0); assert(mlcb_local_player_id(&b)!=0);
    clf4_lpps_init(); clf4_lpps_set_local_player(mlcb_local_player_id(&a)); lpo_clear_provider(); provider = clf4_lpps_provider(); lpo_install_provider(&provider, LPO_SOURCE_HOOKED_LIVE); fpabs_reset(); fpabs_install();
    ps.player_id = mlcb_local_player_id(&a); ps.position.x = 1.5f; ps.position.y = 2.5f; ps.rotation.yaw = 33.0f; ps.stance = STANCE_WALK; ps.valid = true; clf4_lpps_submit(&ps);
    for(i=0;i<80;++i){ assert(mlcb_tick(&a,&sent,&applied)); assert(mlcb_tick(&b,&sent,&applied)); remote = rpm_get_remote_player(mlcb_local_player_id(&a)); if(remote!=NULL) break; usleep(10000);} remote = rpm_get_remote_player(mlcb_local_player_id(&a)); assert(remote != NULL); assert(remote->position.x == 1.5f);
    lpo_clear_provider(); mlcb_close(&a); for(i=0;i<40;++i){ assert(mlcb_tick(&b,&sent,&applied)); usleep(10000);} mlcb_close(&b); f4mp_stop_service(&svc); return 0; }
