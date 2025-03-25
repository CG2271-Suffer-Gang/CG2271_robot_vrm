#include "MKL25Z4.h"                    // Device header

#define LEFT1_Pin	0
#define LEFT2_Pin	1
#define RIGHT1_Pin	2
#define RIGHT2_Pin	3

volatile static short forwardState = 1;	// 1 for forward, 0 for stop, -1 for reverse
// volatile static short directionState = 1; // 1 for right, 0 for straight, -1 for left
volatile static short speedLevel = 2;

void initMotorPWM(void)
{
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[LEFT1_Pin] |= PORT_PCR_MUX(3);

    PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[LEFT2_Pin] |= PORT_PCR_MUX(3);
	
	PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[RIGHT1_Pin] |= PORT_PCR_MUX(3);

    PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[RIGHT2_Pin] |= PORT_PCR_MUX(3);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM0->MOD = 1999; // aim for 24kHz frequency

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
		
	TPM0_C0V = 1000;
	TPM0_C1V = 1000;
	TPM0_C2V = 1000;
	TPM0_C3V = 1000;
}

void changeMotorSpeed() {
	switch(speedLevel) {
		//case 0:
			//PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
			//PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
			//break;
		case 1:
			TPM0_C0V = 499; // 25% speed
			TPM0_C1V = 499;
			TPM0_C2V = 499;
			TPM0_C3V = 499;
			break;
		case 2:
			TPM0_C0V = 999; // 50% speed
			TPM0_C1V = 999;
			TPM0_C2V = 999;
			TPM0_C3V = 999;
			break;
		case 3:
			TPM0_C0V = 1999; // 100% speed
			TPM0_C1V = 1999;
			TPM0_C2V = 1999;
			TPM0_C3V = 1999;
			break;
	}
}

void changeRightMotorSpeed() {
	switch(speedLevel) {
		//case 0:
			//PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
			//PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
			//break;
		case 1:
			TPM0_C0V = 499; // 25% speed
			TPM0_C1V = 499;
			break;
		case 2:
			TPM0_C0V = 999; // 50% speed
			TPM0_C1V = 999;
			break;
		case 3:
			TPM0_C0V = 1999; // 100% speed
			TPM0_C1V = 1999;
			break;
	}
}

void changeLeftMotorSpeed() {
	switch(speedLevel) {
		//case 0:
			//PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
			//PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
			//break;
		case 1:
			TPM0_C2V = 499; // 25% speed
			TPM0_C3V = 499;
			break;
		case 2:
			TPM0_C2V = 999; // 50% speed
			TPM0_C3V = 999;
			break;
		case 3:
			TPM0_C2V = 1999; // 100% speed
			TPM0_C3V = 1999;
			break;
	}
}

void controlStraightMovement(short newState) {
	forwardState = newState;
	switch(newState) {
		case 1:
			PORTD->PCR[LEFT1_Pin] |= PORT_PCR_MUX(3);
			PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
			PORTD->PCR[RIGHT1_Pin] |= PORT_PCR_MUX(3);
			PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
		case 0:
			PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
			PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
			PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
			PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
		case -1:
			PORTD->PCR[LEFT2_Pin] |= PORT_PCR_MUX(3);
			PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
			PORTD->PCR[RIGHT2_Pin] |= PORT_PCR_MUX(3);
			PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
	}
}

void controlDirectionMovement(short newState) {
	changeMotorSpeed();
	switch(forwardState) {
		case 1:
			if (newState == 1) {
				PORTD->PCR[LEFT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
				TPM0_C0V = TPM0_C2V / 2;
			}	else if (newState == -1) {
				PORTD->PCR[LEFT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
				TPM0_C2V = TPM0_C0V / 2;
			}			
		case 0:
			if (newState == 1) {
				PORTD->PCR[LEFT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT2_Pin] &= ~PORT_PCR_MUX_MASK;
			} else if (newState == -1) {
				PORTD->PCR[LEFT1_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT2_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
			}
		case -1:
			if (newState == 1) {
				PORTD->PCR[LEFT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
				TPM0_C3V = TPM0_C1V / 2;
			} else if (newState == -1) {
				PORTD->PCR[LEFT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[LEFT1_Pin] &= ~PORT_PCR_MUX_MASK;
				PORTD->PCR[RIGHT2_Pin] |= PORT_PCR_MUX(3);
				PORTD->PCR[RIGHT1_Pin] &= ~PORT_PCR_MUX_MASK;
				TPM0_C1V = TPM0_C3V / 2;
			}
	}
}

int main(){
	initMotorPWM();
}