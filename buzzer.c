#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#define PTB0_Pin 0  // Define PTB0 pin number

// Note frequencies (in Hz) for the C major scale
#define C4  262
#define D4  294
#define E4  330
#define G4  392

// Melody sequence: C E G G G G G G G G G E C E D C D E
const uint16_t notes[] = {C4, E4, G4, G4, G4, G4, G4, G4, G4, G4, G4, E4, C4, E4, D4, C4, D4, E4, 
                            C4, E4, G4, G4, G4, G4, G4, G4, G4, G4, G4, E4, C4, E4, D4, C4, D4, C4};
#define NUM_NOTES (sizeof(notes)/sizeof(notes[0]))

// Rhythm sequence
const uint16_t durations[] = {250, 250, 125, 125, 125, 125, 125, 125, 125, 125, 250, 250, 250, 250, 500, 250, 250, 500, 
                                250, 250, 125, 125, 125, 125, 125, 125, 125, 125, 250, 250, 250, 250, 500, 250, 250, 500};

osMutexId_t pwmMutex;  // Mutex for PWM updates

// Function to initialize PWM on TPM1
void initPWM(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;  // Enable clock to Port B
    PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);  // Set PTB0 to TPM1_CH0

    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;  // Enable clock to TPM1
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  // Use 48MHz clock

    TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));  // Enable TPM1 with prescaler 128

    TPM1->SC &= ~(TPM_SC_CPWMS_MASK);  // Edge-aligned PWM mode

    TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));  // High-true PWM
}

// Function to set a specific note frequency
void setNoteFrequency(uint16_t freq) {
    if (freq == 0) { // Silence 
        TPM1->MOD = 0;
        TPM1_C0V = 0;
        return;
    }

    uint16_t mod_value = (375000 / freq) - 1;
    
    osMutexAcquire(pwmMutex, osWaitForever);
    TPM1->MOD = mod_value;
    TPM1_C0V = mod_value / 2;  // 50% duty cycle
    osMutexRelease(pwmMutex);
}

// RTOS thread for playing notes
void playNotesThread(void *argument) {
    while (1) {
        for (int i = 0; i < NUM_NOTES; i++) {
            setNoteFrequency(notes[i]);  // Set PWM frequency
            osDelay(pdMS_TO_TICKS(durations[i]));  // Play for the note duration

            setNoteFrequency(0);  // Small pause to separate notes
            osDelay(pdMS_TO_TICKS(100));  // Short gap between notes
        }
    }
}

int main(void) {
    SystemCoreClockUpdate();
    initPWM();

    osKernelInitialize(); // Initialize CMSIS-RTOS
    pwmMutex = osMutexNew(NULL);

    osThreadNew(playNotesThread, NULL, NULL); // Create PWM sound thread

    osKernelStart(); // Start RTOS scheduler

    for (;;) {} 
}
