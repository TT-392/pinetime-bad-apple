#include "nrf.h"
#include "core.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static struct process **runningProcesses;
static int runningProcessCount = 0;
volatile bool event_always = 1;


int core_start_process(struct process *Process) {
    for (int i = 0; i < (runningProcessCount); i++) {
        if (runningProcesses[i] == Process) {
            return ERROR_ALREADY_RUNNING;
        }
    }

    if (Process->startExists)
        Process->start();

    runningProcessCount++;

    struct process **oldProcesses = runningProcesses;
    runningProcesses = (struct process**)malloc((runningProcessCount) * sizeof(struct process*));

    memcpy(runningProcesses, oldProcesses, (runningProcessCount - 1) * sizeof(struct process*));
    runningProcesses[runningProcessCount - 1] = Process;

    free(oldProcesses);
    return 0;
}

int core_stop_process(struct process *Process) {
    bool running = 0;
    for (int i = 0; i < (runningProcessCount); i++) {
        if (runningProcesses[i] == Process) {
            running = 1;
        }
    }

    if (!running) {
        return ERROR_NOT_RUNNING;
    }

    if (Process->stopExists)
        Process->stop();

    runningProcessCount--;

    struct process **oldProcesses = runningProcesses;
    runningProcesses = (struct process**)malloc((runningProcessCount) * sizeof(struct process*));
    
    int l = 0;
    for (int i = 0; i < (runningProcessCount + 1); i++) {
        if (oldProcesses[i] != Process) {
            runningProcesses[l] = oldProcesses[i];
            l++;
        }
    }

    free(oldProcesses);
}

void core_run() {
    event_always = 1;
    bool currentEvents[runningProcessCount];

    __disable_irq();
    for (int i = 0; i < runningProcessCount; i++) {
        currentEvents[i] = *runningProcesses[i]->event;
    }

    for (int i = 0; i < runningProcessCount; i++) {
        *runningProcesses[i]->event = 0;
    }
    __enable_irq();


    for (int i = 0; i < runningProcessCount; i++) {
        if (runningProcesses[i]->runExists && currentEvents[i]) {
            runningProcesses[i]->run();
        }
    }
}
