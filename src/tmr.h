/* 
 * File:   tmr.h
 * Author: liruya
 *
 * Created on 2016?11?21?, ??3:34
 */

#ifndef TMR_H
#define	TMR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"

    typedef void (*TMR_InterruptCallback_t)();

    extern void TMR0_Initialize();
    extern void TMR0_StartTimer();
    extern void TMR0_StopTimer();
    extern void TMR0_SetInterruptCallback(TMR_InterruptCallback_t callback);
    extern void TMR0_ISR();

    extern void TMR2_Initialize();
    extern void TMR2_StartTimer();
    extern void TMR2_StopTimer();
    extern void TMR2_SetInterruptCallback(TMR_InterruptCallback_t callback);
    extern void TMR2_ISR();

#ifdef	__cplusplus
}
#endif

#endif	/* TMR_H */

