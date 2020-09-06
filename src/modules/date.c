#include "../display_print.h"
#include "../calctime.c"
#include "../timecake/clock_pine.c"

void date_init() {
    clock_setup();
}

void itos (int integer, _Bool removeZeros, char* String, int strLength)  {
    int powerOfTen = 1;

    for (int i = 0; i < strLength - 1; i++) {
        String[(strLength - 2)-i] = (integer / powerOfTen) % 10 + 48;
        powerOfTen *= 10;
    }

    String[strLength - 1] = '\0';
}

// kinda shitty implementation of the unix command, can only return a string of up to 100 characters and will use at least 100 bytes of ram, missing a bunch of stuff
void drawDate (char* input) {
    long long int time = clock_time();
    int second, minute, hour, day, month, year;

    epochtotime(time, &second, &minute, &hour, &day, &month, &year);

    char tempString[100];

    bool terminated = 0;

    int i = 0;
    int newStrLeng = 0;

    while (!terminated) {
        if (input[i] == '\0' || i >= 100) {
            terminated = 1;
        } else {
            if (input[i] != '%') {
                tempString[newStrLeng] = input[i];
                newStrLeng++;
            } else {
                if (input[i+1] == '%') { // literal %
                    tempString[newStrLeng] = '%';
                    newStrLeng++;
                } else if (input[i+1] == 'Y') { // year
                    char String[5];
                    itos(2020, 0, String, 5);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                    tempString[newStrLeng++] = String[2];
                    tempString[newStrLeng++] = String[3];
                } else if (input[i+1] == 'm') { // month
                    char String[3];
                    itos(month, 0, String, 3);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                } else if (input[i+1] == 'd') { // day
                    char String[3];
                    itos(day, 0, String, 3);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                } else if (input[i+1] == 'H') { // hour
                    char String[3];
                    itos(hour, 0, String, 3);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                } else if (input[i+1] == 'M') { // minute
                    char String[3];
                    itos(minute, 0, String, 3);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                } else if (input[i+1] == 'S') { // second
                    char String[3];
                    itos(second, 0, String, 3);

                    tempString[newStrLeng++] = String[0];
                    tempString[newStrLeng++] = String[1];
                }
                i++;
            }
        }
        i++;
    }
    tempString[newStrLeng] = '\0';
    
    

    drawString (30, 70, tempString, 0xffff, 0x0000);
}
