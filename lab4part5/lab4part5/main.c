/*
 * dseet001_lab4_part5.c
 *
 * Created: 4/11/2019 10:14:36 PM
 * Author : 
 */ 

#include <avr/io.h>

enum state {init, waitP, waitPR, open, lock} curr;
unsigned char arr[] = {0x04, 0x01, 0x02, 0x01};
unsigned short pos = 0x00;

void tick() {
	switch(curr) {
		case init:
			curr = waitP;
			break;
		case waitP:
			if (PINA & 0x80) {
				curr = lock;
			} else if (PINA == arr[pos]) {
				if (pos == 0x03) {
					if (PORTB == 0x00) {
						curr = open;
					} else {
						curr = lock;
					}
				} else {
					curr = waitPR;
				}
			} else {
				curr = waitP;
			}
			break;
		case waitPR:
			if (PINA & 0x80) {
				curr = lock;
			} else if (PINA == 0x00) {
				pos++;
				curr = waitP;
			} else if (PINA == arr[pos]) {
				curr = waitPR;
			} else {
				curr = waitP;
			}
			break;
		case open:
			if (PINA & 0x80) {
				curr = lock;
			} else {
				curr = waitP;
			}
			pos = 0x00;
			break;
		case lock:
			if (PINA & 0x80) {
				curr = lock;
			} else {
				curr = waitP;
			}
			pos = 0x00;
			break;
		default:
			curr = init;
			break;
	}
	
	switch(curr) {
		case init:
			break;
		case waitP:
			break;
		case waitPR:
			break;
		case open:
			PORTB = 0x01;
			break;
		case lock:
			PORTB = 0x00;
			break;
		default:
			break;
	}
	PORTC = curr << 4;
	PORTC = (PORTC & 0xF0) | pos;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; //DDR to 00 for read, PORT to FF for read
	DDRB = 0xFF; PORTB = 0x00; //DDR to FF for write, PORT to FF for write
	DDRC = 0xFF; PORTC = 0x00; //DDR to FF for write, PORT to FF for write
	while (1)
	{
		tick();
	}
}
