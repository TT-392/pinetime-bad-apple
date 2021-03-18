#include "nrf.h"
#include "touch.h"
#include "systick.h"
#include "display.h"
#include "display_print.h"
#include "nrf_delay.h"
#include <stdlib.h>
#include <math.h>
#include "icons.c"
#include "semihost.h"
#include "uart.h"

#define SCREENTAB -1

int randnumber (int seed) {
    int randomNumber = seed * 1103515245 + 12345;
    return (unsigned int)(randomNumber/65536) % 32768;
}

// ┌──────────┐ 
// │          │ lines that don't move (used for status bar)
// ├──────────┤ menu.top
// │          │ actual visible menu
// │          │
// ├──────────┤ menu.bottom (should be the last line of the display, 239)
// ├──────────┤ menu.bottom + 1 this is where writing happens (writeU)
// ├──────────┤ menu.bottom + 20 this is where writing happens (writeD)
// │          │ lines that don't scroll (probably off screen)
// └──────────┘
// the lines from menu.top to menu.bottom + 1 scroll; therefore memory coordinates
// are mapped to shifted screen coordinates, this causes writeY to change
//
// there are 20 lines that are scrolling but not visible, this is because scroll
// commands lagg behind display write commands on the screen causing problems at
// higher speed (these 20 lines are referred to as clearance in code)

int clearance = 20;

volatile static int tabY = 0;
volatile static int tabX = 0;

struct menuBMP { // (x2 - x1 + 1) % 8 should always be 0
    int x1;
    int y1;
    int x2;
    int y2;
    uint16_t color;
    char* BMP;
};

struct menu_item {
    char* name;
    int elements;
    struct menuBMP element[5];
};

struct menu_item menu_items[13] = { // first element reserved for text
    {"clock",      2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x06fe, clockDigital}}},
    {"test2",      2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"SL",         2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0xffff, trainIcon,  }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"uwu",        2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"wow",        2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, distorted,  }}},
    {"test",       2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}},
    {"this works", 2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, distorted,  }}},
    {"yay",        2, {{70, 28, 0, 0, 0xffff},{0, 12, 55, 60, 0x00f0, termux,     }}}
};

struct menu_properties {
    uint16_t top;
    uint16_t bottom;
    int length;
    struct menu_item* items;
    int item_size;
    int icon_top;
    int icon_height;
    int icon_width; // in bytes
};

struct menu_properties menu = {
    .top = 20,
    .bottom = 238,
    .length = 13,
    .items = menu_items,
    .item_size = 73,
    .icon_top = 0,
    .icon_height = 49,
    .icon_width = 7
};


static struct touchPoints touchPoint1;

//// Read the touch screen and turn that into a position where the user scrolled to 
//int scrollPosition(int lowerBound, int upperBound, bool reset) {
//    
//}
// Read the touch screen and turn that into a position where the user scrolled to 
int scrollPosition(int lowerBound, int upperBound, bool reset) {
    static double scroll = 0;
    if (reset) {
        scroll = 0;
    }

    int error;
    error = touch_refresh(&touchPoint1);

    if (touchPoint1.touchY == 0) // sometimes the touch controller incorrectly returns 0 0 as coordinate
        error = 1;


    if (!error) {
        if (touchPoint1.New) {
            if (touchPoint1.tab == 1) {
                tabY = touchPoint1.touchY;
                return SCREENTAB;
            }
        }

    }

    static int lastStatus = 0;
    static double scrollSpeed = 0;
    if (touchPoint1.New && !error && touchPoint1.fingerStatus == 1) {
        static int lastTouchY = 0;
        static uint64_t lastTime = 0;
        uint64_t sampleTime = cpuTime() - lastTime;

        if (lastStatus == 1) {
            scroll += lastTouchY - touchPoint1.touchY;
            //if ((lastTouchY - touchPoint1.touchY) > 0)

            if (lastTouchY - touchPoint1.touchY) {
                scrollSpeed = (lastTouchY - touchPoint1.touchY) / (double)sampleTime;
            }
        }
        lastTouchY = touchPoint1.touchY;

        lastTime = cpuTime();
    } else if (touchPoint1.fingerStatus != 1) {
        static uint64_t lastTime = 0;
        uint64_t sampleTime = cpuTime() - lastTime;

        //uart_send_number(scrollSpeed*100000);

        //scroll += scrollSpeed * (double)sampleTime;//(double)sampleTime;
        if (scrollSpeed > 0) {
            scrollSpeed -= 1;
            if (scrollSpeed < 0)
                scrollSpeed = 0;
        } else if (scrollSpeed < 0) {
            scrollSpeed += 1;
            if (scrollSpeed > 0)
                scrollSpeed = 0;
        }
            

        lastTime = cpuTime();
    }


    lastStatus = touchPoint1.fingerStatus;

    if (scroll <= lowerBound) {
        scroll = lowerBound;
    } else if (scroll >= upperBound) {
        scroll = upperBound;
    }

    if (scroll >= 0)
        return scroll;
    else
        return 0;
}

