/*    Partner 1 Name & E-mail: Ankush Vangari - avang001@ucr.edu
 *    Partner 2 Name & E-mail: Justin Luo - jluo036@ucr.edu
 *    Lab Section: 21
 *    Assignment: Lab # 3 Exercise # 4
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */
 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    unsigned char left = 0x00;
    unsigned char right = 0x00;
    unsigned char pd = 0x00;
    while (1)
    {
        left = PINA & 0xF0;
		right = PINA & 0x0F;

		PORTB = (left >> 4);
		PORTC = (right << 4);
    }
}

