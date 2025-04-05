#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#define PTB0_Pin 0  // Define PTB0 pin number

// Note frequencies (in Hz)
#define C4  262
#define Cs4 277
#define D4  294
#define Ds4 311
#define E4  330
#define F4  349
#define Fs4 370
#define G4  392
#define Gs4 415
#define A4  440
#define As4 466
#define B4  494

#define C5  523
#define Cs5 554
#define D5  587
#define Ds5 622
#define E5  659
#define F5  698
#define Fs5 740
#define G5  784
#define Gs5 831
#define A5  880
#define As5 932
#define B5  988

#define C6  1047

// Melody sequence
const uint16_t default_notes[] = {C4, E4, G4, G4, G4, G4, G4, G4, G4, G4, G4, E4, C4, E4, D4, C4, D4, E4, 
                            C4, E4, G4, G4, G4, G4, G4, G4, G4, G4, G4, E4, C4, E4, D4, C4, D4, C4};

// Rhythm sequence
const uint16_t _default_durations[] = {250, 250, 125, 125, 125, 125, 125, 125, 125, 125, 250, 250, 250, 250, 500, 250, 250, 500, 
                                250, 250, 125, 125, 125, 125, 125, 125, 125, 125, 250, 250, 250, 250, 500, 250, 250, 500};

#define NUM_NOTES_DEFAULT (sizeof(default_notes)/sizeof(default_notes[0]))

// Victory sequence
const uint16_t victory_notes[] = {D5, D5, D5, D5, A4, C5, D5, C5, D5};

// Rhythm sequence
const uint16_t victory_durations[] = {160, 160, 160, 500, 500, 500, 320, 160, 1500};

#define NUM_NOTES_VICTORY (sizeof(victory_notes)/sizeof(victory_notes[0]))
																
osThreadId_t buzzerThreadId;
osThreadId_t defaultBuzzerThreadId;
osThreadId_t victoryBuzzerThreadId;
																
// Function to initialize PWM on TPM1
void initBuzzerPWM(void) {
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
    if (freq == 0) { 
        TPM1->MOD = 0;
        TPM1_C0V = 0;
        return;
    }
    uint16_t mod_value = (375000 / freq) - 1;
    TPM1->MOD = mod_value;
    TPM1_C0V = mod_value * 3 / 4;  // 50% duty cycle
}

void playMelody(const uint16_t *notes, const uint16_t *durations, int numNotes) {
    for (int i = 0; i < numNotes; i++) {
        setNoteFrequency(notes[i]);  // Set PWM frequency
        osDelay(durations[i]);  // Play for the note duration
        setNoteFrequency(0);  // Silence between notes
        osDelay(25);  // Short gap between notes
    }
}

void defaultBuzzerThread(void *argument){
    while (1) {
        playMelody(default_notes, _default_durations, NUM_NOTES_DEFAULT);
        osDelay(100);
    }
}

void victoryBuzzerThread(void *argument){
    while (1) {
        //osThreadSuspend(victoryBuzzerThreadId);
        playMelody(victory_notes, victory_durations, NUM_NOTES_VICTORY);
        //buzzerState = 0;
    }
}

// Function to switch threads
void buzzerControl() {
    if (buzzerState == 1) {
        osThreadSuspend(defaultBuzzerThreadId);
        osThreadResume(victoryBuzzerThreadId);
    } else {
        osThreadSuspend(victoryBuzzerThreadId);
        osThreadResume(defaultBuzzerThreadId);
    }
}

void tBuzzer(void *argument) {
    uint8_t lastState = 0;

    while (1) {
        if (buzzerState != lastState) {
            buzzerControl();
            lastState = buzzerState;
        }
    }
}
