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

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)
    
#define _XTAL_FREQ  16000000
    
//ble module pins
#define BLE_RST         LATB5
#define BLE_WKP		LATC1
#define BLE_INT		RA2
   
#define STRIP_NIGHT_CHN_NUM         2
#define MARINE_NIGHT_CHN_NUM        2
#define PLANT_NIGHT_CHN_NUM         1  
#define NANO_MARINE_NIGHT_CHN_NUM   2
#define NANO_PLANT_NIGHT_CHN_NUM    1
    
#define LIGHT_TYPE_STRIP_III            0x0111
#define LIGHT_TYPE_MARINE_REEF_500      0x0121
#define LIGHT_TYPE_MARINE_REEF_800      0x0122
#define LIGHT_TYPE_MARINE_REEF_1100     0x0123
#define LIGHT_TYPE_PLANT_FRESH_500      0x0131
#define LIGHT_TYPE_PLANT_FRESH_800      0x0132
#define LIGHT_TYPE_PLANT_FRESH_1100     0x0133
#define LIGHT_TYPE_NANO_MARINE          0x0151
#define LIGHT_TYPE_NANO_PLANT           0x0152
 
//#define LIGHT_TYPE      LIGHT_TYPE_STRIP_III
//#define LIGHT_TYPE      LIGHT_TYPE_MARINE_REEF_500
//#define LIGHT_TYPE      LIGHT_TYPE_MARINE_REEF_800
//#define LIGHT_TYPE      LIGHT_TYPE_MARINE_REEF_1100
//#define LIGHT_TYPE      LIGHT_TYPE_PLANT_FRESH_500
//#define LIGHT_TYPE      LIGHT_TYPE_PLANT_FRESH_800
//#define LIGHT_TYPE      LIGHT_TYPE_PLANT_FRESH_1100
//#define LIGHT_TYPE      LIGHT_TYPE_NANO_MARINE
#define LIGHT_TYPE      LIGHT_TYPE_NANO_PLANT             

#define BLE_CMD_SLAVE	"AT+SET=1\r\n"
#define BLE_CMD_DATA	"AT+MODE=DATA\r\n" 
#define BLE_CMD_GET_ADV "AT+ADV_MFR_SPC=?\r\n" 
#define BLE_CMD_ACK_OK	"AT+OK\r\n"	
#if     LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_500
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031323130313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031323130313032\r\n"       //id 0121 version 0102
    #define LED_NIGHT_CHN_NUM   MARINE_NIGHT_CHN_NUM  
#elif   LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_800
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031323230313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031323230313032\r\n"       //id 0122 version 0102
    #define LED_NIGHT_CHN_NUM   MARINE_NIGHT_CHN_NUM  
#elif   LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_1100
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031323330313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031323330313032\r\n"       //id 0123 version 0102
    #define LED_NIGHT_CHN_NUM   MARINE_NIGHT_CHN_NUM  
#elif   LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_500
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031333130313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031333130313032\r\n"       //id 0131 version 0102
    #define LED_NIGHT_CHN_NUM   PLANT_NIGHT_CHN_NUM  
#elif   LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_800
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031333230313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031333230313032\r\n"       //id 0132 version 0102
    #define LED_NIGHT_CHN_NUM   PLANT_NIGHT_CHN_NUM 
#elif   LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_1100
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031333330313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031333330313032\r\n"       //id 0133 version 0102
    #define LED_NIGHT_CHN_NUM   PLANT_NIGHT_CHN_NUM 
#elif   LIGHT_TYPE == LIGHT_TYPE_NANO_MARINE
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031353130313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031353130313032\r\n"       //id 0151 version 0102
    #define LED_NIGHT_CHN_NUM   NANO_MARINE_NIGHT_CHN_NUM
#elif   LIGHT_TYPE == LIGHT_TYPE_NANO_PLANT
    #define CHANNEL_COUNT       5
    #define DEVICE_ID_VERSION   "3031353230313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031353230313032\r\n"       //id 0152 version 0102
    #define LED_NIGHT_CHN_NUM   NANO_PLANT_NIGHT_CHN_NUM
#elif   LIGHT_TYPE == LIGHT_TYPE_STRIP_III
    #define CHANNEL_COUNT       4
    #define DEVICE_ID_VERSION   "3031313130313032"	
    #define BLE_CMD_SET_ADV     "AT+ADV_MFR_SPC=3031313130313032\r\n"       //id 0111 version 0102
    #define LED_NIGHT_CHN_NUM   STRIP_NIGHT_CHN_NUM 
#else

#endif
	
#define	OVERTIME		30			//unit 4ms
#define	RESENDCOUNT		3
#define	RCVINTERVAL		15			//unit 4ms
	
#define CUSTOM_COUNT            4
    
//para defines
#define EEPROM_START_ADDR   0x7000          //eeprom start addr
#define ADDR_LED_PARA       0x10            //led para saved offset addr
#define PARA_SAVE_DELAY     3               //para saved delay if there is no para changed    

typedef union{
    struct{
        unsigned char year;
        unsigned char month;
        unsigned char day;
        unsigned char wk;
        unsigned char hour;
        unsigned char minute;
        unsigned char second;
    }datetime;
    unsigned char array[7];
}DateTime_t;

typedef struct{
    unsigned char hour;
    unsigned char minute;
}Time_t;

typedef struct{
    Time_t start;
    Time_t end;
}TimeRamp_t;

typedef union{
    struct{
        unsigned fOn : 1;
        unsigned fRsv : 7;
        unsigned char fDyn;
        unsigned int nBrt[CHANNEL_COUNT];
        unsigned char nCustomBrt[4][CHANNEL_COUNT];
    }manualPara;
    unsigned char array[6*CHANNEL_COUNT+2];
}ManualPara_t;

typedef union{
    struct{
        TimeRamp_t sunrise;
        unsigned char daylight[CHANNEL_COUNT];
        TimeRamp_t sunset;
        unsigned char moonlight[CHANNEL_COUNT];
    }autoPara;
    unsigned char array[2*CHANNEL_COUNT+8];
}AutoPara_t;

typedef struct{
    unsigned int nDayBrt;
    unsigned int nNightBrt;
}TouchPara_t;

typedef struct{
    unsigned fAuto : 1;
    unsigned fSta : 2;
    unsigned fRsv : 5;
    ManualPara_t mPara;
    AutoPara_t aPara;
    TouchPara_t tPara;
}LedPara_t;

typedef struct{
    unsigned find : 1;
    unsigned fSec : 1;
    unsigned fSave : 1;
    unsigned fPrev : 1;
    unsigned fDr : 1;
    unsigned fNr : 1;
    unsigned fRsv : 2;
    unsigned char nFlashCount;
    unsigned char nPrevCount;
    unsigned char nSaveDelayCount;
    unsigned int nTargetBrt[CHANNEL_COUNT];
    unsigned int nCurrentBrt[CHANNEL_COUNT];
    unsigned char nCustomIndex;
}LedRunPara_t;

extern  volatile LedRunPara_t gLedRunPara;
extern  volatile LedPara_t    gLedPara;

extern  void SYSTEM_Initialize();
extern	unsigned char strCmp(const unsigned char *s1, const unsigned char *s2);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMAN_H */

