#include "nrf_clock.h"
#include "nrf_timer.h"
#include "steamLocomotive.h"
volatile bool sl_nextFrameReady = 1;

#include "display_print.h"

void TIMER0_IRQHandler(void) {
    sl_nextFrameReady = 1;

    nrf_timer_event_clear(NRF_TIMER0, NRF_TIMER_EVENT_COMPARE0);
    nrf_timer_task_trigger(NRF_TIMER0, NRF_TIMER_TASK_CLEAR);
}


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
        drawString (x, y + i*16, train[i], color_text, color_bg);
    }
}



int sl(int y, uint16_t color_text, uint16_t color_bg) {
    sl_nextFrameReady = 0;
    static int time = 0;

    static bool firstframe = 1;
    if (firstframe) {
        // Irq setup
        NVIC_SetPriority(TIMER0_IRQn, 15); // Lowes priority
        NVIC_ClearPendingIRQ(TIMER0_IRQn);
        NVIC_EnableIRQ(TIMER0_IRQn);


        nrf_timer_mode_set(NRF_TIMER0,NRF_TIMER_MODE_TIMER);
        nrf_timer_bit_width_set(NRF_TIMER0, NRF_TIMER_BIT_WIDTH_32);
        nrf_timer_frequency_set(NRF_TIMER0, NRF_TIMER_FREQ_1MHz);
        nrf_timer_cc_write(NRF_TIMER0, NRF_TIMER_CC_CHANNEL0, 1000000/8);

        nrf_timer_int_enable(NRF_TIMER0,NRF_TIMER_INT_COMPARE0_MASK );
        nrf_timer_task_trigger(NRF_TIMER0, NRF_TIMER_TASK_START);

        firstframe = 0;
    }


    char* trains[6][6] = {{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--/~O========O-+ ",
    " //// \\_/      \\_/   "
    },{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--/O========O\\-+ ",
    " //// \\_/      \\_/   "
    },{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--O========O~\\-+ ",
    " //// \\_/      \\_/   "
    },{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--/~\\------/~\\-+ ",
    " //// 0========0_/   "
    },{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--/~\\------/~\\-+ ",
    " //// \\0========0/   "
    },{
    "      ++      +------ ", 
    "      ||      |+-+ | ",
    "    /---------|| | | ",
    "   + ========  +-+ | ",
    "  _|--/~\\------/~\\-+ ",
    " //// \\_0========0   "
    }};


    for (int line = 0; line < 6; line++) {
        int x = (30-time)*8;
        int charY = y + line*16;
        char* string = trains[(time/2)%6][line];
        _Bool terminated = 0;
        int character = 1;

        while (!terminated) {
            if (string[character] == '\0') {
                terminated = 1;
            } else {
                if (((x + character*8) < 240 && (x + character*8) >= 0) // char out of display bounds
                && ((line > 3 && character > 6) || string[character-1] != string[character])) { // optimization
                    drawChar (x + character*8, charY, string[character], color_text, color_bg, 0);
                }
                character++;
            }
        }
    }
    time++;
    if (time >= (30+22)) {
        time = 0;
        return 1;
    } else {
        return 0;
    }
}
