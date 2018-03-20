#include "eeprom.h"

#define EEPROM_START_ADDR   0x7000          //eeprom start addr

static void EEPROM_WriteByte( uint16_t addr, uint8_t byte ) {
	uint8_t GIEBitValue = INTCONbits.GIE;

	NVMADRH = ( ( addr >> 8 ) & 0xFF );
	NVMADRL = ( addr & 0xFF );
	NVMDATL = byte;
	NVMCON1bits.NVMREGS = 1;
	NVMCON1bits.WREN = 1;
	INTCONbits.GIE = 0; // Disable interrupts
	NVMCON2 = 0x55;
	NVMCON2 = 0xAA;
	NVMCON1bits.WR = 1;
	// Wait for write to complete
	while ( NVMCON1bits.WR );

	NVMCON1bits.WREN = 0;
	INTCONbits.GIE = GIEBitValue; // restore interrupt enable
}

static uint8_t EEPROM_ReadByte( uint16_t addr ) {
	NVMADRH = ( ( addr >> 8 ) & 0xFF );
	NVMADRL = ( addr & 0xFF );
	NVMCON1bits.NVMREGS = 1;
	NVMCON1bits.RD = 1;
	NOP( ); // NOPs may be required for latency at high frequencies
	NOP( );

	return ( NVMDATL );
}

/*********************************************************************
 * Function:        void EEPROM_Write(uint8_t addr, uint8_t byte)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void EEPROM_Write( uint8_t addr, uint8_t byte ) {
	EEPROM_WriteByte( EEPROM_START_ADDR + addr, byte );
}

/*********************************************************************
 * Function:        uint8_t EEPROM_Read(uint8_t addr)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
uint8_t EEPROM_Read( uint8_t addr ) {
	return EEPROM_ReadByte( EEPROM_START_ADDR + addr );
}

/*********************************************************************
 * Function:        void EEPROM_WriteBuffer(uint8_t addr, const uint8_t * const pbuf, uint8_t len )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void EEPROM_WriteBuffer( uint8_t addr, const uint8_t * const pbuf, uint8_t len ) {
	for ( uint8_t i = 0; i < len; i++ ) {
		EEPROM_Write( addr + i, *( pbuf + i ) );
	}
}

/*********************************************************************
 * Function:        void EEPROM_ReadBuffer( uint8_t addr, uint8_t * const pbuf, uint8_t len )
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        None
 * Note:            None
 ********************************************************************/
void EEPROM_ReadBuffer( uint8_t addr, uint8_t * const pbuf, uint8_t len ) {
	for ( uint8_t i = 0; i < len; i++ ) {
		*( pbuf + i ) = EEPROM_Read( addr + i );
	}
}
