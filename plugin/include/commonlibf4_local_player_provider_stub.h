#pragma once

#include <stdbool.h>
#include "local_player_observer.h"

#ifdef __cplusplus
extern "C" {
#endif

void clf4_lpo_stub_install(PlayerId local_player_id);
void clf4_lpo_stub_uninstall(void);
void clf4_lpo_stub_set_available(bool available);
void clf4_lpo_stub_feed(const LocalPlayerObservedState* state);
bool clf4_lpo_stub_has_state(void);
const LocalPlayerObservedState* clf4_lpo_stub_get_last_state(void);

#ifdef __cplusplus
}
#endif
