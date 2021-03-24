#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"

#define BUFFER_SIZE		16
UINT8  temp,temp1;
UINT16 u16CNT=0,u16CNT1=0;
bit riflag;

void MODIFY_Hirc166(void)
{
    UINT8 xdata hircmap0,hircmap1;
    UINT16 xdata trimvalue16bit;
    // Check if power on reset, modify HIRC 
    if ((PCON&SET_BIT4)==SET_BIT4)				
    {
        hircmap0 = RCTRIM0;
        hircmap1 = RCTRIM1;
        trimvalue16bit = ((hircmap0<<1)+(hircmap1&0x01));
        trimvalue16bit = trimvalue16bit - 15;
        hircmap1 = trimvalue16bit&0x01;
        hircmap0 = trimvalue16bit>>1;
        TA=0XAA;
        TA=0X55;
        RCTRIM0 = hircmap0;
        TA=0XAA;
        TA=0X55;
        RCTRIM1 = hircmap1;
        // Clear power on flag 
        PCON &= CLR_BIT4;
    }
}

void SerialPort1_ISR(void) interrupt 15 
{
    if (RI_1==1) 
    {                                       /* if reception occur */
        clr_RI_1;                             /* clear reception flag for next reception */
        Send_Data_To_UART0(SBUF_1);
    }
}

void SerialPort0_ISR(void) interrupt 4
{
    if(RI == 1)
    {
        clr_RI;
        Send_Data_To_UART1(SBUF);

    }
}

void main (void)
{
    MODIFY_Hirc166();
    InitialUART0_Timer1(9600);
    InitialUART1_Timer3(9600);
    
	//set_ES_1;					//For interrupt enable
    set_ES;
	set_EA;
    while (1)
    {
        // temp = Receive_Data_From_UART0();
        // Send_Data_To_UART1(temp);
        temp1 = Receive_Data_From_UART1();
        Send_Data_To_UART0(temp1);
			//Send_Data_To_UART1(0x68);
        //Timer2_Delay500us(10000);
        /* code */
			
    }
    
}