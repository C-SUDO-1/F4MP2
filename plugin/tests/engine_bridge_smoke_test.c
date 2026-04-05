#include <assert.h>
#include <string.h>

#include "engine_bridge_stub.h"
#include "proxy_runtime.h"
#include "messages_ui.h"

int main(void) {
    ProxyPlayerSpawnSpec ps = {0};
    ProxyActorSpawnSpec as = {0};
    MsgDialogueOpen dlg = {0};
    MsgDialogueLine line = {0};
    EngineBridgePlayerSnapshot pshot;
    EngineBridgeActorSnapshot ashot;

    ebstub_init();

    ps.player_id = 1;
    ps.proxy_net_id = 1001;
    ps.position.x = 10.0f;
    ps.equipped_form_id = 0x1234u;
    assert(proxy_spawn_player(&ps));

    as.actor_net_id = 1;
    as.base_form_id = 0xDEADu;
    as.position.y = 3.0f;
    assert(proxy_spawn_actor(&as));

    assert(ebstub_player_count() == 1);
    assert(ebstub_actor_count() == 1);
    assert(ebstub_get_player_snapshot(0, &pshot));
    assert(pshot.player_id == 1);
    assert(pshot.equipped_form_id == 0x1234u);
    assert(ebstub_get_actor_snapshot(0, &ashot));
    assert(ashot.actor_net_id == 1);
    assert(!ashot.is_dead);

    dlg.dialogue_id = 500;
    dlg.npc_actor_net_id = 1;
    dlg.speaker_actor_net_id = 1;
    proxy_set_dialogue_open(&dlg);
    line.dialogue_id = 500;
    line.speaker_actor_net_id = 1;
    proxy_set_dialogue_line(&line, "Hello there");
    assert(ebstub_has_active_dialogue());
    assert(strcmp(ebstub_get_dialogue()->current_line, "Hello there") == 0);

    return 0;
}
