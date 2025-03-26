#include "constants.h"
#include "globals.h"
#include "ESPCommands.h"

// Bus clock is 12MHz, if not working check bus clock settings
// Using formula (delay in seconds) * (bus clock freq) - 1
#define DELAY_250 2999999
#define DELAY_500 5999999

// Port C pin 3
#define BACK_LEDS 3
#define NUM_FRONT_LEDS 8

// Port C, 8 front leds
int front_leds[NUM_FRONT_LEDS] = {5, 6, 10, 11, 12, 13, 16, 17};
osThreadId_t movingFrontLedThreadId;

void initLEDpins(void) {
    // Enable Clock to PORTC
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // Front Leds
    for (int i = 0; i < NUM_FRONT_LEDS; i++) {
        PORTC->PCR[front_leds[i]] &= ~PORT_PCR_MUX_MASK;
        PORTC->PCR[front_leds[i]] |= PORT_PCR_MUX(1);
        PTC->PDDR |= MASK(front_leds[i]);
        PTC->PCOR |= MASK(front_leds[i]);
    }

    // Back Leds
    PORTC->PCR[BACK_LEDS] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[BACK_LEDS] |= PORT_PCR_MUX(1);
    PTC->PDDR |= MASK(BACK_LEDS);
    PTC->PCOR |= MASK(BACK_LEDS);
}

void initPIT() {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    PIT_MCR = 0;
    PIT_TCTRL0 &= ~(PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);
    PIT_LDVAL0 = DELAY_250;
    PIT_TFLG0 = PIT_TFLG_TIF_MASK;
    NVIC_EnableIRQ(PIT_IRQn);
    PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
    PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;
}

void initLEDs() {
    initLEDpins();
    initPIT();
}

void PIT_IRQHandler() {
    // Clear Pending IRQ
    NVIC_ClearPendingIRQ(PIT_IRQn);

    // toggle back leds
    PTC->PTOR |= MASK(BACK_LEDS);

    // Clear INT Flag
    PIT_TFLG0 = PIT_TFLG_TIF_MASK;
}

void turnOffAllLeds() {
    for (int i = 0; i < NUM_FRONT_LEDS; i++) {
        PTC->PCOR |= MASK(front_leds[i]);
    }
}

void turnOnAllLeds() {
    for (int i = 0; i < NUM_FRONT_LEDS; i++) {
        PTC->PSOR |= MASK(front_leds[i]);
    }
}

void movingFrontLedThread(void *argument) {
    int index = 0;
    while (1) {
        turnOffAllLeds();
        PTC->PSOR |= MASK(front_leds[index]);
        osDelay(100);
        index++;
        if (index >= NUM_FRONT_LEDS) {
            index = 0;
        }
    }
}

void movingLedPattern() {
    turnOffAllLeds();
    osThreadResume(movingFrontLedThreadId);
    PIT_TCTRL0 &= ~(PIT_TCTRL_TEN_MASK);
    PIT_LDVAL0 = DELAY_500;
    PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;
}

void stationaryLedPattern() {
    osThreadSuspend(movingFrontLedThreadId);
    turnOnAllLeds();
    PIT_TCTRL0 &= ~(PIT_TCTRL_TEN_MASK);
    PIT_LDVAL0 = DELAY_250;
    PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;
}

void tLED(void *argument) {
    static uint8_t lastState = STOP;

    while (1) {
        movementState = directionState;
        if (directionState != lastState && (directionState == STOP || lastState == STOP)) {
            if (directionState == STOP) {
                stationaryLedPattern();
            } else {
                movingLedPattern();
            }

            lastState = directionState;
        }
    }
}

