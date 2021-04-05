#include <math.h>
#include "calctime.h"
#include "clock_pine.c"
#include "display.h"
#include "display_print.h"
#include "semihost.h"
#include "touch.h"
#include "systick.h"
#include "geometry.h"
#include "wdt.h"
#include "core.h"
#include "watchface.h"
#include "main_menu.h"

struct process watchface = {
    .runExists = 1,
    .run = &digitalWatch_run,
    .startExists = 1,
    .start = &digitalWatch_init,
    .stopExists = 0,
    .event = &event_always,
};

int drawSegment(int x, int y, int bevelSwitch1, int bevelSwitch2, int width, int height, bool horizontal, uint16_t color)  {
    if (!horizontal) {
        for (int i = 0; i < width; i++) {
            int start, end;
            if (bevelSwitch1 > i)
                start = y + bevelSwitch1 - i;
            else 
                start = y + i - bevelSwitch1;

            if (bevelSwitch2 > i)
                end = y + height - bevelSwitch2 + i;
            else 
                end = y + height + bevelSwitch2 - i;

            drawSquare(x + i, start, x + i, end , color);
        }
    } else {
        for (int i = 0; i < width; i++) {
            int start, end;
            if (bevelSwitch1 > i)
                start = x + bevelSwitch1 - i;
            else 
                start = x + i - bevelSwitch1;

            if (bevelSwitch2 > i)
                end = x + height - bevelSwitch2 + i;
            else 
                end = x + height + bevelSwitch2 - i;

            drawSquare(start, y + i, end, y + i, color);
        }
    }
}

int draw7SegmentNumber(int xSegment, int ySegment, int number, int colorOn, int colorOff) {
//    ─ A
//  F│  │B
//    ─ G
//  E│  │C
//    ─ D

    int numbers[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; //array of 7 segment numbers

    int bevelSwitch1, bevelSwitch2, width, height, x, y;

    int gap = 2;
    width = 6;


// A
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2 - 30 - gap;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 1, numbers[number] & (1 << 0) ? colorOn : colorOff);

// B
    x = xSegment + 30 + gap;
    y = ySegment - 30;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 0, numbers[number] & (1 << 1) ? colorOn : colorOff);

// C
    x = xSegment + 30 + gap;
    y = ySegment + gap;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 0, numbers[number] & (1 << 2) ? colorOn : colorOff);

// D
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2 + 30 + gap;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 1, numbers[number] & (1 << 3) ? colorOn : colorOff);

// E
    x = xSegment;
    y = ySegment + gap;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 0, numbers[number] & (1 << 4) ? colorOn : colorOff);

// F
    x = xSegment;
    y = ySegment - 30;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 0, numbers[number] & (1 << 5) ? colorOn : colorOff);

// G
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2;
    bevelSwitch1 = 3;
    bevelSwitch2 = 3;
    height = 30 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, height, 1, numbers[number] & (1 << 6) ? colorOn : colorOff);
}

int draw14SegmentNumber(int xSegment, int ySegment, uint16_t character, int colorOn, int colorOff) {
//    ───A
//  F│123│B
//    8*4
//  E│765│C
//    ───D


    float bevelSwitch1, bevelSwitch2, width, length, x, y;

    float gap = 1;
    width = 4;

    float charSize = 20;


// A
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2 - charSize - gap;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 1, character & (1 << 0) ? colorOn : colorOff);

// B
    x = xSegment + charSize + gap;
    y = ySegment - charSize;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 0, character & (1 << 1) ? colorOn : colorOff);

// C
    x = xSegment + charSize + gap;
    y = ySegment + gap;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 0, character & (1 << 2) ? colorOn : colorOff);

// D
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2 + charSize + gap;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 1, character & (1 << 3) ? colorOn : colorOff);

// E
    x = xSegment;
    y = ySegment + gap;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 0, character & (1 << 4) ? colorOn : colorOff);

// F
    x = xSegment;
    y = ySegment - charSize;
    bevelSwitch1 = width/2;
    bevelSwitch2 = width/2;
    length = charSize - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 0, character & (1 << 5) ? colorOn : colorOff);

// 8
    x = xSegment + width / 2 + gap;
    y = ySegment - width / 2;
    bevelSwitch1 = width / 2;
    bevelSwitch2 = width / 2;
    length = (charSize - gap) / 2 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 1, (character & (1 << 6)) || (character & (1 << 15)) ? colorOn : colorOff);

