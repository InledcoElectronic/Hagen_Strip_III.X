/* 
 * File:   ble.h
 * Author: Administrator
 *
 * Created on 2016?10?15?, ??3:04
 */

#ifndef BLE_H
#define	BLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"

    extern void BLE_Initialize();
    extern void BLE_Receive(uint8_t rcv);
    extern void BLE_DecodeData();
    extern bool BLE_ReadyToReceive();
    extern void BLE_StartReceive();
    extern bool BLE_ReceiveComplete();
    extern void BLE_StopReceive();

#ifdef	__cplusplus
}
#endif

#endif	/* BLE_H */

