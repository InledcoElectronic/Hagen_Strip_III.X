#include "pin.h"

void PIN_Initialize(void)
{
    /**
    LATx registers
    */   
    LATA = 0x02;    
    LATB = 0xD0;
    LATC = 0x00;    

    /**
    TRISx registers
    */    
    TRISA = 0x34;
    TRISB = 0x00;
    TRISC = 0x05;

    /**
    ANSELx registers
    */   
    ANSELC = 0x00;
    ANSELB = 0x00;
    ANSELA = 0x00;
    
    /**
    WPUx registers
    */ 
    WPUB = 0x00;
    WPUA = 0x04;
    WPUC = 0x01;

    /**
    ODx registers
    */   
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;
   
    uint8_t state = GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    RXPPSbits.RXPPS = 0x12;		//RC2->EUSART:RX;
    RB4PPSbits.RB4PPS = 0x14;   //RB4->EUSART:TX;
    RC5PPSbits.RC5PPS = 0x0C;   //RC5->CCP1:CCP1;
    RC4PPSbits.RC4PPS = 0x0D;   //RC4->CCP2:CCP2;
    RC3PPSbits.RC3PPS = 0x0E;   //RC3->CCP3:CCP3;
    RC6PPSbits.RC6PPS = 0x0F;   //RC6->CCP4:CCP4;
    RC7PPSbits.RC7PPS = 0x02;	//RC7->PWM5

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS

    GIE = state;
}       
