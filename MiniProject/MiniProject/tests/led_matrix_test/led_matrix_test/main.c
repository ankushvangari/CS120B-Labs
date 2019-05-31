#include <avr/io.h>
#include "shift.h"


int main(void)
{
    /* Replace with your application code */
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	unsigned char pd = 0x00;
    while (1) 
    {
        shregwrite(0);
		shregwrite(255);
    }
}