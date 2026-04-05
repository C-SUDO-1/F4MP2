#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "plugin_transport_client.h"

int main(void) {
    PluginTransportClient c;
    bool applied = false;
    int i;

    memset(&c, 0, sizeof(c));

    if (!ptc_open(&c, "127.0.0.1", 7777)) {
        fprintf(stderr, "ptc_open failed\n");
        return 1;
    }

    if (!ptc_send_hello(&c, 0x12345678u, "itest-guid-0001", "itest-player")) {
        fprintf(stderr, "ptc_send_hello failed\n");
        ptc_close(&c);
        return 2;
    }

    for (i = 0; i < 20; ++i) {
        if (!ptc_poll_once(&c, 250, &applied)) {
            fprintf(stderr, "ptc_poll_once failed at iteration %d\n", i);
            ptc_close(&c);
            return 3;
        }

        printf("poll=%d connected=%d applied=%d session=%u player=%u\n",
            i,
            ptc_connected(&c) ? 1 : 0,
            applied ? 1 : 0,
            ptc_session_id(&c),
            (unsigned)ptc_local_player_id(&c));

        if (ptc_connected(&c)) {
            break;
        }
    }

    if (!ptc_connected(&c)) {
        fprintf(stderr, "client never reached connected state\n");
        ptc_close(&c);
        return 4;
    }

    printf("SUCCESS session=%u player=%u\n",
        ptc_session_id(&c),
        (unsigned)ptc_local_player_id(&c));

    ptc_close(&c);
    return 0;
}