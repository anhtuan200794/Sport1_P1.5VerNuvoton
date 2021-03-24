#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "string.h"

#define BUFFER_SIZE 16
#define MOTO P30
UINT8 temp;
UINT16 u16CNT = 0, u16CNT1 = 0;
char *motorStart = "MOTOR_START\r\n"; //12
char *motorStop = "MOTOR_STOP\r\n";   //11
UINT8 PreviousStatus = 0;

void MODIFY_Hirc166(void)
{
    UINT8 xdata hircmap0, hircmap1;
    UINT16 xdata trimvalue16bit;
    // Check if power on reset, modify HIRC
    if ((PCON & SET_BIT4) == SET_BIT4)
    {
        hircmap0 = RCTRIM0;
        hircmap1 = RCTRIM1;
        trimvalue16bit = ((hircmap0 << 1) + (hircmap1 & 0x01));
        trimvalue16bit = trimvalue16bit - 15;
        hircmap1 = trimvalue16bit & 0x01;
        hircmap0 = trimvalue16bit >> 1;
        TA = 0XAA;
        TA = 0X55;
        RCTRIM0 = hircmap0;
        TA = 0XAA;
        TA = 0X55;
        RCTRIM1 = hircmap1;
        // Clear power on flag
        PCON &= CLR_BIT4;
    }
}

void UART0_SendData(UINT8 *buff, UINT8 len) // USB2COM port
{
    UINT8 i;
    for (i = 0; i < len; i++)
    {
        Send_Data_To_UART0(buff[i]);
        Timer2_Delay500us(1);
    }
}

// void SerialPort1_ISR(void) interrupt 15
// {
//     if (RI_1 == 1)
//     {   /* if reception occur */
//         clr_RI_1; /* clear reception flag for next reception */
//         Send_Data_To_UART0(SBUF_1);
//     }
// }

void SerialPort0_ISR(void) interrupt 4
{
    if (RI == 1)
    {
        clr_RI;
        Send_Data_To_UART1(SBUF);
    }
}

void main(void)
{
    P11_Input_Mode;

    //MODIFY_Hirc166();
    InitialUART0_Timer1(9600);
    InitialUART1_Timer3(9600);
    //set_ES_1;					//For interrupt enable
    set_ES;
    set_EA;
    UART0_SendData((UINT8 *)motorStart, strlen(motorStart));
    while (1)
    {
        if (P30 != 0 && PreviousStatus == 0)
        {
            UART0_SendData((UINT8 *)motorStart, strlen(motorStart));
            //Send_Data_To_UART0(0xFF);
            PreviousStatus = 1;
        }
        else if (P30 == 0 && PreviousStatus == 1)
        {
            UART0_SendData((UINT8 *)motorStop, strlen(motorStop));
            //Send_Data_To_UART0(0xFE);
            PreviousStatus = 0;
        }
        else
        {
            temp = Receive_Data_From_UART1();
            Send_Data_To_UART0(temp);
        }

        // UART0_SendData((UINT8 *)motorStart, strlen(motorStart));
        // Timer2_Delay500us(2000);
    }
}