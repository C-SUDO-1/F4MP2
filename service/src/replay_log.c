#include "replay_log.h"
#include <stdio.h>

static FILE* g_log = NULL;

bool replay_log_open(const char* path) {
    g_log = fopen(path, "ab");
    return g_log != NULL;
}

void replay_log_close(void) {
    if (g_log) fclose(g_log);
    g_log = NULL;
}

void replay_log_write(const char* event_type, const char* result, const char* details_json) {
    if (!g_log) return;
    fprintf(g_log, "{\"event\":\"%s\",\"result\":\"%s\",\"details\":%s}\n",
            event_type ? event_type : "", result ? result : "", details_json ? details_json : "{}");
    fflush(g_log);
}
