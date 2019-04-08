/*    Name & E-mail: Ankush Vangari - avang001@ucr.edu
 *    Lab Section: 21
 *    Assignment: Lab # 2 Exercise # 2
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/io.h>

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
    unsigned char cntavail = 0x00; // Temporary variable to hold the value of A
    unsigned char mask = 0x01; // Temporary variable to hold the value of A
    while(1)
    {
		cntavail = 0;
        // 1) Read input
		for (mask = 0x01; mask <= 0x08; mask += mask) {
			tmpA = PINA & mask;
			if (tmpA == 0x00) {
				cntavail++;
			}
		}
        PORTC = cntavail;
    }
    return 0;
}


