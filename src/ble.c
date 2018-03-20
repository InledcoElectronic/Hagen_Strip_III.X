#include "ble.h"
#include "rtc.h"
#include "eeprom.h"
#include "eusart.h"
#include "led.h"
#include <xc.h>

#define END_FLASH           8192

#define FRM_HDR             0x68
#define CMD_DATA_AUTO       0x02
#define CMD_DATA_ONOFF      0x03
#define CMD_DATA_CTRL       0x04
#define CMD_DATA_READ       0x05
#define CMD_DATA_CUSTOM     0x06
#define CMD_DATA_CYCLE      0x07
#define CMD_DATA_INCREASE   0x08
#define CMD_DATA_DECREASE   0x09
#define CMD_DATA_DYN        0x0A
#define CMD_DATA_PREVIEW    0x0B
#define CMD_DATA_STOPPREV   0x0C
#define CMD_DATA_SYNCTIME   0x0E
#define CMD_DATA_FIND       0x0F
#define CMD_OTA_RESET       0x00

#define RX_BUF_SIZE     64  

uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint8_t nRcvIdx = 0;
static volatile uint8_t rcvXor = 0;

/*********************************************************************
 * Function:        void BLE_Receive( uint8_t rcv )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void BLE_Receive( uint8_t rcv ) {
	rxBuf[ nRcvIdx++ ] = rcv;
	rcvXor ^= rcv;
	if ( nRcvIdx >= sizeof ( rxBuf ) ) {
		nRcvIdx = 0;
	}
}

static void BLE_SendCMD( const uint8_t *pBuf ) {
	uint8_t ack[] = { 'A', 'T', '+', 'O', 'K', '\r', '\n' };
	uint8_t index = 0;
	uint16_t cnt = 0;
	uint8_t rev = RCREG;
	while ( *pBuf != '\0' ) {
		//		EUSART_SendByte( *pBuf++ );
		EUSART_Write( *pBuf++ );
	}

	//等待ble应答 并判断指令是否成功
	while ( index < sizeof ( ack ) ) {
		//        rev = eusartReadByte();
		if ( PIR1bits.RCIF ) {
			if ( RC1STAbits.OERR ) {
				RC1STAbits.CREN = 0;
				RC1STAbits.CREN = 1;
			}
			rev = RCREG;
			if ( rev == ack[index] ) {
				index++;
			} else {
				index = 0;
			}
		}
		__delay_us( 100 );
		cnt++;
		if ( cnt > 1000 ) {
			break;
		}
	}
}

static uint8_t BLE_CheckDevID( ) {
	uint8_t dev[] = DEVICE_ID_VERSION;
	uint8_t index = 0;
	uint16_t cnt = 0;
	uint8_t rev;
	BLE_SendCMD( BLE_CMD_GET_ADV );
	while ( *( dev + index ) != '\0' ) {
		//        rev = eusartReadByte();
		if ( PIR1bits.RCIF ) {
			if ( RC1STAbits.OERR ) {
				RC1STAbits.CREN = 0;
				RC1STAbits.CREN = 1;
			}
			rev = RCREG;
			if ( rev == *( dev + index ) ) {
				index++;
			} else {
				return 0;
			}
		}
		__delay_us( 100 );
		cnt++;
		if ( cnt > 1000 ) {
			return 0;
		}
	}
	return 1;
}

/*********************************************************************
 * Function:        void BLE_StartTransmit()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void BLE_StartTransmit( ) {
	BLE_WKP = 0;
	__delay_us( 800 );
}

/*********************************************************************
 * Function:        void BLE_StopTransmit()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
static void BLE_StopTransmit( ) {
	while ( !TRMT );
	__delay_us( 200 );
	BLE_WKP = 1;
}

static void BLE_SendDeviceData( ) {
	uint8_t i;
	uint8_t len;
	uint8_t xor = 0;
	BLE_StartTransmit( );
	xor ^= EUSART_Write( FRM_HDR );
	xor ^= EUSART_Write( CMD_DATA_READ );
	xor ^= EUSART_Write( gLedPara.fAuto );
	if ( gLedPara.fAuto ) {
		len = sizeof (gLedPara.aPara.array );
		for ( i = 0; i < len; i++ ) {
			xor ^= EUSART_Write( gLedPara.aPara.array[i] );
		}
	} else {
		len = sizeof (gLedPara.mPara.array );
		for ( i = 0; i < len; i++ ) {
			xor ^= EUSART_Write( gLedPara.mPara.array[i] );
		}
	}
	EUSART_Write( xor );
	BLE_StopTransmit( );
}

/*********************************************************************
 * Function:        void BLE_SendOTAAck()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void BLE_SendOTAAck( ) {
	BLE_StartTransmit( );
	EUSART_Write( 0x68 );
	EUSART_Write( 0x00 );
	EUSART_Write( 0x00 );
	EUSART_Write( 0x00 );
	EUSART_Write( 0x68 );
	BLE_StopTransmit( );
}

void BLE_DecodeData( ) {
	if ( rxBuf[0] != FRM_HDR || rcvXor != 0 ) {
		return;
	}
	switch ( rxBuf[1] ) {
		case CMD_DATA_AUTO:
			if ( nRcvIdx == 4 ) {
				if ( LED_SetAuto( rxBuf[2] ) ) {
					LED_PrepareToSavePara( );
					BLE_SendDeviceData( );
				}
			}
			break;

		case CMD_DATA_ONOFF:
			if ( nRcvIdx == 4 ) {
				if ( LED_SetOn( rxBuf[2] ) ) {
					LED_PrepareToSavePara( );
					BLE_SendDeviceData( );
				}
			}
			break;

		case CMD_DATA_CTRL:
			if ( nRcvIdx == 3 + 2 * CHANNEL_COUNT ) {
				if ( LED_SetManualBrights( &rxBuf[2] ) ) {
					LED_PrepareToSavePara( );
				}
			}
			break;

		case CMD_DATA_READ:
			if ( nRcvIdx == 3 ) {
				BLE_SendDeviceData( );
			}
			break;

		case CMD_DATA_CUSTOM:
			if ( nRcvIdx == 4 ) {
				if ( LED_SetCustom( rxBuf[2] ) ) {
					LED_PrepareToSavePara( );
					BLE_SendDeviceData( );
				}
			}
			break;

		case CMD_DATA_CYCLE:
			if ( nRcvIdx == 2 * CHANNEL_COUNT + 11 ) {
				if ( LED_SetCycle( &rxBuf[2] ) ) {
					LED_PrepareToSavePara( );
					BLE_SendDeviceData( );
				}
			}
			break;

		case CMD_DATA_INCREASE:
			if ( nRcvIdx == 5 ) {
				if ( LED_ManualIncrease( rxBuf[2], rxBuf[3] ) ) {
					LED_PrepareToSavePara( );
				}
			}
			break;

		case CMD_DATA_DECREASE:
			if ( nRcvIdx == 5 ) {
				if ( LED_ManualDecrease( rxBuf[2], rxBuf[3] ) ) {
					LED_PrepareToSavePara( );
				}
			}
			break;

		case CMD_DATA_PREVIEW:
			if ( nRcvIdx == 3 + 2 * CHANNEL_COUNT ) {
				if ( LED_SetPreviewBrights( &rxBuf[2] ) ) {
					
					LED_StartPreview( );
				}
			}
			break;

		case CMD_DATA_STOPPREV:
			if ( nRcvIdx == 3 ) {
				LED_StopPreview( );
			}
			break;

		case CMD_DATA_SYNCTIME:
			if ( nRcvIdx == 10 ) {
				RTC_SetCurrentTime( &rxBuf[2] );
				//				gLedPara.fSta = LED_STATUS_BLE;
				LED_IndicateBle( );
				BLE_SendDeviceData( );
			}
			break;

		case CMD_DATA_FIND:
			if ( nRcvIdx == 3 ) {
				LED_StartFlash( );
			}
			break;

		case CMD_OTA_RESET:
			if ( nRcvIdx == 5 ) {
				NVMADR = END_FLASH - 1;
				NVMCON1 = 0x94;
				NVMCON2 = 0x55;
				NVMCON2 = 0xAA;
				NVMCON1bits.WR = 1;
				NOP( );
				NOP( );
				while ( NVMCON1bits.WR );
				NVMCON1bits.WREN = 0;
				//				BLE_SendData( rxBuf, 5 );
				BLE_SendOTAAck( );
				__delay_ms( 96 );
				RESET( );
			}
			break;

		default:
			break;
	}
}

void BLE_Initialize( ) {
	BLE_WKP = 0;
	BLE_RST = 0;
	__delay_ms( 40 );
	BLE_RST = 1; //release from reset
	__delay_ms( 240 ); //delay 200ms after power on

	//检查设备id version是否写入蓝牙模块广播数据
	if ( BLE_CheckDevID( ) == 0 ) {
		__delay_ms( 40 );
		BLE_SendCMD( BLE_CMD_SET_ADV );
	}
	__delay_ms( 40 );
	//设置ble从机模式
	BLE_SendCMD( BLE_CMD_SLAVE );
	__delay_ms( 40 );
	//设置ble透传模式
	BLE_SendCMD( BLE_CMD_DATA );
	__delay_ms( 20 );
	BLE_WKP = 1;
}

/*********************************************************************
 * Function:        bool BLE_ReadyToReceive()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
bool BLE_ReadyToReceive( ) {
	if ( ( BLE_INT == 0 ) && BLE_WKP ) {
		return true;
	}
	return false;
}

/*********************************************************************
 * Function:        void BLE_StartReceive()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void BLE_StartReceive( ) {
	nRcvIdx = 0;
	rcvXor = 0;
	__delay_us( 800 );
	BLE_WKP = 0;
}

/*********************************************************************
 * Function:        bool BLE_ReceiveComplete()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
bool BLE_ReceiveComplete( ) {
	if ( BLE_INT && ( BLE_WKP == 0 ) ) {
		return true;
	}
	return false;
}

/*********************************************************************
 * Function:        void BLE_StopReceive()
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void BLE_StopReceive( ) {
	__delay_us( 100 );
	BLE_WKP = 1;
	nRcvIdx = 0;
	rcvXor = 0;
}


