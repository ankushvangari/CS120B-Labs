/*    Partner 1 Name & E-mail: Ankush Vangari - avang001@ucr.edu
 *    Partner 2 Name & E-mail: Justin Luo - jluo036@ucr.edu
 *    Lab Section: 21
 *    Assignment: Lab # 3 Exercise # 3
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
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
    unsigned char pd = 0x00;
    while (1)
    {
        pd = ((PINA & 0x0F) > 0x0C) ? pd | 0x01 : pd & 0xFE;
        pd = ((PINA & 0x0F) > 0x09) ? pd | 0x02 : pd & 0xFD;
        pd = ((PINA & 0x0F) > 0x06) ? pd | 0x04 : pd & 0xFB;
        pd = ((PINA & 0x0F) > 0x04) ? pd | 0x08 : pd & 0xF7;
        pd = ((PINA & 0x0F) > 0x02) ? pd | 0x10 : pd & 0xEF;
        pd = ((PINA & 0x0F) > 0x00) ? pd | 0x20 : pd & 0xDF;
        pd = ((PINA & 0x0F) < 0x05) ? pd | 0x40 : pd & 0xBF;
        pd = ((PINA & 0x70) == 0x30) ? pd | 0x80 : pd & 0x7F;

        PORTC = pd;
    }
}

