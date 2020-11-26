#include "touch.c"
#include "systick.h"
int randnumber (int seed) {
    int randomNumber = seed * 1103515245 + 12345;
    return (unsigned int)(randomNumber/65536) % 32768;
}

struct menu_items {
    uint8_t *icon;
    char* name;
    uint16_t color;
    int nameLength;
    uint8_t *textBMP;
};

int menu_length = 13;
volatile static int tabY = 0;

struct menu_items menu[13] = {
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


// ┌──────────┐ first 20 lines are reserved for the statusbar
// ├──────────┤ on this line writing happens
// │          │ 220 lines are visible scrolling screen area
// │          │
// ├──────────┤ on this line more writing happens
// │          │ this area (55 lines) scrolls but nothing is written here (for easy math)
// ├──────────┤ 
// └──────────┘ 25 lines are unused


static struct touchPoints touchPoint1;

// Read the touch screen and turn that into a position where the user scrolled to 
int scrollPosition(int lowerBound, int upperBound) {
    static int touchAtStart = 0;
    static int lastEvent = 0;
    static int scrollAtTouchUp = 0;
    static bool potentialTab = 0;
    static int counter = 0;
    static int eventCooldown = 0;


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
            cooldown = cpuTime();
        }

        XatTouchUp = touchPoint1.touchX;
        YatTouchUp = touchPoint1.touchY;
    }

    if (touchPoint1.event == 0) {
        noiseReject = 0;
        if (cpuTime() > cooldown + 6400000) {
            if (cpuTime() < debounceStatus + 12800000) {
                int maxDeviate = 3;
                if (YatStart + maxDeviate > YatTouchUp && YatStart - maxDeviate < YatTouchUp) {
                    if (XatStart + maxDeviate > XatTouchUp && XatStart - maxDeviate < XatTouchUp) {
                        return -1;
                    }
                }
            }

            debounceStatus = 0;
        }
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
                if (x < 55 && (menu[selectedItem].icon[y*7 + x/8] & 1 << (x % 8))) {
                    displayColumn[y*2] = menu[selectedItem].color >> 8;
                    displayColumn[y*2 + 1] = menu[selectedItem].color & 0xff;

                } else if (y >= 18 && y < 34 && x >= 70 && x <= (70 + 8*menu[selectedItem].nameLength - 1) &&
                        menu[selectedItem].textBMP[(y-18)*menu[selectedItem].nameLength + (x-70)/8] & 1 << ((x-70) % 8)) {

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

void drawMenuLine (int lineNr, int overwritingLineNr, int screenY) {
    static bool init = 1;
    static int stringLength = 4;

    if (init) {
        for (int i = 0; i < menu_length; i++) {
            menu[i].nameLength = 0;
            while (menu[i].name[menu[i].nameLength] != '\0') {
                menu[i].nameLength++;
            }
            menu[i].textBMP= malloc(16*menu[i].nameLength);

            stringBMP (menu[i].textBMP, menu[i].name, menu[i].nameLength);
        }
        init = 0;
    }

    if (overwritingLineNr / 55 < menu_length && overwritingLineNr >= 0) {
        if (overwritingLineNr >= 0 && ((overwritingLineNr % 55) > 17 && (overwritingLineNr % 55) < 34))
            drawSquare(70, screenY, 70+8*menu[overwritingLineNr / 55].nameLength-1, screenY, 0x0000);
    }



    if (lineNr / 55 < menu_length && lineNr >= 0) {
        // if on a line where text should be drawn
        if ((lineNr % 55) >= 18 && (lineNr % 55) < 34)
            drawMono(70, screenY, 70+8*menu[lineNr / 55].nameLength-1, screenY, menu[lineNr / 55].textBMP+ (menu[lineNr / 55].nameLength*(lineNr % 55 - 18)), 0xffff, 0x0000);





        if (lineNr % 55 < 49) {
            int logColor = menu[lineNr / 55].color;
            drawMono(0, screenY, 55, screenY, menu[lineNr / 55].icon+(lineNr%55)*7, logColor, 0x0000);
        } else {
            drawSquare(0, screenY, 55, screenY, 0x0000);
        }
    } 
}


int ScrollMenu_init () {

}

int drawScrollMenu () {
    static int scrollPos = 0;
    
    
    int currentScroll = scrollPosition(0, menu_length*55 - 240 + 20);

    if (currentScroll != -1) {
        int direction = 0;
        if (scrollPos < currentScroll) { // scrolling down
            scrollPos ++;


            int screenY = (220 + scrollPos) % 275 + 20;
            int overwritingLineNr = scrollPos - 55;
            drawMenuLine (220 + scrollPos, overwritingLineNr, screenY);

        } if (scrollPos > currentScroll) {// scrolling up
            scrollPos --;
            direction = 2;

            int screenY = (scrollPos) % 275 + 20;
            int overwritingLineNr = 220 + scrollPos + 55;
            drawMenuLine (scrollPos, overwritingLineNr, screenY);
        }


        scroll(20, 275, 25, 20 + (uint16_t)scrollPos % 275);

    } else {
        int selectedItem = ((tabY - 20) + scrollPos) / 55;

        drawSelected(100, selectedItem, scrollPos);
        return selectedItem;
    }
    return -1;
}
