#include "led.h"
#include "eeprom.h"
#include "rtc.h"
#include "key.h"
#include "pwm.h"
#include "util.h"

#define ADDR_LED_PARA       0x10            //led para saved offset addr
#define PARA_SAVE_DELAY     3               //para saved delay if there is no para changed

static void LED_StateOffAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state );
static void LED_StateDayAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state );
static void LED_StateNightAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state );
static void LED_StateBleAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state );
Key_OnStateChangedCallback_t led_state_actions[4] = { LED_StateOffAction_OnKeyStateChanged,
													 LED_StateDayAction_OnKeyStateChanged,
													 LED_StateNightAction_OnKeyStateChanged,
													 LED_StateBleAction_OnKeyStateChanged };

typedef void (*PWM_LoadDuty_Func_t )( uint16_t duty );
#if     LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_500 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_800 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_1100 || LIGHT_TYPE == LIGHT_TYPE_MARINE_REEF_1000
static PWM_LoadDuty_Func_t PWM_LoadDutyValue[CHANNEL_COUNT] = { PWM2_LoadDutyValue,
															   PWM4_LoadDutyValue,
															   PWM5_LoadDutyValue,
															   PWM3_LoadDutyValue,
															   PWM1_LoadDutyValue };
#elif	LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_500 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_800 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_1100 || LIGHT_TYPE == LIGHT_TYPE_PLANT_FRESH_1000
static PWM_LoadDuty_Func_t PWM_LoadDutyValue[CHANNEL_COUNT] = { PWM2_LoadDutyValue,
															   PWM5_LoadDutyValue,
															   PWM3_LoadDutyValue,
															   PWM1_LoadDutyValue,
															   PWM4_LoadDutyValue };
#elif	LIGHT_TYPE == LIGHT_TYPE_NANO_MARINE
static PWM_LoadDuty_Func_t PWM_LoadDutyValue[CHANNEL_COUNT] = { PWM2_LoadDutyValue,
															   PWM1_LoadDutyValue,
															   PWM3_LoadDutyValue,
															   PWM4_LoadDutyValue,
															   PWM5_LoadDutyValue };
#elif	LIGHT_TYPE == LIGHT_TYPE_NANO_PLANT
static PWM_LoadDuty_Func_t PWM_LoadDutyValue[CHANNEL_COUNT] = { PWM3_LoadDutyValue,
															   PWM2_LoadDutyValue,
															   PWM1_LoadDutyValue,
															   PWM5_LoadDutyValue,
															   PWM4_LoadDutyValue };
#elif	LIGHT_TYPE == LIGHT_TYPE_STRIP_III
static PWM_LoadDuty_Func_t PWM_LoadDutyValue[CHANNEL_COUNT] = { PWM3_LoadDutyValue,
															   PWM2_LoadDutyValue,
															   PWM4_LoadDutyValue,
															   PWM1_LoadDutyValue };
#endif

void LED_InitStatus( );

/**
 *	LED Para sava
 */

