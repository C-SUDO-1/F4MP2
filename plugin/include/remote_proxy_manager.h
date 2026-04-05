#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "messages_player.h"
#include "proxy_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

void rpm_init(PlayerId local_player_id);
bool rpm_apply_remote_player_state(const MsgPlayerState* msg);
bool rpm_remove_remote_player(PlayerId player_id);
const ProxyPlayerRecord* rpm_get_remote_player(PlayerId player_id);
uint32_t rpm_rejected_stale_updates(void);
void rpm_reset_rejected_stale_updates(void);

#ifdef __cplusplus
}
#endif
