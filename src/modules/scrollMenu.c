#include "nrf.h"
#include "touch.h"
#include "systick.h"
#include "display.h"
#include "display_print.h"
#include "nrf_delay.h"
#include <stdlib.h>
#include "icons.c"
#include "semihost.h"

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


volatile static int tabY = 0;
volatile static int tabX = 0;

struct menu_item {
    uint8_t *icon;
    char* name;
    uint16_t color;
    int nameLength;
    uint8_t *textBMP;
};

struct menu_item menu_items[13] = {
    {clockDigital,"clock",   0x06fe},
    {termux,   "test2",      0x00f0},
    {trainIcon,"SL",         0xffff},
    {termux,   "test",       0x00f0},
    {termux,   "test",       0x00f0},
    {termux,   "uwu",        0x00f0},
    {termux,   "test",       0x00f0},
    {termux,   "test",       0x00f0},
    {termux,   "test",       0x00f0},
    {distorted,"wow",        0x00f0},
    {termux,   "test",       0x00f0},
    {distorted,"this works", 0x00f0},
    {termux,   "yay",        0x00f0}
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
    static int touchAtStart = 0;
    static int lastEvent = 0;
    static int scrollAtTouchUp = 0;
    static _Bool potentialTab = 0;
    static int counter = 0;
    static int eventCooldown = 0;

    if (reset) {
        touchAtStart = 0;
        lastEvent = 0;
        scrollAtTouchUp = 0;
        potentialTab = 0;
        counter = 0;
        eventCooldown = 0;
        return 0;
    }


    int error;
    do {
        error = touch_refresh(&touchPoint1);
    } while (touchPoint1.touchY == 0 && error == 0);


    static uint64_t debounceStatus;
    static uint64_t cooldown = 0;

    static int XatStart, YatStart, XatTouchUp, YatTouchUp;

    static int noiseReject = 0;

    if (touchPoint1.event != 0) { // if finger touches display
        noiseReject++;
        if (noiseReject > 1) { // to prevent ghost triggers when screen sleeps
            if (debounceStatus == 0) {
                debounceStatus = cpuTime();
                XatStart = touchPoint1.touchX;
                YatStart = touchPoint1.touchY;
                noiseReject = 0;
            }

            tabY = touchPoint1.touchY;
            tabX = touchPoint1.touchX;
            cooldown = cpuTime();
        }

        XatTouchUp = touchPoint1.touchX;
        YatTouchUp = touchPoint1.touchY;
    }


    if (touchPoint1.tab == 1) {
        return -1;
    }
    
    
    int touchY = 240 - touchPoint1.touchY;


    if (lastEvent == 0 && touchPoint1.event != 0) { // touch down
        touchAtStart = touchY;
    }

    if (lastEvent != 0 && touchPoint1.event == 0) { // touch up
        scrollAtTouchUp += touchY - touchAtStart;

    }



    if (touchPoint1.event != 0) {
        if ((scrollAtTouchUp + touchY - touchAtStart) <= lowerBound) { // if lower bound is reached
            scrollAtTouchUp = lowerBound;
            touchAtStart = touchY; // switch these around to change performance
                                   // to leaving off when reached bound

        } else if ((scrollAtTouchUp + touchY - touchAtStart) >= upperBound) {
            scrollAtTouchUp = upperBound;
            touchAtStart = touchY;                                    
        }
    }


    lastEvent = touchPoint1.event;
    

    if (touchPoint1.event != 0) 
        return scrollAtTouchUp + touchY - touchAtStart;
    else
        return scrollAtTouchUp;

}

