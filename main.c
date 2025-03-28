#include "constants.h"
#include "LED.c"

#include "tBrain.c"
#include "globals.h"

#define BAUD_RATE 9600

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
    // ...
    for (;;) {
				directionState = FRONT;
				controlDirectionMovement();
				osDelay(1000);
				directionState = STOP;
				controlDirectionMovement();
				osDelay(1000);
				directionState = BACK;
				controlDirectionMovement();
				osDelay(1000);
				directionState = STOP;
				controlDirectionMovement();
				osDelay(1000);
		}
}

int main (void) {
    // System Initialization
    SystemCoreClockUpdate();
    initUART2(BAUD_RATE);
    initTestGPIO();
    initLEDs();
		initMotorPWM();
    initBuzzerPWM();
		
    // ...

    osKernelInitialize();                 // Initialize CMSIS-RTOS
    //osThreadNew(app_main, NULL, NULL);    // Create application main thread

    buzzerThreadId = osThreadNew(tBuzzer, NULL, NULL);

    movingFrontLedThreadId = osThreadNew(movingFrontLedThread, NULL, NULL);
    osThreadSuspend(movingFrontLedThreadId);
    osThreadNew(tLED, NULL, NULL);
    //osThreadNew(tBrain, NULL, NULL);  //uncomment for tBrain implementation.
    //osThreadNew(tMotors, NULL, NULL);
    osKernelStart();                      // Start thread execution
		for (;;){}
}

