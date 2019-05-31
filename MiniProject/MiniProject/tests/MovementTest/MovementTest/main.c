#include <avr/io.h>
#include "ledmatrix7219d88.c"
#include "max7219.c"

unsigned char curr_row,level,cnt, speed, lose,counter = 0;

unsigned char LETTER_L[8] = {0x00,0x01,0x01,0x01,0x01,0x7f,0x7f,0x00};
unsigned char LETTER_O[8] = {0x00,0x3e,0x7f,0x41,0x41,0x7f,0x3e,0x00};
unsigned char LETTER_S[8] = {0x00,0x26,0x6f,0x49,0x49,0x7b,0x32,0x00};
unsigned char LETTER_E[8] = {0x00,0x41,0x49,0x49,0x49,0x7f,0x7f,0x00};
unsigned char LETTER_W[8] = {0xff,0xff,0x06,0x0c,0x06,0xff,0xff,0x00};
unsigned char LETTER_I[8] = {0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00};
unsigned char LETTER_N[8] = {0x7f,0x7f,0x0c,0x18,0x30,0x7f,0x7f,0x00};


//,0x
#pragma region TimerCode
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
#pragma endregion TimerCode

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
void displayColumn(unsigned char col, unsigned char value){

	unsigned char start_loc = 8*col % 64;
	unsigned char matrix_num = 8*col / 64;


	for(int i = 0; i < 8; i++){
		ledmatrix7219d88_setled(matrix_num,start_loc + i, GetBit(value,i));
	}

}

enum MovementSM{Right,Left}MOVEMENTSTATE;
enum LoseSM{ LSWait, Display,Blink}LOSESTATE;

void MovementSM_Tick(){
	unsigned char button = ~PINA & 0x01;

	switch(MOVEMENTSTATE){ //Transitions
		case Right:
			if(curr_row & 0x01){
				MOVEMENTSTATE = Left;
				cnt = 0;
			}else{
				MOVEMENTSTATE = Right;
			}
		break;
		case Left:
			if(curr_row & 0x80){
				MOVEMENTSTATE = Right;
				cnt = 0;
				}else{
				MOVEMENTSTATE = Left;
			}
		break;
		default:
			MOVEMENTSTATE = Right;
		break;
	}
	switch(MOVEMENTSTATE){ //Actions
		case Right:
			level = (button)? level + 1: level;

			if(cnt % speed == 0){
				curr_row = curr_row >> 1;
				displayColumn(level,curr_row);
			}
			cnt++;
		break;
		case Left:
			level = (button)? level + 1: level;

			if(cnt % speed == 0){
				curr_row = curr_row << 1;
				displayColumn(level,curr_row);
			}
			cnt++;
		break;
		default:
		break;
	}
}

void LoseSM_Tick(){
	static unsigned short int_cntr;

	switch(LOSESTATE){ //Transitions
		case LSWait:
			LOSESTATE = (lose)? Display:LSWait;
		break;
		case Display:
			if(!lose){
				LOSESTATE = LSWait;
			}else if(counter > 4){
				int_cntr = 0;
				LOSESTATE = Blink;
			}else{
				LOSESTATE = Display;
			}
		break;
		case Blink:
			LOSESTATE = (lose)? Blink:LSWait;
		break;
		default:;
	}

	switch(LOSESTATE){ //Actions
		case LSWait:
		counter = 0;
		int_cntr = 0;
		break;
		case Display:
			if(int_cntr % 75 == 0){
				switch(counter){
					case 0:
						ledmatrix7219d88_setmatrix(3,LETTER_L);
						break;
					case 1:
						ledmatrix7219d88_setmatrix(2,LETTER_O);
						break;
					case 2:
						ledmatrix7219d88_setmatrix(1,LETTER_S);
						break;
					case 3:
						ledmatrix7219d88_setmatrix(0,LETTER_E);
						break;
					default:
					break;
				}
				counter++;
			}
			int_cntr++;
		break;
		case Blink:
		if(int_cntr % 100 == 0){
			if(counter %2 == 0){
				ledmatrix7219d88_resetmatrix(0);
				ledmatrix7219d88_resetmatrix(1);
				ledmatrix7219d88_resetmatrix(2);
				ledmatrix7219d88_resetmatrix(3);
			}else{
				ledmatrix7219d88_setmatrix(3,LETTER_L);
				ledmatrix7219d88_setmatrix(2,LETTER_O);
				ledmatrix7219d88_setmatrix(1,LETTER_S);
				ledmatrix7219d88_setmatrix(0,LETTER_E);
			}
			counter++;
			}
			int_cntr++;

		break;
		default:;
	}
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;

	ledmatrix7219d88_init();
	for(unsigned char i = 0; i < 4; i++){
		ledmatrix7219d88_setintensity(i,1);
	}

	TimerSet(10);
	TimerOn();

    unsigned char start = 0x38;
	curr_row = start; level = 0; 
	cnt = 0; speed = 20;lose = 1;
	counter = 0;
	MOVEMENTSTATE = Right;
	LOSESTATE = LSWait;

	while(1){
		//MovementSM_Tick();
		LoseSM_Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}

}