void drawSelected (int filled, int selectedItem, int scrollPos) {
    int Ytop = (selectedItem*55) % 275 + 20; 
    int Ybottom = (selectedItem*55) % 275 + 48 + 20; 


    for (int i = 0; i < 120; i++) {
        for (int lr = 0; lr < 2; lr++) {
            uint8_t displayColumn[49*2] = {};

            int x;
            if (lr)
                x = 120 + i;
            else 
                x = 119 - i;

            for (int y = 0; y < 48; y++) {
                if (x < 55 && (menu.items[selectedItem].icon[y*7 + x/8] & 1 << (x % 8))) {
                    displayColumn[y*2] = menu.items[selectedItem].color >> 8;
                    displayColumn[y*2 + 1] = menu.items[selectedItem].color & 0xff;

                } else if (y >= 18 && y < 34 && x >= 70 && x <= (70 + 8*menu.items[selectedItem].nameLength - 1) &&
                        menu.items[selectedItem].textBMP[(y-18)*menu.items[selectedItem].nameLength + (x-70)/8] & 1 << ((x-70) % 8)) {

                    displayColumn[y*2] = 0xff;
                    displayColumn[y*2 + 1] = 0xff;

                } else {
                    displayColumn[y*2] = 0x08;
                    displayColumn[y*2 + 1] = 0x41;
                }
            }

            drawBitmap (x, Ytop, x, Ybottom, displayColumn);
            nrf_delay_us(10*i);
        }
    }
}

// drawMenuLine is a function to draw a line of the menu
// lineNr = the scroll menu line Nr
// overWritingLineNr = the line Nr of the line that is currently on this line of display memory (for partial updates)
// screenY = the line of the display memory to write to
void drawMenuLine (int lineNr, int overWritingLineNr, int screenY) {
    static _Bool init = 1;

    if (init) {
        // convert text to bmp
        for (int i = 0; i < menu.length; i++) {
            menu.items[i].nameLength = 0;
            while (menu.items[i].name[menu.items[i].nameLength] != '\0') {
                menu.items[i].nameLength++;
            }
            menu.items[i].textBMP = malloc(16*menu.items[i].nameLength);

            stringBMP (menu.items[i].textBMP, menu.items[i].name, menu.items[i].nameLength);
        }
        init = 0;
    }
    

    if (overWritingLineNr / menu.item_size < menu.length && overWritingLineNr >= 0) {
        if (overWritingLineNr >= 0 && ((overWritingLineNr % menu.item_size) > 17 && (overWritingLineNr % menu.item_size) < 34))
            drawSquare(70, screenY, 70+8*menu.items[overWritingLineNr / menu.item_size].nameLength-1, screenY, 0x0000);
    }



    if (lineNr / menu.item_size < menu.length && lineNr >= 0) {
        // if on a line where text should be drawn
        if ((lineNr % menu.item_size) >= 18 && (lineNr % menu.item_size) < 34) {
            uint16_t startX = 70;
            uint16_t endX = 70+8*menu.items[lineNr / menu.item_size].nameLength-1;
            drawMono(startX, screenY, endX, screenY, menu.items[lineNr / menu.item_size].textBMP + (menu.items[lineNr / menu.item_size].nameLength*(lineNr % menu.item_size - 18)), 0xffff, 0x0000);
        }



        int iconLine = (lineNr % menu.item_size) - menu.icon_top;
        int overWritingIconLine = (overWritingLineNr % menu.item_size) - menu.icon_top;

        if (iconLine < menu.icon_height && iconLine >= 0) {
            uint8_t *icon = menu.items[lineNr / menu.item_size].icon;
            int iconColor = menu.items[lineNr / menu.item_size].color;
            drawMono(0, screenY, menu.icon_width*8-1, screenY, icon + iconLine * menu.icon_width, iconColor, 0x0000);
        } else if (overWritingIconLine < menu.icon_height && overWritingIconLine >= 0){
            drawSquare(0, screenY, menu.icon_width*8-1, screenY, 0x0000);
        }
    } 
}


static int actualScroll = 0;
    
void drawScrollBar( );

int scrollMenu_init () {
    for (int i = 0; i < 220; i++)
        drawMenuLine(i, -1, i+20);
    actualScroll = 0;
    scrollPosition(0,0,1);
}


int drawScrollMenu () {

    int clearance = 20; // see explaination at beginning of file
    int TFA = menu.top; // top fixed area
    int VSA = 220 + clearance;// vertical scrolling area

    static int direction = 1;
    static int actualScroll = 0; // the amount of scrolling that is actually gonna be on the display by the next cycle


    int fingerScroll = scrollPosition(0, menu.length*55 - 220, 0);
    if (fingerScroll != -1) {
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
