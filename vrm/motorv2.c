#include "constants.h"                    // Device header
#include "ESPCommands.h"
#include "globals.h"
#include "GPIO.h"

#define LEFT_FORWARD_PIN	0
#define LEFT_REVERSE_PIN	1
#define RIGHT_FORWARD_PIN	2
#define RIGHT_REVERSE_PIN	3
#define MOD_VALUE 			600
#define LEFT_FORWARD 		TPM0_C0V
#define LEFT_REVERSE 		TPM0_C1V
#define RIGHT_FORWARD 		TPM0_C2V
#define RIGHT_REVERSE 		TPM0_C3V
#define DUTY_CYCLE(mod, percent) (mod * percent / 100 - 1)
#define DIAG_COEFF  1 / 10
#define TURN_COEFF  3 / 4
#define NEW_TURN_COEFF  (600 * 50 / 100) - 1

//volatile static uint8_t directionState = 2;	// number pad like direction
//volatile static uint8_t speedLevel = 2;

void onPin(uint8_t pin) {
	PORTD->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[pin] |= PORT_PCR_MUX(4);
	//PTD->PCOR |= MASK(pin);
}

void offPin(uint8_t pin) {
	PORTD->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[pin] |= PORT_PCR_MUX(1);
	PTD->PCOR |= MASK(pin);
}

void highPin(uint8_t pin) {
	PORTD->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[pin] |= PORT_PCR_MUX(1);
	PTD->PSOR |= MASK(pin);
}

void initMotorPWM(void)
{
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTD->PCR[LEFT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
    onPin(LEFT_FORWARD_PIN);

    PORTD->PCR[LEFT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
    onPin(LEFT_REVERSE_PIN);
	
		PORTD->PCR[RIGHT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
    onPin(RIGHT_FORWARD_PIN);

    PORTD->PCR[RIGHT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
    onPin(RIGHT_REVERSE_PIN);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM0->MOD = MOD_VALUE; // aim for 24kHz frequency

    TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(0));	// PS set to 1
    TPM0->SC &= ~(TPM_SC_CPWMS_MASK); // Not center-aligned

    TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); // edge-aligned, high-true pulses
		
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); // edge-aligned, high-true pulses
		
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); // edge-aligned, high-true pulses
		
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); // edge-aligned, high-true pulses
		
	LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50);
	LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
	RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50);
	RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
}

void changeMotorSpeed() {
	switch(speedState) {
		case SLOW:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 80); // 80% speed
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 80);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 80);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 80);
			offRGB();
			ledControl(RED_LED);
			break;
		case FAST:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100); // 100% speed
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
			offRGB();
			break;
	}
}


void controlDirectionMovement() {
	//changeMotorSpeed();
	switch(directionState) {
		case FRONTLEFT:
			changeMotorSpeed();
			offPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			ledControl(GREEN_LED);
			break;
		case FRONT:
			changeMotorSpeed();
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			break;
		case FRONTRIGHT:
			changeMotorSpeed();
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			ledControl(GREEN_LED);
			break;
		case LEFT:
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 80);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 80);
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			break;
		case STOP:
			offPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			break;
		case RIGHT:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 80);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 80);
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offRGB();
			break;
		case BACKRIGHT:
			changeMotorSpeed();
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offRGB();
			ledControl(GREEN_LED);
			break;
		case BACK:
			changeMotorSpeed();
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offRGB();
			break;
		case BACKLEFT:
			changeMotorSpeed();
			offPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offRGB();
			ledControl(GREEN_LED);
			break;
	}
}
