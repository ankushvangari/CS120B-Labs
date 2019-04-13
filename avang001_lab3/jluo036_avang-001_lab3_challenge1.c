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
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0x00; PORTC = 0xFF; // Configure port A's 8 pins as inputs
    DDRD = 0xFF; PORTD = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
    unsigned char pd = 0x00; // Temporary variable to hold the value of A
    unsigned char mask = 0x01; // Temporary variable to hold the value of A
    while(1)
    {
        //1) Read input
        pd = pd & 0x00;
		pd = (PINA + PINB + PINC >= 0x8C) ? pd | 0x01 : pd | 0x00;
		pd = (PINA - PINC > 80 || PINC - PINA > 80) ? pd | 0x02 : pd | 0x00;
		// Challenge activity - I am dividing each weight input into 4 different categories to represent each PIN in 2 bits for an approximate weight.
		pd = pd | ((PINA / 0x40) << 6);
		pd = pd | ((PINB / 0x40) << 4);
		pd = pd | ((PINC / 0x40) << 2);
        PORTD = pd;
	}
    return 0;
}