/*********************************************************************
 * Function:        void LED_PrepareToSavePara()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_PrepareToSavePara( ) {
	gLedRunPara.fSave = 1;
	gLedRunPara.nSaveDelayCount = PARA_SAVE_DELAY;
}

/*********************************************************************
 * Function:        void LED_SavePara()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_SavePara( ) {
	if ( gLedRunPara.nSaveDelayCount > 0 ) {
		gLedRunPara.nSaveDelayCount--;
	}
	if ( gLedRunPara.nSaveDelayCount == 0 ) {
		EEPROM_WriteBuffer( ADDR_LED_PARA, ( const uint8_t * const ) &gLedPara, sizeof ( gLedPara ) );
		gLedRunPara.fSave = 0;
	}
}

/*********************************************************************
 * Function:        void LED_ReadPara()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_ReadPara( ) {
	EEPROM_ReadBuffer( ADDR_LED_PARA, ( uint8_t * const ) &gLedPara, sizeof ( gLedPara ) );
}

void LED_UpdatePWM( ) {
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		PWM_LoadDutyValue[i]( gLedRunPara.nCurrentBrt[i] );
	}
}

void LED_UpdateBrightRamp( ) {
	if ( gLedRunPara.find || gLedRunPara.fPrev || gLedPara.fAuto ) {
		return;
	}
	if ( gLedPara.fSta == LED_STATUS_BLE ) {
		for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
			if ( gLedRunPara.nCurrentBrt[i] + STEP_NORMAL_BRIGHT < gLedRunPara.nTargetBrt[i] ) {
				gLedRunPara.nCurrentBrt[i] += STEP_NORMAL_BRIGHT;
			} else if ( gLedRunPara.nTargetBrt[i] + STEP_NORMAL_BRIGHT < gLedRunPara.nCurrentBrt[i] ) {
				gLedRunPara.nCurrentBrt[i] -= STEP_NORMAL_BRIGHT;
			} else {
				gLedRunPara.nCurrentBrt[i] = gLedRunPara.nTargetBrt[i];
			}
		}
		LED_UpdatePWM( );
	}
}

void LED_AutoRun( ) {
	uint8_t i, j;
	uint16_t ct = RTC_GetHour( ) * 60u + RTC_GetMinute( );
	uint8_t sec = RTC_GetSecond( );
	uint16_t tr[4];
	tr[0] = gLedPara.aPara.autoPara.sunrise.start.hour * 60u + gLedPara.aPara.autoPara.sunrise.start.minute;
	tr[1] = gLedPara.aPara.autoPara.sunrise.end.hour * 60u + gLedPara.aPara.autoPara.sunrise.end.minute;
	tr[2] = gLedPara.aPara.autoPara.sunset.start.hour * 60u + gLedPara.aPara.autoPara.sunset.start.minute;
	tr[3] = gLedPara.aPara.autoPara.sunset.end.hour * 60u + gLedPara.aPara.autoPara.sunset.end.minute;
	uint8_t val[4][CHANNEL_COUNT];
	for ( i = 0; i < CHANNEL_COUNT; i++ ) {
		val[0][i] = gLedPara.aPara.autoPara.moonlight[i];
		val[1][i] = gLedPara.aPara.autoPara.daylight[i];
		val[2][i] = gLedPara.aPara.autoPara.daylight[i];
		val[3][i] = gLedPara.aPara.autoPara.moonlight[i];
	}
	for ( i = 0; i < 4; i++ ) {
		j = ( i + 1 ) % 4;
		uint16_t st = tr[i];
		uint16_t et = tr[j];
		uint16_t duration;
		unsigned long dt;
		uint8_t dbrt;
		if ( et >= st ) {
			if ( ct >= st && ct < et ) {
				duration = et - st;
				dt = ( ct - st ) * 60ul + sec;
			} else {
				continue;
			}
		} else {
			if ( ct >= st || ct < et ) {
				duration = 1440u - st + et;
				if ( ct >= st ) {
					dt = ( ct - st ) * 60ul + sec;
				} else {
					dt = ( 1440u - st + ct ) * 60ul + sec;
				}
			} else {
				continue;
			}
		}
		for ( uint8_t k = 0; k < CHANNEL_COUNT; k++ ) {
			if ( val[j][k] >= val[i][k] ) {
				dbrt = val[j][k] - val[i][k];
				gLedRunPara.nCurrentBrt[k] = val[i][k] * 10u + dbrt * dt / ( duration * 6u );
			} else {
				dbrt = val[i][k] - val[j][k];
				gLedRunPara.nCurrentBrt[k] = val[i][k] * 10u - dbrt * dt / ( duration * 6u );
			}
		}
	}
	LED_UpdatePWM( );
}

/**
 * turn on led
 */
void LED_TurnOnRamp( ) {
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		gLedRunPara.nTargetBrt[i] = gLedPara.mPara.manualPara.nBrt[i];
	}
}

/**
 * turn off led ramp
 */
void LED_TurnOffRamp( ) {
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		gLedRunPara.nTargetBrt[i] = 0;
	}
}

