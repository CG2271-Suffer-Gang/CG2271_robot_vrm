/**
 * This file is largely for purposes of testing the serial comms.
 */

#include "MKL25Z4.h"

#define RED_LED     18  // PortB Pin 18
#define GREEN_LED   19  // PortB Pin 19
#define BLUE_LED    1   // PortD Pin 1
#define EXTRA_PIN 0     // PortD Pin 0
#define MASK(x) (1 << (x))

void initTestGPIO(void) {
    // Enable Clock to PORTB and PORTD
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

    // Configure MUX settings to make all 3 pins GPIO

    PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);

    PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);

    PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

    PORTD->PCR[EXTRA_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[EXTRA_PIN] |= PORT_PCR_MUX(1);

    // Set Data Direction Registers for PortB and PortD
    PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
    PTD->PDDR |= (MASK(BLUE_LED) | MASK(EXTRA_PIN));
}

void offRGB() {
    PTB->PSOR |= (MASK(RED_LED) | MASK(GREEN_LED));
    PTD->PSOR |= (MASK(BLUE_LED) | MASK(EXTRA_PIN));
}

void ledControl(int colour) {
    switch(colour) {
        case RED_LED: 
            PTB->PCOR = MASK(RED_LED);
            break;
        case GREEN_LED:
            PTB->PCOR = MASK(GREEN_LED);
            break;
        case BLUE_LED:
            PTD->PCOR = MASK(BLUE_LED);
            break;
        case EXTRA_PIN:
            PTD->PCOR = MASK(EXTRA_PIN);
            break;
        default:
            offRGB();
    }
}