// 4
    x = xSegment + width / 2 + gap + ((charSize - gap) / 2 - gap) + gap*2;
    y = ySegment - width / 2;
    bevelSwitch1 = width / 2;
    bevelSwitch2 = width / 2;
    length = (charSize - gap) / 2 - gap;

    drawSegment(x, y, bevelSwitch1, bevelSwitch2, width, length, 1, (character & (1 << 6)) || (character & (1 << 11)) ? colorOn : colorOff);

// 6
    x = xSegment + gap + (charSize - gap) / 2;
    y = ySegment + gap;
    bevelSwitch1 = width / 2;
    //bevelSwitch2 = width / 2;
    length = (charSize - gap) - width / 2;

    int color = character & (1 << 13) ? colorOn : colorOff;
    for (int i = 0; i < width; i++) {
        float start, end;
        if (bevelSwitch1 > i)
            start = y + bevelSwitch1 - i;
        else 
            start = y + i - bevelSwitch1;

        end = y + length;

        drawSquare(x + i, start, x + i, end, color);
    }

// 2
    x = xSegment + gap + (charSize - gap) / 2;
    y = ySegment - charSize + width/2;
    bevelSwitch1 = width / 2;
    //bevelSwitch2 = width / 2;
    length = (charSize - gap) - width / 2;

    color = character & (1 << 9) ? colorOn : colorOff;
    for (int i = 0; i < width; i++) {
        float start, end;
        start = y;
        if (bevelSwitch2 > i)
            end = y + length - bevelSwitch2 + i;
        else 
            end = y + length + bevelSwitch2 - i;

        drawSquare(x + i, start, x + i, end, color);
    }

// 1
    x = xSegment + width + gap;
    y = ySegment - width/2 - gap;
    float vertical = (charSize) - width - gap;
    float horizontal = (charSize + gap)/2 - gap - width - gap;
    float diagonalWidth = width * 1.5;
    
    color = character & (1 << 8) ? colorOn : colorOff;
    vertical -= diagonalWidth;
    for (int i = 0; i < horizontal - 1; i++) {
        float delta = vertical / horizontal;
        drawSquare(x + i, y - vertical - diagonalWidth + i*delta, x + i, y - vertical + i*delta, color);
    }

// 3
    x = xSegment + (charSize) / 2 + width + gap;
    y = ySegment - width/2 - gap;
    vertical = (charSize) - width - gap;
    horizontal = (charSize + gap)/2 - gap - width - gap;
    diagonalWidth = width * 1.5;
    
    color = character & (1 << 10) ? colorOn : colorOff;
    vertical -= diagonalWidth;
    for (int i = 0; i < horizontal - 1; i++) {
        float delta = vertical / horizontal;
        drawSquare(x + horizontal - i, y - vertical - diagonalWidth + i*delta, x + horizontal - i, y - vertical + i*delta, color);
    }

// 7
    x = xSegment + width + gap;
    y = ySegment + width/2 + gap;
    vertical = (charSize) - width - gap;
    horizontal = (charSize + gap)/2 - gap - width - gap;
    diagonalWidth = width * 1.5;
    
    color = character & (1 << 14) ? colorOn : colorOff;
    vertical -= diagonalWidth;
    for (int i = 0; i < horizontal - 1; i++) {
        float delta = vertical / horizontal;
        drawSquare(x + i, y + vertical - i*delta, x + i, y + vertical - i*delta + diagonalWidth, color);
    }

// 5
    x = xSegment + (charSize) / 2 + width + gap;
    y = ySegment + width/2 + gap;
    vertical = (charSize) - width - gap;
    horizontal = (charSize + gap)/2 - gap - width - gap;
    diagonalWidth = width * 1.5;
    
    color = character & (1 << 12) ? colorOn : colorOff;
    vertical -= diagonalWidth;
    for (int i = 0; i < horizontal - 1; i++) {
        float delta = vertical / horizontal;
        drawSquare(x + horizontal - i, y + vertical - i*delta, x + horizontal - i, y + vertical - i*delta + diagonalWidth, color);
    }
}

