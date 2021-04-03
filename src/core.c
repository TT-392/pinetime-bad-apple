#include "core.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static struct process **runningProcesses;
static int runningProcessCount = 0;

int core_start_process(struct process *Process) {
    for (int i = 0; i < (runningProcessCount); i++) {
        if (runningProcesses[i] == Process) {
            return ERROR_ALREADY_RUNNING;
        }
    }

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
    for (int i = 0; i < runningProcessCount; i++) {
        runningProcesses[i]->run();
    }
}
