#include "util.h"

void Util_Increase( uint16_t * const psrc, uint8_t delta, uint16_t max ) {
	if ( ( *psrc ) + delta < max ) {
		( *psrc ) += delta;
	} else {
		( *psrc ) = max;
	}
}

void Util_Decrease( uint16_t * const psrc, uint8_t delta, uint16_t min ) {
	if ( ( *psrc ) > min + delta ) {
		( *psrc ) -= delta;
	} else {
		( *psrc ) = min;
	}
}
