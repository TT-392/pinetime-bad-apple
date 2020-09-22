#include "../display_print.h"

void sl_static(int x, int y, uint16_t color_text, uint16_t color_bg) {
    char* train[6] = {
    "      ++      +------", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--O========O~\\-+ ",
    " //// \\_/      \\_/   "
    };


    
    for (int i = 0; i < 6; i++) {
        drawString (40, 90 + i*16, train[i], color_text, color_bg);
    }
}
