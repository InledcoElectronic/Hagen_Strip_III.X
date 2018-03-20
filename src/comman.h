/* 
 * File:   comman.h
 * Author: Administrator
 *
 * Created on 2016?10?15?, ??11:31
 */

#ifndef COMMAN_H
#define	COMMAN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

#define _XTAL_FREQ  16000000

    //ble module pins
#define BLE_RST         LATB5
#define BLE_WKP         LATC1
#define BLE_INT         RA2

#define STRIP_NIGHT_CHN_NUM         2
#define MARINE_NIGHT_CHN_NUM        2
#define PLANT_NIGHT_CHN_NUM         1  
#define NANO_MARINE_NIGHT_CHN_NUM   2
#define NANO_PLANT_NIGHT_CHN_NUM    1

#define LIGHT_TYPE_STRIP_III            0x0111
#define LIGHT_TYPE_MARINE_REEF_500      0x0121
#define LIGHT_TYPE_MARINE_REEF_800      0x0122
#define LIGHT_TYPE_MARINE_REEF_1100     0x0123
#define LIGHT_TYPE_MARINE_REEF_1000     0x0124
#define LIGHT_TYPE_PLANT_FRESH_500      0x0131
#define LIGHT_TYPE_PLANT_FRESH_800      0x0132
#define LIGHT_TYPE_PLANT_FRESH_1100     0x0133
#define LIGHT_TYPE_PLANT_FRESH_1000     0x0134
#define LIGHT_TYPE_NANO_MARINE          0x0151
#define LIGHT_TYPE_NANO_PLANT           0x0152

#define LIGHT_ID_STRIP_III              "30313131"            //0111
#define LIGHT_ID_MARINE_REEF_500        "30313231"            //0121
#define LIGHT_ID_MARINE_REEF_800        "30313232"            //0122
#define LIGHT_ID_MARINE_REEF_1100       "30313233"            //0123
#define LIGHT_ID_MARINE_REEF_1000       "30313234"            //0124
#define LIGHT_ID_PLANT_FRESH_500        "30313331"            //0131
#define LIGHT_ID_PLANT_FRESH_800        "30313332"            //0132
#define LIGHT_ID_PLANT_FRESH_1100       "30313333"            //0133
#define LIGHT_ID_PLANT_FRESH_1000       "30313334"            //0134
#define LIGHT_ID_NANO_MARINE            "30313531"            //0151
#define LIGHT_ID_NANO_PLANT             "30313532"            //0152

#define FIRMWARE_VERSION                "30313032"            //0102
#define DEVICE_ID_VERSION               LIGHT_ID FIRMWARE_VERSION           

#if     LIGHT_TYPE == LIGHT_TYPE_STRIP_III
#define CHANNEL_COUNT   4
#define LED_NIGHT_CHN_NUM    STRIP_NIGHT_CHN_NUM
#else
#define CHANNEL_COUNT   5
#if     LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_500 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_800 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_1100 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_1000
#define LED_NIGHT_CHN_NUM   MARINE_NIGHT_CHN_NUM
#elif   LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_500 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_800 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_1100 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_1000
#define LED_NIGHT_CHN_NUM   PLANT_NIGHT_CHN_NUM
#elif   LIGHT_TYPE == LIGHT_TYPE_NANO_MARINE
#define LED_NIGHT_CHN_NUM   NANO_MARINE_NIGHT_CHN_NUM
#elif   LIGHT_TYPE == LIGHT_TYPE_NANO_PLANT
#define LED_NIGHT_CHN_NUM   NANO_PLANT_NIGHT_CHN_NUM
#endif
#endif

#define BLE_CMD_SLAVE       "AT+SET=1\r\n"
#define BLE_CMD_DATA        "AT+MODE=DATA\r\n" 
#define BLE_CMD_GET_ADV     "AT+ADV_MFR_SPC=?\r\n" 
#define BLE_CMD_ACK_OK      "AT+OK\r\n"	
#define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=" DEVICE_ID_VERSION "\r\n" 

    //#define	OVERTIME		30			//unit 4ms
    //#define	RESENDCOUNT		3
    //#define	RCVINTERVAL		15			//unit 4ms

#define CUSTOM_COUNT            4  

    typedef struct {
        uint8_t hour;
        uint8_t minute;
    } Time_t;

    typedef struct {
        Time_t start;
        Time_t end;
    } TimeRamp_t;

    typedef union {

        struct {
            unsigned fOn : 1;
            unsigned fRsv : 7;
            uint8_t fDyn;
            uint16_t nBrt[CHANNEL_COUNT];
            uint8_t nCustomBrt[4][CHANNEL_COUNT];
        } manualPara;
        uint8_t array[6 * CHANNEL_COUNT + 2];
    } ManualPara_t;

    typedef union {

        struct {
            TimeRamp_t sunrise;
            uint8_t daylight[CHANNEL_COUNT];
            TimeRamp_t sunset;
            uint8_t moonlight[CHANNEL_COUNT];
        } autoPara;
        uint8_t array[2 * CHANNEL_COUNT + 8];
    } AutoPara_t;

    typedef struct {
        uint16_t nDayBrt;
        uint16_t nNightBrt;
    } TouchPara_t;

    typedef struct {
        unsigned fAuto : 1;
        unsigned fSta : 2;
        unsigned fRsv : 5;
        ManualPara_t mPara;
        AutoPara_t aPara;
        TouchPara_t tPara;
    } LedPara_t;

    typedef struct {
        unsigned find : 1;
        unsigned fSave : 1;
        unsigned fPrev : 1;
        unsigned fDr : 1;
        unsigned fNr : 1;
        unsigned fRsv : 3;
        uint8_t nFlashCount;
        uint8_t nPrevCount;
        uint8_t nSaveDelayCount;
        uint16_t nTargetBrt[CHANNEL_COUNT];
        uint16_t nCurrentBrt[CHANNEL_COUNT];
        uint8_t nCustomIndex;
    } LedRunPara_t;

    extern volatile LedRunPara_t gLedRunPara;
    extern volatile LedPara_t gLedPara;

    extern void SYSTEM_Initialize();
    extern void registerCallback();

#ifdef	__cplusplus
}
#endif

#endif	/* COMMAN_H */

