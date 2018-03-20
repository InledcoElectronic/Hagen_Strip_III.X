/* 
 * File:   rtc.h
 * Author: liruya
 *
 * Created on November 1, 2016, 1:15 PM
 */

#ifndef RTC_H
#define	RTC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

    extern void RTC_Initialize();
    extern void RTC_Run();
    extern bool RTC_IsSecondReady();
    extern void RTC_SetCurrentTime(const uint8_t * const time);
    extern uint8_t RTC_GetYear();
    extern uint8_t RTC_GetMonth();
    extern uint8_t RTC_GetDay();
    extern uint8_t RTC_GetWeekday();
    extern uint8_t RTC_GetHour();
    extern uint8_t RTC_GetMinute();
    extern uint8_t RTC_GetSecond();

#ifdef	__cplusplus
}
#endif

#endif	/* RTC_H */

