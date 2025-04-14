#include "constants.h"
#include "LED.c"

#include "tBrain.c"
#include "globals.h"
#include "buzzer.c"

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

int main (void) {
    // System Initialization
    SystemCoreClockUpdate();
    initUART2(BAUD_RATE);
    initTestGPIO();
    initLEDs();
		initMotorPWM();
    initBuzzerPWM();

    osKernelInitialize();                 // Initialize CMSIS-RTOS

    // Buzzer Threads
    defaultBuzzerThreadId = osThreadNew(defaultBuzzerThread, NULL, NULL);
    victoryBuzzerThreadId = osThreadNew(victoryBuzzerThread, NULL, NULL);
    osThreadSuspend(victoryBuzzerThreadId);
		buzzerThreadId = osThreadNew(tBuzzer, NULL, NULL);
    
   
    movingFrontLedThreadId = osThreadNew(movingFrontLedThread, NULL, NULL);
    osThreadSuspend(movingFrontLedThreadId);
		turnOnAllLeds();
    osThreadNew(tLED, NULL, NULL);
    osThreadNew(tBrain, NULL, NULL);  //uncomment for tBrain implementation.
    osKernelStart();                      // Start thread execution
		for (;;){}
}