void LED_TurnMax( ) {
	PWM1_LoadDutyValue( MAX_LED_BRIGHT );
	PWM2_LoadDutyValue( MAX_LED_BRIGHT );
	PWM3_LoadDutyValue( MAX_LED_BRIGHT );
	PWM4_LoadDutyValue( MAX_LED_BRIGHT );
	PWM5_LoadDutyValue( MAX_LED_BRIGHT );
}

/**
 * turn off led
 */
void LED_TurnOff( ) {
	PWM1_LoadDutyValue( 0 );
	PWM2_LoadDutyValue( 0 );
	PWM3_LoadDutyValue( 0 );
	PWM4_LoadDutyValue( 0 );
	PWM5_LoadDutyValue( 0 );
}

/**
 * when find device led flashes
 */
void LED_FindDeviceFlash( ) {
	static uint8_t tog = 0;
	if ( tog ) {
		tog = 0;
		LED_TurnOff( );
		if ( gLedRunPara.nFlashCount > 0 ) {
			gLedRunPara.nFlashCount--;
		}
		if ( gLedRunPara.nFlashCount == 0 ) {
			gLedRunPara.find = 0;
			//recovery status before find device
			LED_InitStatus( );
		}
	} else {
		tog = 1;
		LED_TurnMax( );
	}
}

void LED_Preview( ) {
	if ( gLedRunPara.nPrevCount > 0 ) {
		gLedRunPara.nPrevCount--;
	}
	if ( gLedRunPara.nPrevCount == 0 ) {
		gLedRunPara.fPrev = 0;
	}
}

//led turn off indicate - red led

void LED_IndicateOff( ) {
	gLedPara.fSta = LED_STATUS_OFF;
	LED_RED_SetLow( );
	LED_GRN_SetHigh( );
	LED_BLUE_SetHigh( );
}

//day light led indicate - white led

void LED_IndicateDay( ) {
	gLedPara.fSta = LED_STATUS_DAY;
	LED_RED_SetLow( );
	LED_GRN_SetLow( );
	LED_BLUE_SetLow( );
}

//night light indicate - blue led

void LED_IndicateNight( ) {
	gLedPara.fSta = LED_STATUS_NIGHT;
	LED_RED_SetHigh( );
	LED_GRN_SetHigh( );
	LED_BLUE_SetLow( );
}

//ble mode indicate - green led

void LED_IndicateBle( ) {
	gLedPara.fSta = LED_STATUS_BLE;
	LED_RED_SetHigh( );
	LED_GRN_SetLow( );
	LED_BLUE_SetHigh( );
}

void LED_UpdateDayBright( ) {
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		gLedPara.mPara.manualPara.nBrt[i] = gLedPara.tPara.nDayBrt;
		gLedRunPara.nTargetBrt[i] = gLedPara.tPara.nDayBrt;
		gLedRunPara.nCurrentBrt[i] = gLedPara.tPara.nDayBrt;
	}
	LED_UpdatePWM( );
}

void LED_UpdateDayRiseStatus( ) {
	if ( gLedPara.tPara.nDayBrt > MAX_LED_BRIGHT - DELTA_LED_BRIGHT ) {
		gLedRunPara.fDr = 0;
	} else if ( gLedPara.tPara.nDayBrt < DELTA_LED_BRIGHT ) {
		gLedRunPara.fDr = 1;
	}
}

void LED_UpdateNightBright( ) {
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		if ( i == LED_NIGHT_CHN_NUM ) {
			gLedPara.mPara.manualPara.nBrt[i] = gLedPara.tPara.nNightBrt;
			gLedRunPara.nTargetBrt[i] = gLedPara.tPara.nNightBrt;
			gLedRunPara.nCurrentBrt[i] = gLedPara.tPara.nNightBrt;
		} else {
			gLedPara.mPara.manualPara.nBrt[i] = 0;
			gLedRunPara.nTargetBrt[i] = 0;
			gLedRunPara.nCurrentBrt[i] = 0;
		}
	}
	LED_UpdatePWM( );
}

void LED_UpdateNightRiseStatus( ) {
	if ( gLedPara.tPara.nNightBrt > MAX_LED_BRIGHT - DELTA_LED_BRIGHT ) {
		gLedRunPara.fNr = 0;
	} else if ( gLedPara.tPara.nNightBrt < DELTA_LED_BRIGHT ) {
		gLedRunPara.fNr = 1;
	}
}

