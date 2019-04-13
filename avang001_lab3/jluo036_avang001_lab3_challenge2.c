/*    Partner 1 Name & E-mail: Ankush Vangari - avang001@ucr.edu
 *    Partner 2 Name & E-mail: Justin Luo - jluo036@ucr.edu
 *    Lab Section: 21
 *    Assignment: Lab # 3 Exercise # 5
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */

#include <avr/io.h>
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}



int main(void)
{
	 DDRD = 0x00; PORTD = 0xFF;
	 DDRB = 0x00; PORTB = 0xFF;
	 DDRC = 0xFF; PORTC = 0x00;
     unsigned char temp = 0x00;
	 unsigned char temp1 = 0x00;
    while (1) 
    {
		temp = ((PIND << 1) | (PINB & 0x01));
		if(temp>=70){
			temp1 = SetBit(temp1, 1, 1);
			temp1 = SetBit(temp1, 2, 0);
			
			
		}else if (temp<70 && temp>5){
			temp1 = SetBit(temp1, 1, 0);
			temp1 = SetBit(temp1, 2, 1);
			
		}else{
			temp1 = SetBit(temp1, 1, 0);
			temp1 = SetBit(temp1, 2, 0);
		}
		PORTB = temp1;
			
    }
	return 0;
}

