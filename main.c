#include "constants.h"
#include "LED.c"
#include "tBrain.c"

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
    for (;;) {}
}

int main (void) {
    // System Initialization
    SystemCoreClockUpdate();
    initUART2(BAUD_RATE);
    initTestGPIO();
    initLEDs();
    // ...

    osKernelInitialize();                 // Initialize CMSIS-RTOS
    osThreadNew(app_main, NULL, NULL);    // Create application main thread

    movingFrontLedThreadId = osThreadNew(movingFrontLedThread, NULL, NULL);
    osThreadSuspend(movingFrontLedThreadId);
    osThreadNew(tLED, NULL, NULL);
    osThreadNew(tBrain, NULL, NULL);

    osKernelStart();                      // Start thread execution
    for (;;) {}
}

