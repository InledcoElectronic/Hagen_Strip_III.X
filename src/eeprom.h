/* 
 * File:   eeprom.h
 * Author: liruya
 *
 * Created on 2016?11?2?, ??9:27
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"

    extern void EEPROM_Write(uint8_t addr, uint8_t byte);
    extern uint8_t EEPROM_Read(uint8_t addr);
    extern void EEPROM_WriteBuffer(uint8_t addr, const uint8_t * const pbuf, uint8_t len);
    extern void EEPROM_ReadBuffer(uint8_t addr, uint8_t * const pbuf, uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

