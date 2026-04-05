#include <assert.h>
#include <stdio.h>
#include "workshop_event_translation.h"

int main(void) {
    WorkshopHostEvent ev = {0};
    MsgObjectSpawn s = {0};
    MsgObjectUpdate u = {0};
    MsgObjectDespawn d = {0};

    ev.kind = WHE_PLACE;
    ev.object_net_id = 55;
    ev.form_id = 0xAAAAAAAAu;
    ev.position.x = 1.0f;
    assert(whe_translate_spawn(&ev, &s));
    assert(s.object_net_id == 55);

    ev.kind = WHE_MOVE;
    ev.position.y = 2.0f;
    assert(whe_translate_update(&ev, &u));
    assert((u.fields_mask & OUF_POSITION) != 0);

    ev.kind = WHE_SCRAP;
    assert(whe_translate_despawn(&ev, &d));
    assert(d.reason == ODESPAWN_SCRAPPED);

    puts("workshop_event_translation_smoke_test: ok");
    return 0;
}
