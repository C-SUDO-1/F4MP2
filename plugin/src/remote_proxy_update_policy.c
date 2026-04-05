#include "remote_proxy_update_policy.h"
#include <string.h>

static RemoteProxyUpdatePolicy g_policy;

static float clamp01(float v){ if(v<0.0f) return 0.0f; if(v>1.0f) return 1.0f; return v; }
static float lerpf(float a,float b,float t){ return a + (b-a)*t; }
static float dist_sq(Vec3f a, Vec3f b){ float dx=a.x-b.x, dy=a.y-b.y, dz=a.z-b.z; return dx*dx+dy*dy+dz*dz; }

void rpup_init_default(void){
    memset(&g_policy,0,sizeof(g_policy));
    g_policy.enabled=true;
    g_policy.teleport_distance=8.0f;
    g_policy.position_lerp_alpha=0.25f;
    g_policy.rotation_lerp_alpha=0.35f;
}

void rpup_set(const RemoteProxyUpdatePolicy* policy){
    if(!policy){ rpup_init_default(); return; }
    g_policy=*policy;
    g_policy.position_lerp_alpha=clamp01(g_policy.position_lerp_alpha);
    g_policy.rotation_lerp_alpha=clamp01(g_policy.rotation_lerp_alpha);
}

RemoteProxyUpdatePolicy rpup_get(void){ return g_policy; }

void rpup_apply_player_update_ex(const ProxyPlayerRecord* current, ProxyPlayerUpdateSpec* update, RemoteProxyUpdatePolicyResult* out_result){
    float teleport_sq; float pt; float rt; float d;
    if(out_result) memset(out_result, 0, sizeof(*out_result));
    if(!current||!update) return;
    d = dist_sq(current->position, update->position);
    if(out_result){ out_result->policy_enabled = g_policy.enabled; out_result->source_distance = d; }
    if(!g_policy.enabled) return;
    teleport_sq=g_policy.teleport_distance*g_policy.teleport_distance;
    if(d>teleport_sq) {
        if(out_result) out_result->teleported = true;
        return;
    }
    pt=clamp01(g_policy.position_lerp_alpha);
    rt=clamp01(g_policy.rotation_lerp_alpha);
    update->position.x=lerpf(current->position.x, update->position.x, pt);
    update->position.y=lerpf(current->position.y, update->position.y, pt);
    update->position.z=lerpf(current->position.z, update->position.z, pt);
    update->rotation.pitch=lerpf(current->rotation.pitch, update->rotation.pitch, rt);
    update->rotation.yaw=lerpf(current->rotation.yaw, update->rotation.yaw, rt);
    update->rotation.roll=lerpf(current->rotation.roll, update->rotation.roll, rt);
    if(out_result){ out_result->smoothed_position = true; out_result->smoothed_rotation = true; }
}

void rpup_apply_player_update(const ProxyPlayerRecord* current, ProxyPlayerUpdateSpec* update){
    rpup_apply_player_update_ex(current, update, 0);
}