void drawSelected (int filled, int selectedItem, int scrollPos) {
    int TFA = menu.top; // top fixed area
    int VSA = 220 + clearance;// vertical scrolling area

    struct menu_item menuItem = menu.items[selectedItem];


    // expanding out from the middle
    for (int i = 0; i < 120; i++) {
        for (int leftRight = 0; leftRight < 2; leftRight++) {
            int columnX; 
            if (leftRight)
                columnX = 120 + i; // right
            else 
                columnX = 119 - i; // left

            uint8_t column[menu.item_size*2]; // 2 bytes per pixel

            for (int y = 0; y < menu.item_size; y++) {
                uint16_t color = 0x0841;
                for (int element = 0; element < menuItem.elements; element++) {
                    struct menuBMP bmp = menuItem.element[element];

                    if (columnX >= bmp.x1 && columnX <= bmp.x2) {
                        if (y >= bmp.y1 && y <= bmp.y2) {
                            int bmpOffset = (y - bmp.y1) * ((bmp.x2 - bmp.x1 + 1)/8) + ((columnX-bmp.x1)/8);
                            if (bmp.BMP[bmpOffset] & 1 << ((columnX-bmp.x1) % 8)) {
                                color = bmp.color;
                            }
                        }
                    }
                }
                column[y*2] = color >> 8;
                column[y*2 + 1] = color & 0xff;
            }

            int Ytop = (selectedItem * menu.item_size) % VSA;
            int Ybottom = (selectedItem * menu.item_size) % VSA + menu.item_size; 

            if (Ybottom <= VSA) 
                drawBitmap (columnX, Ytop + menu.top, columnX, Ybottom + menu.top, column);
            else {
                drawBitmap (columnX, Ytop + menu.top, columnX, VSA + menu.top, column);
                drawBitmap (columnX, menu.top, columnX, Ybottom % VSA + menu.top, column + 2 * (VSA - Ytop));
            }



            nrf_delay_us(10*i);
        }
    }
}

// drawMenuLine is a function to draw a line of the menu
// lineNr = the scroll menu line Nr
// overWritingLineNr = the line Nr of the line that is currently on this line of display memory (for partial updates)
// screenY = the line of the display memory to write to
void drawMenuLine (int lineNr, int overWritingLineNr, int screenY) {
    struct menu_item menuItem = menu.items[lineNr / menu.item_size];
    struct menu_item overWritingMenuItem = menu.items[overWritingLineNr / menu.item_size];

    // get rid of old stuff
    for (int i = 0; i < overWritingMenuItem.elements; i++) {
        struct menuBMP bmp = overWritingMenuItem.element[i];
        int overWritingItemY = overWritingLineNr % menu.item_size;
        int itemY = lineNr % menu.item_size;

        bool overWrite = 0;
        // if there used to be stuff on this line
        if (overWritingItemY >= bmp.y1 && overWritingItemY <= bmp.y2) {
            for (int i = 0; i < menuItem.elements; i++) {
                struct menuBMP newBMP = menuItem.element[i];

                // if there is going to be stuff on this line
                if (itemY >= newBMP.y1 && itemY <= newBMP.y2) {
                    // if this item is not on the same location or of the same size
                    if (newBMP.x1 != bmp.x1 || newBMP.x2 != bmp.x2) {
                        overWrite = 1;
                    }
                } else {
                    overWrite = 1;
                }
            }
        }
        if (overWrite) 
            drawSquare(bmp.x1, screenY, bmp.x2, screenY, 0x0000);
    }

    // write new pixels
    for (int i = 0; i < menuItem.elements; i++) {
        int itemY = lineNr % menu.item_size;

        struct menuBMP bmp = menuItem.element[i];
        if (itemY >= bmp.y1 && itemY <= bmp.y2) {
            int bmpOffset = (itemY - bmp.y1) * ((bmp.x2 - bmp.x1 + 1)/8);
            drawMono(bmp.x1, screenY, bmp.x2, screenY, bmp.BMP + bmpOffset, bmp.color, 0x0000);
        }
    }
}

static int actualScroll = 0; // the amount of scrolling that is actually gonna be on the display by the next cycle

int scrollMenu_init () {
    // convert text to bmp
    for (int i = 0; i < menu.length; i++) {
        int textLength = 0;
        while (menu.items[i].name[textLength] != '\0') {
            textLength++;
        }
        menu.items[i].element[0].BMP = malloc(16*textLength);

        stringBMP(menu.items[i].element[0].BMP, menu.items[i].name, textLength);

        menu.items[i].element[0].y2 = menu.items[i].element[0].y1 + 16 - 1;
        menu.items[i].element[0].x2 = menu.items[i].element[0].x1 + 8 * textLength - 1;

    }

    for (int i = 0; i < 220; i++)
        drawMenuLine(i, -1, i+20);

    actualScroll = 0;
    touch_init();
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
    scrollPosition(0,0,1);
}



int drawScrollMenu () {
    int TFA = menu.top; // top fixed area
    int VSA = 220 + clearance;// vertical scrolling area

    static int direction = 1;


    int fingerScroll = scrollPosition(0, menu.length*55 - 220, 0);
    if (fingerScroll != SCREENTAB) {
        if (fingerScroll > actualScroll)
            direction = 1;
        else if (fingerScroll < actualScroll)
            direction = -1;
        else
            direction = 0;

        actualScroll+= direction;

        int SSA = (actualScroll % VSA) + menu.top; // scroll start address

        if (direction == 1) { // scrolling up (finger going up)
            int writeU = (actualScroll + VSA-1-clearance) % VSA + menu.top;
            int lineNr = actualScroll + VSA-1-clearance;
            int overWritingLineNr = lineNr - VSA;

            drawMenuLine (lineNr, overWritingLineNr, writeU);
        }

        if (direction == -1) { // scrolling down
            int writeD = SSA;
            int lineNr = actualScroll;
            int overWritingLineNr = lineNr + VSA;

            drawMenuLine (lineNr, overWritingLineNr, writeD);
        }

        display_scroll(TFA, VSA, 320 - (TFA + VSA), SSA);
    } else {
        //int memY = 20 + ((tabY - 20 + actualScroll) % VSA);

        int selectedItem = ((tabY - 20) + actualScroll) / menu.item_size;
        drawSelected(100, selectedItem, actualScroll);
        return selectedItem;
    }
    return -1;
}
