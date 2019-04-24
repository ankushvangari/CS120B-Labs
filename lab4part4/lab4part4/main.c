/*
 *
 * Created: 4/11/2019 9:54:55 PM
 * Author : 
 */ 

#include <avr/io.h>

enum state {init, waitP, waitPR, waitY, open, lock} curr;

void tick() {
	switch(curr) {
		case init:
			curr = waitP;
			break;
		case waitP:
			if (PINA & 0x80) {
				curr = lock;
			} else if (PINA == 0x04) {
				curr = waitPR;
			} else {
				curr = waitP;
			}
			break;
		case waitPR:
			if (PINA & 0x80) {
				curr = lock;
			} else if (PINA == 0x00) {
				curr = waitY;
			} else if (PINA == 0x04) {
				curr = waitPR;
			} else {
				curr = waitP;
			}
			break;
		case waitY:
			if (PINA & 0x80) {
				curr = lock;
			} else if (PINA == 0x02) {
				if (PORTB == 0x00) {
					curr = open;
				} else {
					curr = lock;
				}
			} else if (PINA == 0x00) {
				curr = waitY;
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
			break;
		case lock:
			if (PINA & 0x80) {
				curr = lock;
			} else {
				curr = waitP;
			}
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
		case waitY:
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
	PORTC =  curr;
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