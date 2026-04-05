#include "fo4_proxy_actor_live_validation.h"

#include <string.h>

#include "fo4_proxy_actor_driver.h"
#include "fo4_proxy_actor_live_candidate.h"
#include "fo4_scene_proxy_backend_stub.h"

static bool same_vec3(Vec3f a, Vec3f b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static bool same_rot3(Rot3f a, Rot3f b) {
    return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll;
}

void fpalv_reset(void) {
}

static void populate_common(PlayerId player_id, Fo4ProxyActorLiveValidationResult* out_result,
                            const Fo4ProxyActorLiveCandidateState* candidate,
                            const Fo4SceneProxyPlayerState* scene,
                            const Fo4ProxyActorState* driver) {
    memset(out_result, 0, sizeof(*out_result));
    out_result->player_id = player_id;
    out_result->candidate_enabled = candidate->enabled;
    out_result->candidate_last_player_match = (candidate->last_player_id == player_id);
    out_result->spawn_seen = candidate->spawn_count != 0;
    out_result->move_seen = candidate->move_count != 0;
    out_result->rotate_seen = candidate->rotate_count != 0;
    out_result->despawn_seen = candidate->despawn_count != 0;

    out_result->scene_present = scene != 0;
    if (scene) {
        out_result->scene_visible = scene->visible;
        out_result->scene_despawned = scene->despawned;
        out_result->scene_actor_handle = scene->actor_handle;
    }

    out_result->driver_present = driver != 0;
    if (driver) {
        out_result->driver_visible = driver->visible;
        out_result->driver_despawned = driver->despawned;
        out_result->driver_actor_handle = driver->actor_handle;
        out_result->proxy_base_form_id = driver->proxy_base_form_id;
        out_result->proxy_base_form_id_present = driver->proxy_base_form_id != 0;
    }

    if (scene && driver) {
        out_result->actor_handle_match = scene->actor_handle != 0 && scene->actor_handle == driver->actor_handle;
        out_result->presented_position_match = same_vec3(scene->presented_position, driver->position);
        out_result->presented_rotation_match = same_rot3(scene->presented_rotation, driver->rotation);
    }
}

bool fpalv_validate_present(PlayerId player_id, Fo4ProxyActorLiveValidationResult* out_result) {
    Fo4ProxyActorLiveCandidateState candidate;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;
    Fo4ProxyActorLiveValidationResult result;

    if (player_id == 0) return false;
    candidate = fpalc_state();
    scene = fo4_scene_proxy_backend_stub_get(player_id);
    driver = fpad_get_state(player_id);
    populate_common(player_id, &result, &candidate, scene, driver);

    result.validation_passed =
        result.candidate_enabled &&
        result.scene_present &&
        result.scene_visible && !result.scene_despawned &&
        result.driver_present && result.driver_visible && !result.driver_despawned &&
        result.actor_handle_match &&
        result.proxy_base_form_id_present &&
        result.presented_position_match &&
        result.presented_rotation_match &&
        result.candidate_last_player_match &&
        result.spawn_seen && result.move_seen && result.rotate_seen;

    if (out_result) *out_result = result;
    return result.validation_passed;
}

bool fpalv_validate_despawn(PlayerId player_id, Fo4ProxyActorLiveValidationResult* out_result) {
    Fo4ProxyActorLiveCandidateState candidate;
    const Fo4SceneProxyPlayerState* scene;
    const Fo4ProxyActorState* driver;
    Fo4ProxyActorLiveValidationResult result;

    if (player_id == 0) return false;
    candidate = fpalc_state();
    scene = fo4_scene_proxy_backend_stub_get(player_id);
    driver = fpad_get_state(player_id);
    populate_common(player_id, &result, &candidate, scene, driver);

    result.validation_passed =
        result.candidate_enabled &&
        result.scene_present && !result.scene_visible && result.scene_despawned &&
        result.driver_present && !result.driver_visible && result.driver_despawned &&
        result.candidate_last_player_match &&
        result.despawn_seen;

    if (out_result) *out_result = result;
    return result.validation_passed;
}
