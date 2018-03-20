/* 
 * File:   util.h
 * Author: liruya
 *
 * Created on 2018年3月19日, 下午1:49
 */

#ifndef UTIL_H
#define	UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
    
    void Util_Increase( uint16_t * const psrc, uint8_t delta, uint16_t max );
    void Util_Decrease( uint16_t * const psrc, uint8_t delta, uint16_t min );

#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

