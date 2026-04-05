#include <assert.h>
#include <string.h>
#include "actor_event_translation.h"

int main(void) {
    ActorHostEvent ev;
    MsgActorSpawn spawn;
    MsgActorState state;
    MsgActorDespawn despawn;
    MsgDamageResult dmg;
    MsgDeathEvent death;

    memset(&ev, 0, sizeof(ev));
    ev.kind = AHE_SPAWN;
    ev.actor_net_id = 77;
    ev.base_form_id = 0x1234;
    ev.position.x = 1.0f;
    ev.actor_kind = ACTOR_KIND_CREATURE;
    ev.actor_flags = ACTF_HOSTILE;
    assert(ahe_translate_spawn(&ev, &spawn));
    assert(spawn.actor_net_id == 77);
    assert(spawn.base_form_id == 0x1234);

    ev.kind = AHE_UPDATE;
    ev.health_norm = 0.5f;
    ev.anim_state = 9;
    assert(ahe_translate_state(&ev, &state));
    assert(state.health_norm == 0.5f);
    assert(state.anim_state == 9);

    ev.kind = AHE_DESPAWN;
    ev.despawn_reason = ADESPAWN_DEAD;
    assert(ahe_translate_despawn(&ev, &despawn));
    assert(despawn.reason == ADESPAWN_DEAD);

    ev.kind = AHE_DAMAGE;
    ev.attacker_id = 5;
    ev.damage_amount = 10.0f;
    ev.damage_type = DMG_BALLISTIC;
    ev.health_after = 0.25f;
    assert(ahe_translate_damage(&ev, &dmg));
    assert(dmg.attacker_id == 5);
    assert(dmg.health_after == 0.25f);

    ev.kind = AHE_DEATH;
    ev.killer_id = 5;
    ev.ragdoll_seed = 42;
    assert(ahe_translate_death(&ev, &death));
    assert(death.killer_id == 5);
    assert(death.ragdoll_seed == 42);

    return 0;
}
