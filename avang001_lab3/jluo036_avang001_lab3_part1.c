/*    Partner 1 Name & E-mail: Ankush Vangari - avang001@ucr.edu
 *    Partner 2 Name & E-mail: Justin Luo - jluo036@ucr.edu
 *    Lab Section: 21
 *    Assignment: Lab # 3 Exercise # 1
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
    DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs, initialize to 1s
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    unsigned char numOnes = 0x00;
    unsigned char mask = 0x00;
    unsigned char tmp = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		numOnes = 0x00;
		for (mask = 0x01; mask <= 0x80 && mask != 0x00; mask+=mask) {
    		tmp = PINA & mask;
    		if (tmp > 0x00) {
        		numOnes++;
			}
			tmp = PINB & mask;
			if (tmp > 0x00) {
    			numOnes++;
			}
		}

		PORTC = numOnes;
    }
}

