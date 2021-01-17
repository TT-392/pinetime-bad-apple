#include "touch.h"
#include "geometry.h"
#include "display.h"
#include "display_print.h"
#include "semihost.h"
#include <math.h>

char* quarter[4] = {"0123","456","789","*/+-"};


int keyboard() {
    drawSquare(0, 0, 239, 238, 0x0000);

    for (int i = 0; i < 240; i++) {
        drawSquare(i, i, i, i, 0xffff);
        drawSquare(240-i, i, 240-i, i, 0xffff);
    }
    drawCircle(120, 120, 45, 0x9492);

    drawSelection (50, 30, quarter[0], 0xffff, 0x0000, -1, 0);
    drawSelection (240-8, 80, quarter[1], 0xffff, 0x0000, -1, 1);
    drawSelection (30, 220, quarter[2], 0xffff, 0x0000, -1, 0);
    drawSelection (0, 80, quarter[3], 0xffff, 0x0000, -1, 1);

    int currentQuarter = 0;
    int lastQuarter = 0;

    bool inCircle = 0;
    struct touchPoints touchPoint;
    while (1) {
        touch_refresh(&touchPoint);

        int touchX = touchPoint.touchX;
        int touchY = touchPoint.touchY;

        double rX = touchX - 120;
        double rY = touchY - 120;

        ///////////////////////////////////
        //// Calculate finger location ////
        ///////////////////////////////////

        // if in central circle
        if (sqrt(rY*rY + rX*rX) < 45) {
            if (!inCircle) {
                drawCircle(120, 120, 45, 0xffff);
                currentQuarter = 0;
            }

            inCircle = 1;
        } else  {
            drawCircle(120, 120, 45, 0x9492);

            if (touchX > touchY) {
                if (240 - touchX > touchY)
                    currentQuarter = 1;
                else 
                    currentQuarter = 2;
            } else {
                if (240 - touchX > touchY)
                    currentQuarter = 4;
                else 
                    currentQuarter = 3;
            }
            if (inCircle) {
                
            }

            inCircle = 0;
        }

        /////////////////////////////////////

        //drawNumber(20,10,currentQuarter, 0xffff, 0x0000, 1, 0);
        //drawNumber(50,10,value, 0xffff, 0x0000, 1, 0);

        static int value = 0;
        
        static int startQuarter = 0;

        static int location = 0;

        if (currentQuarter != lastQuarter) {
            if (currentQuarter != 0 && lastQuarter != 0) {
                if ((lastQuarter+1 == currentQuarter) || (lastQuarter == 4 && currentQuarter == 1)) {
                    value++;
                }

                if ((lastQuarter-1 == currentQuarter) || (lastQuarter == 1 && currentQuarter == 4)) {
                    value--;
                }
            }
            if (currentQuarter == 0) {
                drawChar (16 + location*8, 0, quarter[startQuarter - 1][value], 0xffff, 0x0000, 0);
                location++;
                value = 0;
                startQuarter = 0;
            }

            if (lastQuarter == 0) {
                startQuarter = currentQuarter;
            }

            drawSelection (50, 30, quarter[0], 0xffff, 0x0000, startQuarter == 1 ? value : -1, 0);
            drawSelection (240-8, 80, quarter[1], 0xffff, 0x0000, startQuarter == 2 ? value : -1, 1);
            drawSelection (30, 220, quarter[2], 0xffff, 0x0000, startQuarter == 3 ? value : -1, 0);
            drawSelection (0, 80, quarter[3], 0xffff, 0x0000, startQuarter == 4 ? value : -1, 1);

        }

        lastQuarter = currentQuarter;




       // if (touchPoint.touchX && touchPoint.touchY)
       //     drawSquare(touchPoint.touchX, touchPoint.touchY, touchPoint.touchX, touchPoint.touchY, 0xffff);
    }
}
