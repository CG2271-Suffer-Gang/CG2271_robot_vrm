#include "constants.h"

#define FCLK 48 * 10^6
#define PRESCALAR 3 // sucks that this is hardcoded but ok
#define MOD(x) (FCLK / (PRESCALAR * x)) - 1 // x is Fpwm
#define DUTY_CYCLE(a, b) int((a * b) / 100) // b is % duty

void initTimers() {
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // 8kHz according to mod formula as a macro
    uint32_t mod = MOD(8000);
    TPM1->MOD = mod;
    TPM2->MOD = mod;

    // Timer enable and prescalar
    TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(PRESCALAR));
    TPM1->SC &= ~(TPM_SC_CPWMS_MASK);

    TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(PRESCALAR));
    TPM2->SC &= ~(TPM_SC_CPWMS_MASK);

    // Timer settings, duty cycle 0% on init
    TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

    TPM1_C0V = DUTY_CYCLE(mod, 0);
    TPM1_C1V = DUTY_CYCLE(mod, 0);

    TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

    TPM2_C0V = DUTY_CYCLE(mod, 0);
    TPM2_C1V = DUTY_CYCLE(mod, 0);
}

void initPWMpins() {
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Port B0-B3
    for (int i = 0; i < 4; i++) {
        PORTB->PCR[i] &= ~PORT_PCR_MUX_MASK;
        PORTB->PCR[i] |= PORT_PCR_MUX(3);
    }
}

void initMotors() {
    initPWMpins();
    initTimers();
}

void forward() {
    TPM1_C0V = DUTY_CYCLE(mod, 50);
    TPM1_C1V = DUTY_CYCLE(mod, 0);
    TPM2_C0V = DUTY_CYCLE(mod, 50);
    TPM2_C1V = DUTY_CYCLE(mod, 0);
}

void reverse() {
    TPM1_C0V = DUTY_CYCLE(mod, 0);
    TPM1_C1V = DUTY_CYCLE(mod, 50);
    TPM2_C0V = DUTY_CYCLE(mod, 0);
    TPM2_C1V = DUTY_CYCLE(mod, 50);
}

void left() {
    TPM1_C0V = DUTY_CYCLE(mod, 50);
    TPM1_C1V = DUTY_CYCLE(mod, 0);
    TPM2_C0V = DUTY_CYCLE(mod, 0);
    TPM2_C1V = DUTY_CYCLE(mod, 50);
}

void right() {
    TPM1_C0V = DUTY_CYCLE(mod, 0);
    TPM1_C1V = DUTY_CYCLE(mod, 50);
    TPM2_C0V = DUTY_CYCLE(mod, 50);
    TPM2_C1V = DUTY_CYCLE(mod, 0);
}

void stop() {
    TPM1_C0V = DUTY_CYCLE(mod, 0);
    TPM1_C1V = DUTY_CYCLE(mod, 0);
    TPM2_C0V = DUTY_CYCLE(mod, 0);
    TPM2_C1V = DUTY_CYCLE(mod, 0);
}

void tMotors(void* argument) {
    while (1) {
        forward();
        osDelay(500);
        stop();
        osDelay(500);

        reverse();
        osDelay(500);
        stop();
        osDelay(500);

        left();
        osDelay(500);
        stop();
        osDelay(500);

        right();
        osDelay(500);
        stop();
        osDelay(500);
    }
}

