#include "rtc.h"

typedef union {

	struct {
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t wk;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
	} datetime;
	uint8_t array[7];
} DateTime_t;

typedef struct {
	unsigned fSec : 1;
	unsigned fMin : 1;
	unsigned fHour : 1;
	unsigned fWk : 1;
	unsigned fDay : 1;
	unsigned fMon : 1;
	unsigned fYear : 1;
	unsigned : 1;
} RTC_Status_t;

#define BASE_YEAR               2000
#define NORMAL_YEAR_FEBRUARY    28
#define LEAP_YEAR_FEBRUARY      29

static volatile DateTime_t gCurrentTime;
static RTC_Status_t prvStatus;

uint8_t MONTH_DAY[12] = { 31, NORMAL_YEAR_FEBRUARY, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
 * check if this is a leap year
 * @param year  the year to be check
 * @return 1-yes 0-false
 */
static bool RTC_IsLeapYear( uint16_t year ) {
	if ( year & 0x03 ) {
		if ( year % 100 == 0 ) {
			if ( year % 400 == 0 ) {
				return true;
			}
		} else {
			return true;
		}
	}
	return false;
}

/**
 * init real time 
 */
void RTC_Initialize( ) {
	gCurrentTime.datetime.second = 0;
	gCurrentTime.datetime.minute = 0;
	gCurrentTime.datetime.hour = 0;
	gCurrentTime.datetime.wk = 0;
	gCurrentTime.datetime.day = 1;
	gCurrentTime.datetime.month = 1;
	gCurrentTime.datetime.year = 0;
}

/**
 *  rtc run
 */
void RTC_Run( ) {
	gCurrentTime.datetime.second++;
	if ( gCurrentTime.datetime.second >= 60 ) {
		gCurrentTime.datetime.second = 0;
		gCurrentTime.datetime.minute++;
		if ( gCurrentTime.datetime.minute >= 60 ) {
			gCurrentTime.datetime.minute = 0;
			gCurrentTime.datetime.hour++;
			if ( gCurrentTime.datetime.hour >= 24 ) {
				gCurrentTime.datetime.hour = 0;
				gCurrentTime.datetime.day++;
				gCurrentTime.datetime.wk++;
				if ( gCurrentTime.datetime.wk > 6 ) {
					gCurrentTime.datetime.wk = 0;
				}
				if ( RTC_IsLeapYear( gCurrentTime.datetime.year + BASE_YEAR ) ) {
					MONTH_DAY[1] = LEAP_YEAR_FEBRUARY;
				} else {
					MONTH_DAY[1] = NORMAL_YEAR_FEBRUARY;
				}
				if ( gCurrentTime.datetime.month <= 12 && gCurrentTime.datetime.month > 0 ) {
					if ( gCurrentTime.datetime.day > MONTH_DAY[gCurrentTime.datetime.month - 1] ) {
						gCurrentTime.datetime.day = 1;
						gCurrentTime.datetime.month++;
						if ( gCurrentTime.datetime.month > 12 ) {
							gCurrentTime.datetime.month = 1;
							gCurrentTime.datetime.year++;
						}
					}
				}
			}
		}
	}
	prvStatus.fSec = 1;
}

/*********************************************************************
 * Function:        bool RTC_IsSecondReady()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
bool RTC_IsSecondReady( ) {
	if ( prvStatus.fSec ) {
		prvStatus.fSec = 0;
		return true;
	}
	return false;
}

/*********************************************************************
 * Function:        void RTC_SetCurrentTime( const uint8_t * const time )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void RTC_SetCurrentTime( const uint8_t * const time ) {
	for ( uint8_t i = 0; i < sizeof ( gCurrentTime ); i++ ) {
		gCurrentTime.array[i] = *( time + i );
	}
}

/*********************************************************************
 * Function:        uint8_t RTC_GetYear()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetYear(  ) {
	return gCurrentTime.datetime.year;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetMonth()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetMonth(  ) {
	return gCurrentTime.datetime.month;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetDay()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetDay(  ) {
	return gCurrentTime.datetime.day;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetWeekday()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetWeekday(  ) {
	return gCurrentTime.datetime.wk;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetHour()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetHour(  ) {
	return gCurrentTime.datetime.hour;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetMinute()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetMinute(  ) {
	return gCurrentTime.datetime.minute;
}

/*********************************************************************
 * Function:        uint8_t RTC_GetSecond()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t RTC_GetSecond(  ) {
	return gCurrentTime.datetime.second;
}





