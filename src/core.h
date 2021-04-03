#pragma once

#define ERROR_ALREADY_RUNNING 1
#define ERROR_NOT_RUNNING 2

struct process {
    void (*start)();
    void (*stop)();
    void (*run)();
};

int core_start_process(struct process *Process);
int core_stop_process(struct process *Process);

void core_run();
