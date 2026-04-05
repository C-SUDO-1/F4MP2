#include "test_service_process.h"
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#ifndef F4MP_SERVICE_BINARY_PATH
#define F4MP_SERVICE_BINARY_PATH "./fo4_coop_service"
#endif
const char* f4mp_service_binary_path(void) { return F4MP_SERVICE_BINARY_PATH; }
bool f4mp_spawn_service(F4mpServiceProcess* proc, const char* port) {
    pid_t pid; int status = 0;
    if (proc == NULL || port == NULL || port[0] == '\0') return false;
    proc->pid = -1;
    pid = fork();
    if (pid < 0) { perror("fork fo4_coop_service"); return false; }
    if (pid == 0) {
        execl(F4MP_SERVICE_BINARY_PATH, F4MP_SERVICE_BINARY_PATH, port, (char*)NULL);
        perror(F4MP_SERVICE_BINARY_PATH);
        _exit(127);
    }
    proc->pid = pid;
    usleep(50000);
    if (waitpid(pid, &status, WNOHANG) == pid) {
        fprintf(stderr, "fo4_coop_service exited early while starting on port %s (status=%d, path=%s)\n", port, status, F4MP_SERVICE_BINARY_PATH);
        proc->pid = -1;
        return false;
    }
    return true;
}
void f4mp_stop_service(F4mpServiceProcess* proc) {
    if (proc == NULL || proc->pid <= 0) return;
    kill(proc->pid, SIGTERM);
    waitpid(proc->pid, NULL, 0);
    proc->pid = -1;
}
