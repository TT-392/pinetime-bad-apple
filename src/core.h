#pragma once
#include <stdbool.h> 

#define ERROR_ALREADY_RUNNING 1
#define ERROR_NOT_RUNNING 2

extern volatile bool event_always;

struct process {
    bool startExists;
    void (*start)();
    bool stopExists;
    void (*stop)();
    bool runExists;
    void (*run)();
    volatile bool *event;
};

int core_start_process(struct process *Process);
int core_stop_process(struct process *Process);

void core_run();
