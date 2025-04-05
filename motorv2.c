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
#define TURN_COEFF  1

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
		//case 0:
			//PORTD->PCR[LEFT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
			//PORTD->PCR[LEFT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
			//break;
		case 1:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50); // 50% speed
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
			break;
		case SLOW:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 75); // 75% speed
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 75);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 75);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 75);
			break;
		case FAST:
			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100); // 100% speed
			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100);
			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
			break;
	}
}

// void changeRightMotorSpeed() {
// 	switch(speedLevel) {
// 		//case 0:
// 			//PORTD->PCR[LEFT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 			//PORTD->PCR[LEFT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 			//break;
// 		case 1:
// 			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 25); // 25% speed
// 			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 25);
// 			break;
// 		case 2:
// 			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50); // 50% speed
// 			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
// 			break;
// 		case 3:
// 			LEFT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100); // 100% speed
// 			LEFT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
// 			break;
// 	}
// }

// void changeLeftMotorSpeed() {
// 	switch(speedLevel) {
// 		//case 0:
// 			//PORTD->PCR[RIGHT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 			//PORTD->PCR[RIGHT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 			//break;
// 		case 1:
// 			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 25); // 25% speed
// 			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 25);
// 			break;
// 		case 2:
// 			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 50); // 50% speed
// 			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 50);
// 			break;
// 		case 3:
// 			RIGHT_FORWARD = DUTY_CYCLE(MOD_VALUE, 100); // 100% speed
// 			RIGHT_REVERSE = DUTY_CYCLE(MOD_VALUE, 100);
// 			break;
// 	}
// }

// void controlStraightMovement(short newState) {
// 	directionState = newState;
// 	switch(newState) {
// 		case 1:
// 			onPin(LEFT_FORWARD_PIN);
// 			PORTD->PCR[LEFT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 			onPin(RIGHT_FORWARD_PIN);
// 			PORTD->PCR[RIGHT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 		case 0:
// 			PORTD->PCR[LEFT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 			PORTD->PCR[LEFT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 			PORTD->PCR[RIGHT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 			PORTD->PCR[RIGHT_REVERSE_PIN] &= ~PORT_PCR_MUX_MASK;
// 		case -1:
// 			onPin(LEFT_REVERSE_PIN);
// 			PORTD->PCR[LEFT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 			onPin(RIGHT_REVERSE_PIN);
// 			PORTD->PCR[RIGHT_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
// 	}
// }


void controlDirectionMovement() {
	changeMotorSpeed();
	switch(directionState) {
		case FRONTLEFT:
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			LEFT_FORWARD = RIGHT_FORWARD * DIAG_COEFF;
			offRGB();
			ledControl(RED_LED);
			break;
		case FRONT:
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			offRGB();
			break;
		case FRONTRIGHT:
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			RIGHT_FORWARD = LEFT_FORWARD * DIAG_COEFF;
			offRGB();
			ledControl(RED_LED);
			break;
		case LEFT:
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_FORWARD_PIN);
			offPin(RIGHT_REVERSE_PIN);
			LEFT_REVERSE = LEFT_REVERSE * TURN_COEFF;
			RIGHT_FORWARD = RIGHT_FORWARD * TURN_COEFF;
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
			onPin(LEFT_FORWARD_PIN);
			offPin(LEFT_REVERSE_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			LEFT_FORWARD = LEFT_FORWARD * TURN_COEFF;
			RIGHT_REVERSE = RIGHT_REVERSE * TURN_COEFF;
			offRGB();
			break;
		case BACKLEFT:
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			RIGHT_REVERSE = LEFT_REVERSE * DIAG_COEFF;
			offRGB();
			ledControl(GREEN_LED);
			break;
		case BACK:
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			offRGB();
			break;
		case BACKRIGHT:
			onPin(LEFT_REVERSE_PIN);
			offPin(LEFT_FORWARD_PIN);
			onPin(RIGHT_REVERSE_PIN);
			offPin(RIGHT_FORWARD_PIN);
			LEFT_REVERSE = RIGHT_REVERSE * DIAG_COEFF;
			offRGB();
			ledControl(GREEN_LED);
			break;
	}
}
