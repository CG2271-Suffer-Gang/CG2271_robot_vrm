#include "MKL25Z4.h"
#include "motorv2.c"

#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128
#define Q_SIZE 32

typedef struct {
    uint8_t Data[Q_SIZE];
    unsigned int Head;
    unsigned int Tail;
    unsigned int Size;
} Q_T;

volatile Q_T tx_q, rx_q;

/* Initialize Queue */
void Q_Init(volatile Q_T *q) {
    q->Head = q->Tail = q->Size = 0;
}

/* Check if Queue is Empty */
int Q_Empty(volatile Q_T *q) {
    return q->Size == 0;
}

/* Check if Queue is Full */
int Q_Full(volatile Q_T *q) {
    return q->Size == Q_SIZE;
}

/* Enqueue Data */
int Q_Enqueue(volatile Q_T *q, uint8_t data) {
    if (!Q_Full(q)) {
        q->Data[q->Tail++] = data;
        q->Tail %= Q_SIZE;
        q->Size++;
        return 1;
    }
    return 0;
}

/* Dequeue Data */
uint8_t Q_Dequeue(volatile Q_T *q) {
    uint8_t data = 0;
    if (!Q_Empty(q)) {
        data = q->Data[q->Head++];
        q->Head %= Q_SIZE;
        q->Size--;
    }
    return data;
}

/* Init UART2 */
void initUART2(uint32_t baud_rate) {
    uint32_t divisor, bus_clock;

    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);

    PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);

    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

    bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
    divisor = bus_clock / (baud_rate * 16);
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);

    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;

    NVIC_SetPriority(UART2_IRQn, 64); 
    NVIC_ClearPendingIRQ(UART2_IRQn); 
    NVIC_EnableIRQ(UART2_IRQn);
		
		NVIC_SetPriority(UART1_IRQn, 128);
		NVIC_ClearPendingIRQ(UART1_IRQn);
		NVIC_EnableIRQ(UART1_IRQn);

    UART2->C2 |= UART_C2_TIE_MASK | UART_C2_RIE_MASK;
    UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

    Q_Init(&tx_q);
    Q_Init(&rx_q);
}

void UART2_IRQHandler(void) {
    NVIC_ClearPendingIRQ(UART2_IRQn);

    if (UART2->S1 & UART_S1_TDRE_MASK) {
        // can send another character
        if (!Q_Empty(&tx_q)) {
            UART2->D = Q_Dequeue(&tx_q);
        } else {
            // queue is empty so disable tx
            UART2->C2 &= ~UART_C2_TIE_MASK;
        }
    }
    // Receive ISR
    if (UART2->S1 & UART_S1_RDRF_MASK) {
        if (!Q_Full(&rx_q)) {
            Q_Enqueue(&rx_q, UART2->D);
						//NVIC_SetPendingIRQ(UART1_IRQn); //comment out this line to switch back to tBrain implementation
        }
    }
}

//use other ISR just to actually execute the commands
void UART1_IRQHandler(void) {
		NVIC_ClearPendingIRQ(UART1_IRQn);
	
		if (!Q_Empty(&rx_q)) {
				uint8_t data = Q_Dequeue(&rx_q);
				switch(data) {
						case LEFT:
						case RIGHT:
						case FRONT:
						case BACK:
						case BACKLEFT:
						case FRONTRIGHT:
						case FRONTLEFT:
						case BACKRIGHT:
						case STOP:
								directionState = data;
								controlDirectionMovement();
								break;
						case SLOW:
						case FAST:
								speedState = data;
								changeMotorSpeed();
								break;
						case VICTORY:
                buzzerState = !buzzerState;
                break;
						default:
								break;
				}
		}
}

/* UART2 Transmit Function */
void UART2_Send(uint8_t data) {
    if (!Q_Full(&tx_q)) {
        Q_Enqueue(&tx_q, data);
        UART2->C2 |= UART_C2_TIE_MASK; // Enable TX interrupt, may not be necessary
    }
}

/* UART2 Receive Function */
uint8_t UART2_Receive(void) {
    //while (Q_Empty(&rx_q)); // Wait until data is available
    return Q_Dequeue(&rx_q);
}
