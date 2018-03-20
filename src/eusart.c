#include "eusart.h"

#define TX_BUF_SIZE		64

uint8_t txTail;
uint8_t txRemain;
uint8_t txHead;
uint8_t txBuf[TX_BUF_SIZE];

EUSART_ReceiveInterruptCallback_t fnReceiveCallback;

void EUSART_Initialize( ) {
	// disable interrupts before changing states
	PIE1bits.RCIE = 0;
	PIE1bits.TXIE = 0;

	// Set the EUSART module to the options selected in the user interface.

	// ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
	BAUD1CON = 0x08;

	// SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
	RC1STA = 0x90;

	// TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
	TX1STA = 0x24;

	// Baud Rate = 9600; SP1BRGL 160; 
	SP1BRGL = 0xA0;

	// Baud Rate = 9600; SP1BRGH 1; 
	SP1BRGH = 0x01;

	// enable receive interrupt
	PIE1bits.RCIE = 1;

	txTail = 0;
	txHead = 0;
	txRemain = sizeof ( txBuf );
}

void EUSART_RCV_ISR( ) {
	uint8_t rcv;
	if ( RC1STAbits.FERR || RC1STAbits.OERR ) {
		RC1STAbits.SPEN = 0;
		RC1STAbits.SPEN = 1;
	}

	rcv = RCREG;
	if ( fnReceiveCallback != NULL ) {
		fnReceiveCallback( rcv );
	}
}

void EUSART_Transmit_ISR( ) {
	if ( sizeof ( txBuf ) > txRemain ) {
		TXREG = txBuf[txTail++];
		if ( sizeof ( txBuf ) <= txTail ) {
			txTail = 0;
		}
		txRemain++;
	} else {
		PIE1bits.TXIE = 0;
	}
}

uint8_t EUSART_ReadByte( ) {
	while ( !PIR1bits.RCIF );
	if ( RC1STAbits.OERR ) {
		RC1STAbits.CREN = 0;
		RC1STAbits.CREN = 1;
	}
	return RCREG;
}

//void EUSART_SendByte( uint8_t byte ) {
//	while ( !TXIF );
//	TXREG = byte;
//}

/*********************************************************************
 * Function:        uint8_t EUSART_Write( uint8_t byte )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t EUSART_Write( uint8_t byte ) {
	while ( 0 == txRemain );

	if ( !PIE1bits.TXIE ) {
		TXREG = byte;
	} else {
		PIE1bits.TXIE = 0;
		txBuf[txHead++] = byte;
		if ( sizeof ( txBuf ) <= txHead ) {
			txHead = 0;
		}
		txRemain--;
	}
	PIE1bits.TXIE = 1;
	return byte;
}

/*********************************************************************
 * Function:        void EUSART_SetReceiveCallback( EUSART_ReceiveInterruptCallback_t callback )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void EUSART_SetReceiveCallback(  EUSART_ReceiveInterruptCallback_t callback ) {
	fnReceiveCallback = callback;
}
