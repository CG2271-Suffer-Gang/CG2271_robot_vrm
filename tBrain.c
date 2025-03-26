#include "MKL25Z4.h"
#include "UART2fns.h"
#include "globals.h"

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
                case RIGHT:
                case FRONT:
                case BACK:
                case BACKLEFT:
                case FRONTRIGHT:
                case FRONTLEFT:
                case BACKRIGHT:
								case STOP:
                    directionState = data;
										controlDirectionMovement();
                    break;
								case SLOW:
								case FAST:
										speedState = data;
										changeMotorSpeed();
                default:
                    break;
            }
        }
        //osDelay(100);
    }
}