/**
 *	LED Action On Key State Changed
 */

/*********************************************************************
 * Function:        static void LED_StateOffAction_OnKeyPressShort()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateOffAction_OnKeyPressShort( ) {
	//	gLedPara.fSta++;
	LED_UpdateDayBright( );
	LED_IndicateDay( );
	LED_UpdateDayRiseStatus( );
}

/*********************************************************************
 * Function:        static void LED_StateOffAction_OnKeyPressCont()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateOffAction_OnKeyPressCont( ) {
	gLedPara.tPara.nDayBrt = MIN_LED_BRIGHT;
	//	gLedPara.fSta++;
	gLedRunPara.fDr = 1;
	LED_UpdateDayBright( );
	LED_IndicateDay( );
}

/*********************************************************************
 * Function:        static void LED_StateOffAction_OnKeyStateChanged(uint8_t key, enum Key_State_t state)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateOffAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state ) {
	if ( key == KEY1 ) {
		switch ( state ) {
			case KEY_PRESS_SHORT:
				LED_StateOffAction_OnKeyPressShort( );
				break;

			case KEY_PRESS_CONT:
				LED_StateOffAction_OnKeyPressCont( );
				break;

			case KEY_RELEASE_1:
			case KEY_PRESS_LONG:
			default:
				break;
		}
	}
}

/*********************************************************************
 * Function:        static void LED_StateDay_OnKeyPressShort()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateDay_OnKeyPressShort( ) {
	//	gLedPara.fSta++;
	LED_UpdateNightBright( );
	LED_IndicateNight( );
	LED_UpdateNightRiseStatus( );
}

/*********************************************************************
 * Function:        static void LED_StateDay_OnKeyPressCont()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateDay_OnKeyPressCont( ) {
	if ( gLedRunPara.fDr ) {
		if ( gLedPara.tPara.nDayBrt + LED_DELTA_TOUCH < MAX_LED_BRIGHT ) {
			gLedPara.tPara.nDayBrt += LED_DELTA_TOUCH;
		} else {
			gLedPara.tPara.nDayBrt = MAX_LED_BRIGHT;
		}
	} else {
		if ( gLedPara.tPara.nDayBrt > MIN_LED_BRIGHT + LED_DELTA_TOUCH ) {
			gLedPara.tPara.nDayBrt -= LED_DELTA_TOUCH;
		} else {
			gLedPara.tPara.nDayBrt = MIN_LED_BRIGHT;
		}
	}
	LED_UpdateDayBright( );
}

/*********************************************************************
 * Function:        static void LED_StateDay_OnKeyRelease()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateDay_OnKeyRelease( ) {
	LED_UpdateDayRiseStatus( );
}

/*********************************************************************
 * Function:        static void LED_StateDayAction_OnKeyStateChanged(uint8_t key, enum Key_State_t state)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateDayAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state ) {
	if ( key == KEY1 ) {
		switch ( state ) {
			case KEY_PRESS_SHORT:
				LED_StateDay_OnKeyPressShort( );
				break;

			case KEY_PRESS_CONT:
				LED_StateDay_OnKeyPressCont( );
				break;

			case KEY_RELEASE_1:
				LED_StateDay_OnKeyRelease( );
				break;

			case KEY_PRESS_LONG:
			default:
				break;
		}
	}
}

/*********************************************************************
 * Function:        static void LED_StateNight_OnKeyPressShort()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateNight_OnKeyPressShort( ) {
	//	gLedPara.fSta++;
	LED_IndicateBle( );
	gLedPara.fAuto = 1;
}

/*********************************************************************
 * Function:        static void LED_StateNight_OnKeyPressCont()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateNight_OnKeyPressCont( ) {
	if ( gLedRunPara.fNr ) {
		if ( gLedPara.tPara.nNightBrt + LED_DELTA_TOUCH < MAX_LED_BRIGHT ) {
			gLedPara.tPara.nNightBrt += LED_DELTA_TOUCH;
		} else {
			gLedPara.tPara.nNightBrt = MAX_LED_BRIGHT;
		}
	} else {
		if ( gLedPara.tPara.nNightBrt > MIN_LED_BRIGHT + LED_DELTA_TOUCH ) {
			gLedPara.tPara.nNightBrt -= LED_DELTA_TOUCH;
		} else {
			gLedPara.tPara.nNightBrt = MIN_LED_BRIGHT;
		}
	}
	LED_UpdateNightBright( );
}

/*********************************************************************
 * Function:        static void LED_StateNight_OnKeyRelease()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateNight_OnKeyRelease( ) {
	LED_UpdateNightRiseStatus( );
}

/*********************************************************************
 * Function:        static void LED_StateNightAction_OnKeyStateChanged(uint8_t key, enum Key_State_t state)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateNightAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state ) {
	if ( key == KEY1 ) {
		switch ( state ) {
			case KEY_PRESS_SHORT:
				LED_StateNight_OnKeyPressShort( );
				break;

			case KEY_PRESS_CONT:
				LED_StateNight_OnKeyPressCont( );
				break;

			case KEY_RELEASE_1:
				LED_StateNight_OnKeyRelease( );
				break;

			case KEY_PRESS_LONG:
			default:
				break;
		}
	}
}

/*********************************************************************
 * Function:        static void LED_StateBle_OnKeyPressShort()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateBle_OnKeyPressShort( ) {
	//	gLedPara.fSta++;
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		gLedPara.mPara.manualPara.nBrt[i] = 0;
		gLedRunPara.nTargetBrt[i] = 0;
		gLedRunPara.nCurrentBrt[i] = 0;
	}
	LED_UpdatePWM( );
	LED_IndicateOff( );
}

/*********************************************************************
 * Function:        static void LED_StateBleAction_OnKeyStateChanged(uint8_t key, enum Key_State_t state)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void LED_StateBleAction_OnKeyStateChanged( uint8_t key, enum Key_State_t state ) {
	if ( key == KEY1 ) {
		switch ( state ) {
			case KEY_PRESS_SHORT:
				LED_StateBle_OnKeyPressShort( );
				break;

			case KEY_PRESS_CONT:
			case KEY_RELEASE_1:
			case KEY_PRESS_LONG:
			default:
				break;
		}
	}
}

/*********************************************************************
 * Function:        void LED_Action_OnKeyStateChanged(uint8_t key, enum Key_State_t state)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_Action_OnKeyStateChanged( uint8_t key, enum Key_State_t state ) {
	led_state_actions[gLedPara.fSta]( key, state );
}

/*********************************************************************
 * Function:        void LED_ActionOnSecond()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_ActionOnSecond( ) {
	//when find device
	if ( gLedRunPara.find ) {
		LED_FindDeviceFlash( );
	} else if ( gLedPara.fSta == LED_STATUS_BLE && gLedPara.fAuto ) {
		//fast preview auto mode
		if ( gLedRunPara.fPrev ) {
			LED_Preview( );
		} else {
			LED_AutoRun( );
		}
	}

	if ( gLedRunPara.fSave ) {
		LED_SavePara( );
	}
}

/*********************************************************************
 * Function:        void LED_StartFlash()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_StartFlash( ) {
	gLedRunPara.find = 1;
	gLedRunPara.nFlashCount = FIND_FLASH_COUNT;
}

/*********************************************************************
 * Function:        void LED_StartPreview()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_StartPreview( ) {
	gLedRunPara.fPrev = 1;
	gLedRunPara.nPrevCount = PREVIEW_COUNT;
}

/*********************************************************************
 * Function:        void LED_StopPreview()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void LED_StopPreview( ) {
	gLedRunPara.fPrev = 0;
	gLedRunPara.nPrevCount = 0;
}

/*********************************************************************
 * Function:        bool LED_SetCustom( uint8_t chn )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
bool LED_SetCustom( uint8_t idx ) {
	if ( !gLedRunPara.find && !gLedPara.fAuto && gLedPara.mPara.manualPara.fOn && idx < CUSTOM_COUNT ) {
		for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
			gLedPara.mPara.manualPara.nCustomBrt[idx][i] = gLedRunPara.nCurrentBrt[i] / 10;
		}
		return true;
	}
	return false;
}

bool LED_SetAuto( bool a ) {
	if ( !gLedRunPara.find && !gLedRunPara.fPrev ) {
		if ( a ) {
			gLedPara.fAuto = 1;
		} else {
			gLedPara.fAuto = 0;
			if ( gLedPara.mPara.manualPara.fOn ) {
				LED_TurnOnRamp( );
			} else {
				LED_TurnOffRamp( );
			}
		}
		return true;
	}
	return false;
}

bool LED_SetOn( bool on ) {
	if ( !gLedRunPara.find && !gLedPara.fAuto ) {
		if ( on ) {
			gLedPara.mPara.manualPara.fOn = 1;
			LED_TurnOnRamp( );
		} else {
			gLedPara.mPara.manualPara.fOn = 0;
			LED_TurnOffRamp( );
		}
		return true;
	}
	return false;
}

bool LED_SetManualBrights( const uint8_t * const pbuf ) {
	if ( !gLedRunPara.find && gLedPara.mPara.manualPara.fOn && ( !gLedPara.fAuto ) ) {
		gLedPara.mPara.manualPara.fDyn = 0;
		for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
			uint16_t val = ( *( pbuf + 2 * i ) ) << 8 | ( *( pbuf + 2 * i + 1 ) );
			if ( val <= MAX_LED_BRIGHT ) {
				gLedPara.mPara.manualPara.nBrt[i] = val;
				gLedRunPara.nTargetBrt[i] = val;
			}
		}
		return true;
	}
	return false;
}

bool LED_SetCycle( const uint8_t * const pbuf ) {
	if ( !gLedRunPara.find && !gLedRunPara.fPrev && gLedPara.fAuto ) {
		uint8_t len = sizeof ( gLedPara.aPara.array );
		for ( uint8_t i = 0; i < len; i++ ) {
			gLedPara.aPara.array[i] = *( pbuf + i );
		}
		return true;
	}
	return false;
}

bool LED_ManualIncrease( uint8_t chn, uint8_t delta ) {
	if ( !gLedRunPara.find && !gLedPara.fAuto && gLedPara.mPara.manualPara.fOn && chn < CHANNEL_COUNT ) {
		Util_Increase( ( uint16_t * const ) &gLedPara.mPara.manualPara.nBrt[chn], delta, MAX_LED_BRIGHT );
		Util_Increase( ( uint16_t * const ) &gLedRunPara.nTargetBrt[chn], delta, MAX_LED_BRIGHT );
		return true;
	}
	return false;
}

bool LED_ManualDecrease( uint8_t chn, uint8_t delta ) {
	if ( !gLedRunPara.find && !gLedPara.fAuto && gLedPara.mPara.manualPara.fOn && chn < CHANNEL_COUNT ) {
		Util_Decrease( ( uint16_t * const ) &gLedPara.mPara.manualPara.nBrt[chn], delta, 0 );
		Util_Decrease( ( uint16_t * const ) &gLedRunPara.nTargetBrt[chn], delta, 0 );
		return true;
	}
	return false;
}

bool LED_SetPreviewBrights( const uint8_t * const pbuf ) {
	if ( !gLedRunPara.find && gLedPara.fAuto ) {
		for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
			uint16_t val = ( *( pbuf + 2 * i ) ) << 8 | ( *( pbuf + 2 * i + 1 ) );
			if ( val <= MAX_LED_BRIGHT ) {
				gLedRunPara.nCurrentBrt[i] = val;
			}
		}
		LED_UpdatePWM( );
		return true;
	}
	return false;
}

void LED_InitPara( ) {
	LED_ReadPara( );
	gLedPara.fRsv = 0;
	gLedPara.mPara.manualPara.fRsv = 0;
	for ( uint8_t i = 0; i < CHANNEL_COUNT; i++ ) {
		//manual light check
		if ( gLedPara.mPara.manualPara.nBrt[i] > MAX_LED_BRIGHT ) {
			gLedPara.mPara.manualPara.nBrt[i] = MAX_LED_BRIGHT;
		}
		//custom light check
		for ( uint8_t j = 0; j < 4; j++ ) {
			if ( gLedPara.mPara.manualPara.nCustomBrt[j][i] > 100 ) {
				gLedPara.mPara.manualPara.nCustomBrt[j][i] = 100;
			}
		}
		//day light check
		if ( gLedPara.aPara.autoPara.daylight[i] > 100 ) {
			gLedPara.aPara.autoPara.daylight[i] = 100;
		}
		//night light check
		if ( gLedPara.aPara.autoPara.moonlight[i] > 100 ) {
			gLedPara.aPara.autoPara.moonlight[i] = 100;
		}
	}

	//sunrise start time check
	if ( gLedPara.aPara.autoPara.sunrise.start.hour > 23 ) {
		gLedPara.aPara.autoPara.sunrise.start.hour = 0;
	}
	if ( gLedPara.aPara.autoPara.sunrise.start.minute > 59 ) {
		gLedPara.aPara.autoPara.sunrise.start.minute = 0;
	}
	//sunrise end time check
	if ( gLedPara.aPara.autoPara.sunrise.end.hour > 23 ) {
		gLedPara.aPara.autoPara.sunrise.end.hour = 0;
	}
	if ( gLedPara.aPara.autoPara.sunrise.end.minute > 59 ) {
		gLedPara.aPara.autoPara.sunrise.end.minute = 0;
	}
	//sunset start time check
	if ( gLedPara.aPara.autoPara.sunset.start.hour > 23 ) {
		gLedPara.aPara.autoPara.sunset.start.hour = 0;
	}
	if ( gLedPara.aPara.autoPara.sunset.start.minute > 59 ) {
		gLedPara.aPara.autoPara.sunset.start.minute = 0;
	}
	//sunset end time check
	if ( gLedPara.aPara.autoPara.sunset.end.hour > 23 ) {
		gLedPara.aPara.autoPara.sunset.end.hour = 0;
	}
	if ( gLedPara.aPara.autoPara.sunset.end.minute > 59 ) {
		gLedPara.aPara.autoPara.sunset.end.minute = 0;
	}

	if ( gLedPara.tPara.nDayBrt > MAX_LED_BRIGHT ) {
		gLedPara.tPara.nDayBrt = MAX_LED_BRIGHT;
	} else if ( gLedPara.tPara.nDayBrt < MIN_LED_BRIGHT ) {
		gLedPara.tPara.nDayBrt = MIN_LED_BRIGHT;
	}
	LED_UpdateDayRiseStatus( );
	if ( gLedPara.tPara.nNightBrt > MAX_LED_BRIGHT ) {
		gLedPara.tPara.nNightBrt = MAX_LED_BRIGHT;
	} else if ( gLedPara.tPara.nNightBrt < MIN_LED_BRIGHT ) {
		gLedPara.tPara.nNightBrt = MIN_LED_BRIGHT;
	}
	LED_UpdateNightRiseStatus( );
}

void LED_InitStatus( ) {
	uint8_t sta = gLedPara.fSta;
	switch ( sta ) {
		case LED_STATUS_OFF:
			LED_TurnOffRamp( );
			LED_IndicateOff( );
			break;

		case LED_STATUS_DAY:
			LED_UpdateDayBright( );
			LED_IndicateDay( );
			LED_TurnOnRamp( );
			break;

		case LED_STATUS_NIGHT:
			LED_UpdateNightBright( );
			LED_IndicateNight( );
			LED_TurnOnRamp( );
			break;

		case LED_STATUS_BLE:
			LED_IndicateBle( );
			if ( !gLedPara.fAuto ) {
				if ( gLedPara.mPara.manualPara.fOn ) {
					LED_TurnOnRamp( );
				} else {
					LED_TurnOffRamp( );
				}
			}
			break;

		default:
			break;
	}
}

void LED_Initialize( ) {
	LED_InitPara( );
	LED_InitStatus( );
}