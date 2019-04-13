/*
 * avang001_lab4_part3.c
 *
 * Created: 4/10/2019 2:47:20 PM
 * Author : Ankush Vangari
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
	DDRC = 0xFF; PORTC = 0x07; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char pd = 0x00;
	unsigned char pressedHash = 0x00;
	unsigned char pressedY = 0x00;
	unsigned char releasedHash = 0x00;
	unsigned char releasedY = 0x00;
	/* Replace with your application code */
	while (1)
	{
		if (releasedY > 0x00 && releasedHash > 0x00) {
			pd = 0x01;
			releasedHash = 0x00;
			releasedY = 0x00;
		}
		else if ((PINA & 0x02) > 0x00) {
			pressedY = 0x01;
		}
		else if ((PINA & 0x02) == 0x00 && pressedY > 0x00) {
			releasedY = 0x01;
			pressedY = 0x00;
		}
		else if ((PINA & 0x80) > 0x00) {
			pd = 0x00;
		}
		else if ((PINA & 0x04) > 0x00) {
			pressedHash = 0x01;
		}
		else if ((PINA & 0x04) == 0x00 && pressedHash > 0x00) {
			releasedHash = 0x01;
			pressedHash = 0x00;
		}
		else if (((PINA & 0x01) > 0x00 || (PINA & 0x02) > 0x00 || (PINA & 0x03) > 0x00) && pd > 0x00 ) {
			pd = 0x00;
		}
		
		PORTC = pd;
		
	}
}