static uint64_t lastTime;
void digitalWatch_init() {
    drawSquare(0, 0, 239, 239, 0x0000);
    clock_setup();
    lastTime = cpuTime();
}


void digitalWatch_run() {
    static int i = 0;
    if ((cpuTime() - lastTime) > 64000000) {
        lastTime = cpuTime();

        // uint16_t colorOff = 0x1800;
        // uint16_t colorOn = 0xf800;
        uint16_t colorOff = 0x0841;
        uint16_t colorOn = 0x07ff;

        // draw the battery
        int x = 190; int y = 20; int lineWidth = 2; int length = 30; int width = 15; int contactWidth = 5; int holeWidth = 5; int segments = 4; int gap = 1; int segmentGap = 1;

        drawSquare(x + lineWidth, y, x + length - 1, y+lineWidth - 1, colorOn);  // top line
        drawSquare(x + lineWidth, y + width - lineWidth, x + length - 1, y+width - 1, colorOn); // bottom line
        drawSquare(x + lineWidth, y + lineWidth, x + lineWidth*2 - 1, y + width - lineWidth - 1, colorOn); // closing front
        drawSquare(x, y + (width - contactWidth)/2, x + lineWidth - 1, y + (width + contactWidth)/2 - 1, colorOn); // contact
        drawSquare(x + length - lineWidth, y + lineWidth, x + length - 1, y + (width - holeWidth)/2 - 1, colorOn); // back segment 1
        drawSquare(x + length - lineWidth, y + (width + holeWidth)/2, x + length - 1, y + width - lineWidth - 1, colorOn); // back segment 2

        float segmentsWidth = length - lineWidth*5 - gap*2;
        float segmentWidth = (segmentsWidth + segmentGap) / segments;

        int batterylevel = 2;
        for (int i = 0; i < segments; i++) {
            drawSquare(x + lineWidth*3 + gap + i*segmentWidth, y + lineWidth*2, x + lineWidth*3 + ((i+1)*segmentWidth - segmentGap) + gap - 1, y + width - lineWidth*2 - 1, i >= batterylevel? colorOn: colorOff); // contact
        }


        int numbers[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; //array of 7 segment numbers

        uint16_t letters[26] = {0x0077,0x2a0f,0x0039,0x220f,0x0079,0x0071,0x083d,0,0,0x001e,0,0x0038,0x0536,0x1136,0x003f,0x0073,0,0x1073,0x006d,0x2201,0x003e,0x4430,0,0,0x2500,0};

        char* months[12] = {"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};




        long long int time = clock_time();
        int second, minute, hour, day, month, year;

        epochtotime(time, &second, &minute, &hour, &day, &month, &year);

        //char printvalue[11] = "0000000000\n";
        //int division = 1;
        //for (int k = 0; k < 10; k++) {
        //    printvalue[9-k] += (time / (division)) % 10;
        //    division *= 10;
        //}

        //semihost_print(printvalue, 11);

        draw7SegmentNumber(32  - 15, 100, (hour / 10) % 10, colorOn, colorOff);
        draw7SegmentNumber(32  - 15 + 50, 100, hour % 10, colorOn, colorOff);

        draw7SegmentNumber(120 + 15, 100, (minute / 10) % 10, colorOn, colorOff);
        draw7SegmentNumber(120 + 15 + 50, 100, minute % 10, colorOn, colorOff);

        drawCircle(120, 100 + 14, 5, i % 2 ? colorOff : colorOn);
        drawCircle(120, 100 - 14, 5, i % 2 ? colorOff : colorOn);

        i++;

        drawString(187, 140, "AM", colorOff, 0x0000);
        drawString(187 + 8*2, 140, "PM", colorOff, 0x0000);

        int j = 0;
        draw14SegmentNumber(18 + j*30, 185, numbers[(day / 10) %10], colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, numbers[day % 10], colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, 0x40, colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, letters[months[month - 1][0] - 97], colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, letters[months[month - 1][1] - 97], colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, letters[months[month - 1][2] - 97], colorOn, colorOff);
        j++;
        draw14SegmentNumber(18 + j*30, 185, 0, colorOn, colorOff);


        wdt_feed();
    }


    struct touchPoints touchPoint;
    touch_refresh(&touchPoint);
    if (touchPoint.gesture == 0x0C) {
        core_stop_process(&watchface);
        core_start_process(&main_menu);
    }
}


