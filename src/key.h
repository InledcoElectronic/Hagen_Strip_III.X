/* 
 * File:   key.h
 * Author: liruya
 *
 * Created on November 10, 2016, 11:49 AM
 */

#ifndef KEY_H
#define	KEY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "comman.h"

#define KEY_PORT        PORTC
#define KEY_MASK        0x01        //RC0
#define KEY_NONE        0x00
#define KEY_PRESS       0x01
#define KEY1            0x01

    //#define KEY_CONT_MASK       0x40
    //#define KEY_CONT_PRESS      (KEY_CONT_MASK | KEY_PRESS)
    //#define KEY_RELEASE_MASK    0x20
    //#define KEY_RELEASE         (KEY_RELEASE_MASK | KEY_PRESS)

    enum Key_State_t {
        KEY_PRESS_SHORT = 0, //短按
        KEY_PRESS_LONG, //长按(触发一次)
        KEY_PRESS_CONT, //持续按(持续触发)
        KEY_RELEASE_1
    }; //按键释放(长按或持续按状态释放)

    typedef void (*Key_OnStateChangedCallback_t)(uint8_t key, enum Key_State_t state);

    extern void KEY_Scan();
    extern void KEY_SetOnStateChangedCallback(Key_OnStateChangedCallback_t callback);

#ifdef	__cplusplus
}
#endif

#endif	/* KEY_H */

