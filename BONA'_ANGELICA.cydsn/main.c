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

uint8 flag = 0;
uint8 count_TIMER = 0;


int main(void)
{
    CyGlobalIntEnable;
    
    uint8 received;
    uint8 DC_RED = 0;
    uint8 DC_GREEN = 0;
    uint8 DC_BLUE = 0;
    
    UART_Start();
    ISR_UART_StartEx(Custom_UART_ISR);
    ISR_TIMER_StartEx(Custom_TIMER_ISR);
    timer_clock_Start();
    
    Clock_PWM_Start();
    PWM_RED_GREEN_Start();
    PWM_BLUE_Start();
    
    
    
    uint8 start_received = 0;
    uint8 stop_received = 1;
    uint8 red_received = 0;
    uint8 green_received = 0;
    uint8 blue_received = 0;
    
    
    
    UART_PutString("Inserisci valore di Start\n");
   

    for(;;)
    {
        if(count_TIMER >= 5)
            {
                
                UART_PutString("Troppo tardi! Inserisci valore di Start\n");
                Timer_Stop();
                count_TIMER = 0;
                stop_received = 1;
            }
        
        if(flag == 1)
        {
           received = UART_ReadRxData();
            
            if(received == 'v')
            {
                UART_PutString("RGB LED Program $$$\n");
                flag = 0;
            }
            
            else if((flag) && (received == 0xA0) && (stop_received))
            {
               
               //UART_PutString("Inserisci valore per ROSSO\n");
               Timer_Start();
               count_TIMER = 0;
               start_received ++;
               stop_received = 0;
               flag = 0;
               
            }
            
             else if((flag) && (start_received) && (count_TIMER < 5))
            {
                
                DC_RED = received;
                //UART_PutString("Inserisci valore per VERDE\n");
                Timer_Start();
                count_TIMER = 0;
                red_received ++;
                start_received = 0;
                flag = 0;
                
            }
            
             else if((red_received) && (flag) && (count_TIMER < 5))
            {
                
                DC_GREEN = received;
                //UART_PutString("Inserisci valore per BLU\n");
                Timer_Start();
                count_TIMER = 0;
                green_received ++;
                red_received = 0;
                flag = 0;
               
              
            }
            
            else if((green_received) && (flag)&&(count_TIMER < 5))
            {
               
               DC_BLUE = received;
               //UART_PutString("Inserisci valore di Stop\n");
               Timer_Start();
               count_TIMER = 0;
               blue_received ++;
               green_received = 0;
               flag = 0;
                
            }
            
            else if((blue_received) && (received == 0xC0) &&(flag)&&(count_TIMER < 5))
            {
                
                Timer_Stop();
                //UART_PutString("Inserisci valore di Start\n");
                stop_received ++;
                blue_received = 0;
                flag = 0;
                
                PWM_RED_GREEN_WriteCompare1(DC_GREEN);
                PWM_RED_GREEN_WriteCompare2(DC_RED);
                PWM_BLUE_WriteCompare(DC_BLUE);
                
                UART_PutChar(DC_RED);
                
                UART_PutChar(DC_GREEN);
                
                UART_PutChar(DC_BLUE);
            }
            
            else if((flag) && (received != 0xA0) && (stop_received))
            {
                UART_PutString("Errore\n");
                flag = 0;
            }
            
            else if((blue_received) && (received != 0xC0)&&(flag))
            {
                UART_PutString("Errore\n");
                flag = 0;
            }
            
       
        }   
        
                
            
    }
}

/* [] END OF FILE */
