/* 
 * File:   eusart.h
 * Author: liruya
 *
 * Created on 2016?11?19?, ??3:22
 */

#ifndef EUSART_H
#define	EUSART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"

    typedef void ( *EUSART_ReceiveInterruptCallback_t ) (uint8_t rcv);

    extern void EUSART_Initialize();
    extern void EUSART_Transmit_ISR();
    extern void EUSART_RCV_ISR();
    extern uint8_t EUSART_ReadByte();
//    extern void EUSART_SendByte(uint8_t byte);
    extern uint8_t EUSART_Write(uint8_t byte);
    extern void EUSART_SetReceiveCallback(EUSART_ReceiveInterruptCallback_t callback);

#ifdef	__cplusplus
}
#endif

#endif	/* EUSART_H */

