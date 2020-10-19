/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "InterruptRoutines.h"
#include "stdio.h"
#include "project.h"

extern uint8 flag; // variabile globale che segnala la ricezione di un byte
extern uint8 count_TIMER; //variabile globale che tiene traccia dei secondi trascorsi

CY_ISR(Custom_UART_ISR)
{
    if(UART_ReadRxStatus() == UART_RX_STS_FIFO_NOTEMPTY)
    {
        flag = 1; // ogni volta che viene ricevuto un byte flag viene settata a 1
    }
    
}

CY_ISR(Custom_TIMER_ISR)
{
    count_TIMER ++; // count_TIMER viene incrementato di 1 ogni secondo poichè il clock del timer è a 255Hz e period = 255
    Timer_ReadStatusRegister();
}
/* [] END OF FILE */
