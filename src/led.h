/* 
 * File:   led.h
 * Author: liruya
 *
 * Created on November 1, 2016, 3:01 PM
 */

#ifndef LED_H
#define	LED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"
#include "key.h"

#define FIND_FLASH_COUNT    3
#define PREVIEW_COUNT       3
#define MIN_LED_BRIGHT      10
#define MAX_LED_BRIGHT      1000
#define DELTA_LED_BRIGHT    200
#define LED_STEP_TOUCH      2
#define LED_DELTA_TOUCH     10    
#define STEP_NORMAL_BRIGHT  4

#define LED_STATUS_OFF      0
#define LED_STATUS_DAY      1
#define LED_STATUS_NIGHT    2
#define LED_STATUS_BLE      3

#define LED_RED_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define LED_RED_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define LED_RED_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define LED_BLUE_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define LED_BLUE_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define LED_BLUE_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define LED_GRN_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define LED_GRN_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define LED_GRN_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)    

    extern void LED_UpdateBrightRamp();
    extern void LED_Initialize();
    extern void LED_IndicateBle();
    extern void LED_Action_OnKeyStateChanged(uint8_t key, enum Key_State_t state);
    extern void LED_ActionOnSecond();
    extern void LED_PrepareToSavePara();
    extern void LED_StartFlash();
    extern void LED_StartPreview();
    extern void LED_StopPreview();
    extern bool LED_SetAuto(bool a);
    extern bool LED_SetOn(bool on);
    extern bool LED_SetManualBrights(const uint8_t * const pbuf);
    extern bool LED_SetCustom(uint8_t idx);
    extern bool LED_SetCycle(const uint8_t * const pbuf);
    extern bool LED_ManualIncrease(uint8_t chn, uint8_t delta);
    extern bool LED_ManualDecrease(uint8_t chn, uint8_t delta);
    extern bool LED_SetPreviewBrights(const uint8_t * const pbuf);

#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

