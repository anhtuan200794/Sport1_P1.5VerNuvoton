
/*
 * Copyright (c) 2021
 * VitechSolutions
 * All Rights Reserved
 *
 *
 * Description:
 *
 * Author: TuanNA
 *
 */
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "string.h"

#define UART0_BAUD 9600
#define UART1_BAUD 9600
#define MOTO P10

char *motorStart = "MOTOR_START\r\n"; //12
char *motorStop = "MOTOR_STOP\r\n";   //11
UINT8 PreviousStatus = 0;

void UART0_SendData(UINT8 *buff, UINT8 len) // USB2COM port
{
    UINT8 i;
    for (i = 0; i < len; i++)
    {
        Send_Data_To_UART0(buff[i]);
        Timer2_Delay500us(1);
    }
}

void main(void)
{
    P10_Quasi_Mode;
    InitialUART0_Timer1(UART0_BAUD); //initialization
    InitialUART1_Timer3(UART1_BAUD);
    set_ES; //For interrupt enable
    set_ES_1;
    set_EA;
    Timer0_Delay1ms(1000);
    while (1)
    {
        if (MOTO != 0 && PreviousStatus == 0)
        {
            clr_ES;
            UART0_SendData((UINT8 *)motorStart, strlen(motorStart));
            PreviousStatus = 1;
            set_ES;
        }
        else if (MOTO == 0 && PreviousStatus == 1)
        {
            clr_ES;
            UART0_SendData((UINT8 *)motorStop, strlen(motorStop));
            PreviousStatus = 0;
            set_ES;
        }
    }
}

void SerialPort_ISR(void) interrupt 4
{
    if (RI)
    {
        clr_RI;
        clr_ES_1;
        Send_Data_To_UART1(SBUF);
        set_ES_1;
    }
    if (TI)
    {
        clr_TI;
    }
}
void SerialPort1_ISR(void) interrupt 15
{
    if (RI_1)
    {
        clr_RI_1;
        clr_ES;
        Send_Data_To_UART0(SBUF_1);
        set_ES;
    }
    if (TI_1)
    {
        clr_TI_1;
    }
}
