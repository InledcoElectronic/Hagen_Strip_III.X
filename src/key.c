#include "key.h"
#include "led.h"
#include "eeprom.h"

#define	RPT_COUNT		32                  //16ms*32=0.512s
#define LONG_COUNT      100                 //16ms*100=1.6s
#define	KEY_MINTIME		2					//按键持续时间最小值
#define	CONT_COUNT		1					//16ms

Key_OnStateChangedCallback_t fnKeyAction;

static uint8_t KEY_Read( ) {
	static uint8_t Trg;
	static uint8_t Cont;
	uint8_t ReadData = ( KEY_PORT ^ 0xFF ) & KEY_MASK;
	Trg = ReadData & ( ReadData ^ Cont );
	Cont = ReadData;
	return ( Trg ^ Cont );
}

void KEY_Scan( ) {
	static uint8_t lastKey = KEY_NONE;
	static uint8_t rptCount = 0;

	uint8_t keyNum = KEY_Read( );
	if ( keyNum != KEY_NONE ) {
		if ( keyNum == lastKey ) {
			rptCount++;
			if ( rptCount == RPT_COUNT ) {
//				KEY_Action( keyNum, KEY_PRESS_LONG );
				if ( fnKeyAction != NULL ) {
					fnKeyAction( keyNum, KEY_PRESS_LONG );
				}
			} else if ( rptCount >= RPT_COUNT + CONT_COUNT ) {
				rptCount = RPT_COUNT;
				//				keyValue = KEY_CONT_MASK | keyNum;
//				KEY_Action( keyNum, KEY_PRESS_CONT );
				if ( fnKeyAction != NULL ) {
					fnKeyAction( keyNum, KEY_PRESS_CONT );
				}
			}
		} else {
			rptCount = 0;
		}
	} else {
		if ( lastKey != KEY_NONE ) {
			if ( rptCount >= RPT_COUNT ) {
				//				keyValue = KEY_RELEASE_MASK | lastKey;
//				KEY_Action( lastKey, KEY_RELEASE_1 );
				if ( fnKeyAction != NULL ) {
					fnKeyAction( lastKey, KEY_RELEASE_1 );
				}
			} else if ( rptCount > KEY_MINTIME ) {
				//				keyValue = lastKey;
//				KEY_Action( lastKey, KEY_PRESS_SHORT );
				if ( fnKeyAction != NULL ) {
					fnKeyAction( lastKey, KEY_PRESS_SHORT );
				}
			}
		}
		rptCount = 0;
	}
	lastKey = keyNum;
}

void KEY_SetOnStateChangedCallback( Key_OnStateChangedCallback_t callback ) {
	fnKeyAction = callback;
}