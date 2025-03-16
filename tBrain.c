#include "MKL25Z4.h"
#include "ESPCommands.h"
#include "GPIO.h"
#include "UART2fns.h"

#define BAUD_RATE 9600

static void delay(volatile uint32_t nof) {
    while (nof != 0) {
        __asm("NOP");
        nof--;
    }
}

void tBrain(void* argument) {
    //uint8_t rx_data = 0x69;

    offRGB();
    uint8_t data;
    while (1) {
        //send data
        // UART2_Send(0x19);

        //receive data
        if (!Q_Empty(&rx_q)) {
            data = UART2_Receive();
            switch(data) {
                case LEFT:
                    offRGB();
                    ledControl(RED_LED);
                    break;
                case RIGHT:
                    offRGB();
                    ledControl(GREEN_LED);
                    break;
                case FRONT:
                    offRGB();
                    ledControl(BLUE_LED);
                    break;
                case BACK:
                    offRGB();
                    ledControl(EXTRA_PIN);
                    break;
                case BACKLEFT:
                    offRGB();
                    ledControl(RED_LED);
                    ledControl(EXTRA_PIN);
                    break;
                case FRONTRIGHT:
                    offRGB();
                    ledControl(GREEN_LED);
                    ledControl(BLUE_LED);
                    break;
                case FRONTLEFT:
                    offRGB();
                    ledControl(BLUE_LED);
                    ledControl(RED_LED);
                    break;
                case BACKRIGHT:
                    offRGB();
                    ledControl(EXTRA_PIN);
                    ledControl(GREEN_LED);
                    break;
                default:
                    offRGB();
            }
        }
        //delay(0x80000);
    }
}

