/*
 * avang001_lab4_part2.c
 *
 * Created: 4/10/2019 12:04:08 PM
 * Author : Ankush Vangari
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
	DDRC = 0xFF; PORTC = 0x07; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char pd = 0x07;
	unsigned char pressedUp = 0x00;
	unsigned char pressedDown = 0x00;
	/* Replace with your application code */
	while (1)
	{
		if ((PINA & 0x01) == 0x00 && (PINA & 0x02) == 0x00 && pressedUp > 0x00 && pressedDown > 0x00) {
			pd = 0x00;
		}
		else if ((PINA & 0x01) == 0x00 && pressedUp > 0x00) {
			if (pd < 9)
				pd++;
			pressedUp = 0x00;
		}
		else if ((PINA & 0x02) == 0x00 && pressedDown > 0x00) {
			if (pd > 0)
				pd--;
			pressedDown = 0x00;
		}
		else if ((PINA & 0x01) > 0x00) {
			pressedUp = 0x01;
		}
		else if ((PINA & 0x02) > 0x00) {
			pressedDown = 0x01;
		}
		
		PORTC = pd;
		
	}
}

