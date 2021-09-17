#include "mydelay.h"


void delay_10xN (uint8 N) { //function to overcome the 32ms maximum delay possible by _delay_ms_ function on 8mhz clock
	for(int i = 0; i < N; i++) {
		_delay_ms(10);
	}
}
