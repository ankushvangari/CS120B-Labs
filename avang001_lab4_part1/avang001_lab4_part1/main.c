/*
 * avang001_lab4_part1.c
 *
 * Created: 4/10/2019 11:54:07 AM
 * Author : Ankush Vangari
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char pd = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		pd = ((PINA & 0x01) > 0x00) ? 0x02 : 0x01;
		PORTB = pd;
			
    }
}

