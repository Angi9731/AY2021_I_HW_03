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
#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"

//variabili globali inizializzate a 0 all'accensione
uint8 flag = 0;
uint8 count_TIMER = 0;


int main(void)
{
    CyGlobalIntEnable;
    
    uint8 received; // variabile in cui si salva il byte ricevuto
    uint8 DC_RED = 0; // valore iniziale dei 3 duty cycle settato a 0
    uint8 DC_GREEN = 0;
    uint8 DC_BLUE = 0;
    
    //avvio UART
    UART_Start(); 
    ISR_UART_StartEx(Custom_UART_ISR);
    
    //avvio TIMER
    ISR_TIMER_StartEx(Custom_TIMER_ISR);
    timer_clock_Start();
    
    //avvio PWM
    Clock_PWM_Start();
    PWM_RED_GREEN_Start();
    PWM_BLUE_Start();
    
    //variabili che consentono di passare da uno stato all'altro (spiegate man mano)
    uint8 start_received = 0;
    uint8 stop_received = 1; // variabile settata a 1 all'accensione e quando viene ricevuto il byte di stop
    uint8 red_received = 0;
    uint8 green_received = 0;
    uint8 blue_received = 0;
    
    
    UART_PutString("Inserisci il pattern\n");
   

    for(;;)
    {
        if(count_TIMER >= 5)// se sono trascorsi 5s dall'invio dell'ultimo byte
            {
                UART_PutString("Troppo tardi! Reinserire il pattern\n");
                Timer_Stop(); // arresto il timer
                count_TIMER = 0;
                stop_received = 1; // variabile che consente di tornare allo stato iniziale
            }
        
        if(flag == 1)// se è stato ricevuto un byte
        {
           received = UART_ReadRxData(); // salva il valore del byte in received
            
            if((received == 'v') && (stop_received == 1)) // se received è uguale a v
            {
               UART_PutString("RGB LED Program $$$\n"); // invia questa stringa
               flag = 0; // riporta il flag a 0
            }
            
            else if((flag) && (received == 0xA0) && (stop_received)) //se un byte è stato ricevuto, si tratta del byte di start e siamo all'accensione oppure è stato precedentemente ricevuto il byte di stop
            {
               
               Timer_Start();
               count_TIMER = 0; // Il timer inizia a contare
               start_received ++; // variabile che segnala che il byte di start è stato ricevuto
               stop_received = 0; 
               flag = 0;
               
            }
            
             else if((flag) && (start_received) && (count_TIMER < 5)) // se un byte è stato ricevuto, il byte precedente era quello di start e non sono ancora trascorsi 5s
            {
                
               DC_RED = received; // salva il valore del byte come duty cycle del ROSSO
               Timer_Start();
               count_TIMER = 0; // Il timer ricomincia a contare
               red_received ++; // variabile che segnala che il byte del ROSSO è stato ricevuto
               start_received = 0;
               flag = 0;
                
            }
            
             else if((red_received) && (flag) && (count_TIMER < 5)) // se un byte è stato ricevuto, il byte precedente era quello relativo al ROSSO e non sono ancora trascorsi 5s
            {
                
               DC_GREEN = received; // salva il valore del byte come duty cycle del VERDE
               Timer_Start();
               count_TIMER = 0; // Il timer ricomincia a contare
               green_received ++; // variabile che segnala che il byte del VERDE è stato ricevuto
               red_received = 0;
               flag = 0;
               
            }
            
            else if((green_received) && (flag) && (count_TIMER < 5)) // se un byte è stato ricevuto, il byte precedente era quello relativo al VERDE e non sono ancora trascorsi 5s
            {
               
               DC_BLUE = received; // salva il valore del byte come duty cycle del BLU
               Timer_Start();
               count_TIMER = 0; // Il timer ricomincia a contare
               blue_received ++; // variabile che segnala che il byte del BLU è stato ricevuto
               green_received = 0;
               flag = 0;
                
            }
            
            else if((blue_received) && (received == 0xC0) && (flag) && (count_TIMER < 5)) // se un byte è stato ricevuto, quello precedente era quello del blu, si tratta del byte di stop e non sono ancora trascorsi 5s
            {
                
               Timer_Stop(); // Arresta il timer
               stop_received ++; // variabile che segnala che il byte di stop è stato ricevuto
               blue_received = 0;
               flag = 0;
                
               PWM_RED_GREEN_WriteCompare1(DC_GREEN); // setto i Duty Cycle dei PWM
               PWM_RED_GREEN_WriteCompare2(DC_RED);
               PWM_BLUE_WriteCompare(DC_BLUE);
                
               UART_PutChar(DC_RED);
                
               UART_PutChar(DC_GREEN);
                
               UART_PutChar(DC_BLUE);
            
            }
            
            
            else if ((blue_received) && (received != 0xC0) && (flag)) // se l'ultimo byte non è quello corretto (0xC0)
            {
                UART_PutString("Errore del byte di stop\n");
                Timer_Stop(); // arresta il timer
                count_TIMER = 0;
                stop_received++; // setto questa variabile per tornare allo stato iniziale
                blue_received = 0;
                flag = 0;
            }
           
        }   
                         
    }
    
}

/* [] END OF FILE */
