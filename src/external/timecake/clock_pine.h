#pragma once
#include <stdbool.h>

extern volatile bool secondPassed;

int clock_setup(void);

long long int clock_time(void);
