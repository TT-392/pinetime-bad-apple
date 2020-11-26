#pragma once

unsigned long timetoepoch(int year, int month, int day, int hour, int minute, int second);

void epochtotime(unsigned long timeSinceEpoch, int* second, int* minute, int* hour, int* day, int* month, int* year);
