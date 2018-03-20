#include "tmr.h"
#include "eusart.h"
#include "led.h"
#include "rtc.h"

TMR_InterruptCallback_t fnTmr0Callback;
TMR_InterruptCallback_t fnTmr2Callback;

/**
 * TMR0 APIs
 */

void TMR0_Initialize( void ) {
	// Set TMR0 to the options selected in the User Interface

	// T0OUTPS 1:1; T0EN disabled; T016BIT 8-bit; 
	T0CON0 = 0x00;

	// T0CS SOSC; T0CKPS 1:128; T0ASYNC not_synchronised; 
	T0CON1 = 0xD7;

	// TMR0H 255; 
	TMR0H = 0xFF;

	// TMR0L 0; 
	TMR0L = 0x00;

	// Clear Interrupt flag before enabling the interrupt
	PIR0bits.TMR0IF = 0;

	// Enabling TMR0 interrupt.
	PIE0bits.TMR0IE = 1;
}

void TMR0_StartTimer( void ) {
	T0CON0bits.T0EN = 1;
}

void TMR0_StopTimer( void ) {
	T0CON0bits.T0EN = 0;
}

void TMR0_SetInterruptCallback( TMR_InterruptCallback_t callback ) {
	fnTmr0Callback = callback;
}

void TMR0_ISR( ) {
	PIR0bits.TMR0IF = 0;
	if ( fnTmr0Callback != NULL ) {
		fnTmr0Callback( );
	}
}

/**
 * TMR2 APIs
 */

void TMR2_Initialize( void ) {
	// Set TMR2 to the options selected in the User Interface

	// T2CKPS 1:1; T2OUTPS 1:16; TMR2ON off; 
	T2CON = 0x78;

	// PR2 249; 
	PR2 = 0xF9;

	// TMR2 0; 
	TMR2 = 0x00;

	// Clearing IF flag before enabling the interrupt.
	PIR1bits.TMR2IF = 0;

	// Enabling TMR2 interrupt.
	PIE1bits.TMR2IE = 1;
}

void TMR2_StartTimer( void ) {
	T2CONbits.TMR2ON = 1;
}

void TMR2_StopTimer( void ) {
	T2CONbits.TMR2ON = 0;
}

void TMR2_SetInterruptCallback( TMR_InterruptCallback_t callback ) {
	fnTmr2Callback = callback;
}

void TMR2_ISR( ) {
	PIR1bits.TMR2IF = 0;
	if ( fnTmr2Callback != NULL ) {
		fnTmr2Callback( );
	}
}
