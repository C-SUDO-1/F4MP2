#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "test_service_process.h"
#include "movement_live_client_bridge.h"
#include "commonlibf4_player_hook_observer_bridge.h"
#include "fo4_scene_proxy_backend_stub.h"
#include "fo4_proxy_actor_driver.h"
#include "remote_proxy_manager.h"
#include "proxy_runtime.h"
int main(void) {
    F4mpServiceProcess svc; MovementLiveClientBridge a, b; bool sent=false, applied=false; int i; PlayerId remote_player_id; CommonLibF4PlayerHookArgs args; const Fo4SceneProxyPlayerState* scene_remote; const Fo4ProxyActorState* driver_remote;
    setenv("F4MP_PLAYER_TIMEOUT_TICKS", "5", 1); assert(f4mp_spawn_service(&svc, "7802")); usleep(200000);
    proxy_runtime_init(); fpad_reset_default_driver(); assert(mlcb_open(&a,"127.0.0.1",7802,"guid-timeout-rs-a","AliceTRS",0x1234u)); assert(mlcb_open(&b,"127.0.0.1",7802,"guid-timeout-rs-b","BobTRS",0x1234u));
    for(i=0;i<120;++i){ assert(mlcb_tick(&a,&sent,&applied)); assert(mlcb_tick(&b,&sent,&applied)); if(mlcb_local_player_id(&a)&&mlcb_local_player_id(&b)) break; usleep(10000);} assert(mlcb_local_player_id(&a)!=0); assert(mlcb_local_player_id(&b)!=0); remote_player_id = mlcb_local_player_id(&a);
    lpo_init(remote_player_id); clf4_phob_install(remote_player_id); rpm_init(mlcb_local_player_id(&b)); fo4_scene_proxy_backend_stub_install(); memset(&args,0,sizeof(args)); args.event.player_id = remote_player_id; args.event.position.x = 55.0f; args.event.position.y = 11.0f; args.event.rotation.yaw = 180.0f; args.event.stance = STANCE_RUN; args.event.anim_state = 3u; assert(clf4_phob_submit(&args));
    for(i=0;i<120;++i){ assert(mlcb_tick(&a,&sent,&applied)); assert(mlcb_tick(&b,&sent,&applied)); scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id); driver_remote = fpad_get_state(remote_player_id); if(scene_remote && driver_remote && rpm_get_remote_player(remote_player_id) && scene_remote->last_update.position.x == 55.0f && driver_remote->position.x == 55.0f) break; usleep(10000);} assert(rpm_get_remote_player(remote_player_id) != 0); assert(fo4_scene_proxy_backend_stub_get(remote_player_id) != 0); assert(fpad_get_state(remote_player_id) != 0);
    for(i=0;i<180;++i){ assert(mlcb_tick(&b,&sent,&applied)); scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id); driver_remote = fpad_get_state(remote_player_id); if(rpm_get_remote_player(remote_player_id) == 0 && scene_remote && scene_remote->despawned && driver_remote && driver_remote->despawned) break; usleep(10000);} scene_remote = fo4_scene_proxy_backend_stub_get(remote_player_id); driver_remote = fpad_get_state(remote_player_id); assert(rpm_get_remote_player(remote_player_id) == 0); assert(scene_remote != 0); assert(scene_remote->despawned); assert(!scene_remote->visible); assert(driver_remote != 0); assert(driver_remote->despawned); assert(!driver_remote->visible); assert(driver_remote->despawn_count == 1u); assert(fpad_despawn_count() == 1u); assert(fo4_scene_proxy_backend_stub_despawn_count() == 1u);
    clf4_phob_uninstall(); ptc_close(&a.client); mlcb_close(&b); f4mp_stop_service(&svc); return 0; }
