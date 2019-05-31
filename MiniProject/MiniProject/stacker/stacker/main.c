#include <avr/io.h>
#include <avr/interrupt.h>
#include "ledmatrix7219d88.c"
#include "max7219.c"

//TIMER FUNCTIONS
#pragma region Timer
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
#pragma endregion Timer
//GLOBAL VARIABLES
#pragma region GlobalVariables
#define MAX_LEVEL 32

unsigned char LETTER_L[8] = {0x00,0x01,0x01,0x01,0x01,0x7f,0x7f,0x00};
unsigned char LETTER_O[8] = {0x00,0x3e,0x7f,0x41,0x41,0x7f,0x3e,0x00};
unsigned char LETTER_S[8] = {0x00,0x26,0x6f,0x49,0x49,0x7b,0x32,0x00};
unsigned char LETTER_E[8] = {0x00,0x41,0x49,0x49,0x49,0x7f,0x7f,0x00};
unsigned char LETTER_W[8] = {0xff,0xff,0x06,0x0c,0x06,0xff,0xff,0x00};
unsigned char LETTER_I[8] = {0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00};
unsigned char LETTER_N[8] = {0x7f,0x7f,0x0c,0x18,0x30,0x7f,0x7f,0x00};
unsigned char LETTER_T[8] = {0x00,0x60,0x40,0x7f,0x7f,0x40,0x60,0x00};
unsigned char LETTER_A[8] = {0x00,0x3f,0x7f,0x48,0x48,0x7f,0x3f,0x00};
unsigned char LETTER_C[8] = {0x00,0x22,0x63,0x41,0x41,0x7f,0x3e,0x00};
unsigned char LETTER_K[8] = {0x00,0x41,0x63,0x3e,0x1c,0x7f,0x7f,0x00};
unsigned char LETTER_R[8] = {0x00,0x31,0x7b,0x4e,0x4c,0x7f,0x7f,0x00};
unsigned char BLANK[8]    = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char grid[MAX_LEVEL];
unsigned char speeds[MAX_LEVEL] = {10,10,10,10,10,11,11,11,11,9,9,9,9,8,8,8,8,7,7,7,7,6,6,6,6,6,6,5,5,5,5,4}; //In terms of a 10ms Period
//unsigned char speeds[MAX_LEVEL] = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20}; //TESTER SPEED
unsigned char sizes[MAX_LEVEL] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1};
unsigned char row_start[3] = {0x08,0x18,0x38};
unsigned char level, size, row, blink_row, blink_flag;
unsigned char move, win, lose, blink, start;
unsigned char play_button, reset_button;
#pragma endregion GlobalVariables
//GLOBAL FUNCTIONS
#pragma region Functions
unsigned char checkHitsAndGetSize(unsigned char row, unsigned char lvl){
	unsigned char count = 0;
	unsigned char temp = row & grid[level - 1];
	
	if(lvl == 0){
		return 3;
	}
	
	while(temp != 0x00){
		if((temp & 0x01) == 0x01){
			count++;
		}
		temp = temp >> 1;
	}

	switch(count){
		case 0:
		case 1:
		return count;
		break;
		case 2:
		if(lvl >= 25){
			blink_flag = 1;
			return 1;
			}else{
			blink_flag = 0;
			return 2;
		}
		break;
		case 3:
		if(lvl >= 14){
			blink_flag = 1;
			return 2;
			}else{
			blink_flag = 0;
			return 3;
		}
		break;
		default:
		break;
	}

	return count;
}
void setGridRow(unsigned char row, unsigned char lvl){
	if(lvl > 0){
		grid[lvl] = row & grid[lvl - 1];
	}else{
		grid[lvl] = row;
	}
}
void clearGrid(){
	for(unsigned char i = 0; i < MAX_LEVEL; i++){
		grid[i] = 0x00;
	}
}
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
#pragma endregion Functions
//STATE MACHINES
#pragma region StateMachines
enum GameLogicSM{GL_Reset, GL_Start,GL_Wait, GL_Move, GL_Pressed, GL_Check, GL_Blink, GL_Win, GL_Lose}GLSTATE;
void GL_Tick(){
	unsigned char new_size;

	switch(GLSTATE){ //STATE TRANSITIONS
		case GL_Reset:
		if(reset_button){
			GLSTATE = GL_Reset;
		}else{
			GLSTATE = GL_Start;
		}
		break;
		case GL_Start:
		GLSTATE = (play_button)? GL_Wait: GL_Start;
		if(play_button){
			GLSTATE = GL_Wait;
			start = 0;
		}
		break;
		case GL_Wait:
		if(reset_button){
			GLSTATE = GL_Reset;
		}else if(play_button){
			GLSTATE = GL_Wait;
		}else{
			GLSTATE = GL_Move;
		}
		break;
		case GL_Move:
		if(reset_button){
			move = 0;
			GLSTATE = GL_Reset;
		}else if(play_button){
			move = 0;
			GLSTATE = GL_Pressed;
		}else{
			GLSTATE = GL_Move;
		}
		break;
		case GL_Pressed:
		if(reset_button){
			GLSTATE = GL_Reset;
		}else if(play_button){
			GLSTATE = GL_Pressed;
		}else{
			GLSTATE = GL_Check;
		}
		break;
		case GL_Check:
		if(reset_button){
			GLSTATE = GL_Reset;
		}else{
			new_size = checkHitsAndGetSize(row,level);

			setGridRow(row,level);
			displayColumn(level,grid[level]);

			if(new_size <= 0){
				displayColumn(level,row);
				GLSTATE = GL_Lose;
			}else{
				level++;
				if(level == MAX_LEVEL){
					GLSTATE = GL_Win;
				}else if(new_size == size || blink_flag){
					GLSTATE = GL_Move;
					size = new_size;
					row = row_start[size - 1];
				}else{
					GLSTATE = GL_Blink;
					blink_row = row;
					size = new_size;
					row = row_start[size - 1];
				}
			}
		}
		break;
		case GL_Blink:
		if(reset_button){
			GLSTATE = GL_Reset;
		}
		else{
			GLSTATE = (blink)? GL_Blink: GL_Move;
		}
		break;
		case GL_Win:
		GLSTATE = (reset_button)? GL_Reset : GL_Win;
		break;
		case GL_Lose:
		GLSTATE = (reset_button)? GL_Reset : GL_Lose;
		break;
		default:
		GLSTATE = GL_Reset;
		break;
	}
	switch(GLSTATE){ //STATE ACTIONS
		case GL_Reset:
		break;
		case GL_Start:
		clearGrid();
		level = 0;
		size = sizes[level];
		row = row_start[size - 1];
		blink_row = 0x00;
		blink_flag = 0;
		move = 0;
		win = 0;
		lose = 0;
		blink = 0;
		start = 1;
		break;
		case GL_Wait:
		for(unsigned char i = 0; i < MAX_LEVEL/8; i++){
			ledmatrix7219d88_resetmatrix(i);
		}
		break;
		case GL_Move:
		move = 1;
		break;
		case GL_Pressed:
		break;
		case GL_Check:
		break;
		case GL_Blink:
		blink = 1;
		break;
		case GL_Win:
		win = 1;
		break;
		case GL_Lose:
		lose = 1;
		break;
		default:
		break;
	}
}

enum MovementSM{MM_Start, MM_Right, MM_Left}MMSTATE;
void MM_Tick(){
	static unsigned char cnt;
	switch(MMSTATE){ //STATE TRANSITIONS
		case MM_Start:
		MMSTATE = move? MM_Right: MM_Start;
		break;
		case MM_Right:
		if(!move){
			MMSTATE = MM_Start;
		}else if(row & 0x01){
			MMSTATE = MM_Left;
			cnt = 0;
		}else{
			MMSTATE = MM_Right;
		}
		break;
		case MM_Left:
		if(!move){
			MMSTATE = MM_Start;
		}else if(row & 0x80){
			MMSTATE = MM_Right;
			cnt = 0;
		}else{
			MMSTATE = MM_Left;
		}
		break;
		default:
		MMSTATE = MM_Start;
		break;
	}
	switch(MMSTATE){ //STATE ACTIONS
		case MM_Start:
		cnt = 0;
		break;
		case MM_Right:
		if(move){
			if(cnt % speeds[level] == 0){
				row = row >> 1;
				displayColumn(level,row);
			}
			cnt++;
		}
		break;
		case MM_Left:
		if(move){
			if(cnt % speeds[level] == 0){
				row = row << 1;
				displayColumn(level,row);
			}
			cnt++;
		}
		break;
		default:
		break;
	}
}

enum BlinkSM{BL_Start, BL_Blink}BLSTATE;
void BL_Tick(){
	static unsigned char counter;

	switch(BLSTATE){ //STATE TRANSITIONS
		case BL_Start:
		BLSTATE = (blink)? BL_Blink : BL_Start;
		break;
		case BL_Blink:
		BLSTATE = (blink)? BL_Blink : BL_Start;
		break;
		default:
		break;
	}
	switch(BLSTATE){ //STATE ACTIONS
		case BL_Start:
		counter = 0;
		break;
		case BL_Blink:
		if(counter >= 5){
			blink = 0;
		}

		if(counter % 2 == 0){
			displayColumn(level - 1, blink_row);
		}else{
			displayColumn(level - 1, grid[level-1]);
		}
		counter++;
		break;
		default:
		break;
	}

}

enum WinSM{WN_Wait,WN_Display,WN_Blink}WNSTATE;
void WN_Tick(){
	static unsigned char counter;

	switch(WNSTATE){ //STATE TRANSITIONS
		case WN_Wait:
		WNSTATE = (win)? WN_Display: WN_Wait;
		break;
		case WN_Display:
		if(!win){
			WNSTATE = WN_Wait;
		}else if(counter > 4){
			WNSTATE = WN_Blink;
			counter = 0;
		}else{
			WNSTATE = WN_Display;
		}
		break;
		case WN_Blink:
		WNSTATE = (win)? WN_Blink: WN_Wait;
		break;
		default:
		break;
	}
	switch(WNSTATE){ //STATE ACTIONS
		case WN_Wait:
		counter = 0;
		break;
		case WN_Display:
		counter++;
		switch(counter){
			case 1:
			ledmatrix7219d88_setmatrix(0,LETTER_W);
			break;
			case 2:
			ledmatrix7219d88_setmatrix(1,LETTER_W);
			ledmatrix7219d88_setmatrix(0,LETTER_I);
			break;
			case 3:
			ledmatrix7219d88_setmatrix(2,LETTER_W);
			ledmatrix7219d88_setmatrix(1,LETTER_I);
			ledmatrix7219d88_setmatrix(0,LETTER_N);
			break;
			case 4:
			ledmatrix7219d88_setmatrix(3,LETTER_W);
			ledmatrix7219d88_setmatrix(2,LETTER_I);
			ledmatrix7219d88_setmatrix(1,LETTER_N);
			ledmatrix7219d88_setmatrix(0,BLANK);
			break;
			default:
			break;
		}
		break;
		case WN_Blink:
		if(counter %2 == 0){
			ledmatrix7219d88_resetmatrix(0);
			ledmatrix7219d88_resetmatrix(1);
			ledmatrix7219d88_resetmatrix(2);
			ledmatrix7219d88_resetmatrix(3);
			}else{
			ledmatrix7219d88_setmatrix(3,LETTER_W);
			ledmatrix7219d88_setmatrix(2,LETTER_I);
			ledmatrix7219d88_setmatrix(1,LETTER_N);
			ledmatrix7219d88_resetmatrix(0);
		}
		counter++;
		break;
		default:
		break;
	}
}

enum LoseSM{LS_Wait,LS_Display,LS_Blink}LSSTATE;
void LS_Tick(){
	static unsigned char counter;

	switch(LSSTATE){ //STATE TRANSITIONS
		case LS_Wait:
		LSSTATE = (lose)? LS_Display: LS_Wait;
		break;
		case LS_Display:
		if(!lose){
			LSSTATE = LS_Wait;
		}else if(counter > 4){
			LSSTATE = LS_Blink;
			counter = 0;
		}else{
			LSSTATE = LS_Display;
		}
		break;
		case LS_Blink:
		LSSTATE = (lose)? LS_Blink: LS_Wait;
		break;
		default:
		break;
	}
	switch(LSSTATE){ //STATE ACTIONS
		case LS_Wait:
		counter = 0;
		break;
		case LS_Display:
		counter++;
		switch(counter){
			case 1:
			ledmatrix7219d88_setmatrix(0,LETTER_L);
			break;
			case 2:
			ledmatrix7219d88_setmatrix(1,LETTER_L);
			ledmatrix7219d88_setmatrix(0,LETTER_O);
			break;
			case 3:
			ledmatrix7219d88_setmatrix(2,LETTER_L);
			ledmatrix7219d88_setmatrix(1,LETTER_O);
			ledmatrix7219d88_setmatrix(0,LETTER_S);
			break;
			case 4:
			ledmatrix7219d88_setmatrix(3,LETTER_L);
			ledmatrix7219d88_setmatrix(2,LETTER_O);
			ledmatrix7219d88_setmatrix(1,LETTER_S);
			ledmatrix7219d88_setmatrix(0,LETTER_E);
			break;
			default:
			break;
		}
		break;
		case LS_Blink:
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
		break;
		default:
		break;
	}
}

enum StartSM{ST_Wait,ST_Display}STSTATE;
void ST_Tick(){
	static unsigned char counter;

	switch(STSTATE){ //STATE TRANSITIONS
		case ST_Wait:
		STSTATE = (start)? ST_Display:ST_Wait;
		break;
		case ST_Display:
		STSTATE = (start)? ST_Display:ST_Wait;
		break;
	}
	switch(STSTATE){ //STATE ACTIONS
		case ST_Wait:
		counter = 0;
		break;
		case ST_Display:
		counter++;
		switch(counter){
			case 1:
			ledmatrix7219d88_setmatrix(3,BLANK);
			ledmatrix7219d88_setmatrix(2,BLANK);
			ledmatrix7219d88_setmatrix(1,BLANK);
			ledmatrix7219d88_setmatrix(0,LETTER_S);
			break;
			case 2:
			ledmatrix7219d88_setmatrix(3,BLANK);
			ledmatrix7219d88_setmatrix(2,BLANK);
			ledmatrix7219d88_setmatrix(1,LETTER_S);
			ledmatrix7219d88_setmatrix(0,LETTER_T);
			break;
			case 3:
			ledmatrix7219d88_setmatrix(3,BLANK);
			ledmatrix7219d88_setmatrix(2,LETTER_S);
			ledmatrix7219d88_setmatrix(1,LETTER_T);
			ledmatrix7219d88_setmatrix(0,LETTER_A);
			break;
			case 4:
			ledmatrix7219d88_setmatrix(3,LETTER_S);
			ledmatrix7219d88_setmatrix(2,LETTER_T);
			ledmatrix7219d88_setmatrix(1,LETTER_A);
			ledmatrix7219d88_setmatrix(0,LETTER_C);
			break;
			case 5:
			ledmatrix7219d88_setmatrix(3,LETTER_T);
			ledmatrix7219d88_setmatrix(2,LETTER_A);
			ledmatrix7219d88_setmatrix(1,LETTER_C);
			ledmatrix7219d88_setmatrix(0,LETTER_K);
			break;
			case 6:
			ledmatrix7219d88_setmatrix(3,LETTER_A);
			ledmatrix7219d88_setmatrix(2,LETTER_C);
			ledmatrix7219d88_setmatrix(1,LETTER_K);
			ledmatrix7219d88_setmatrix(0,LETTER_E);
			break;
			case 7:
			ledmatrix7219d88_setmatrix(3,LETTER_C);
			ledmatrix7219d88_setmatrix(2,LETTER_K);
			ledmatrix7219d88_setmatrix(1,LETTER_E);
			ledmatrix7219d88_setmatrix(0,LETTER_R);
			break;
			case 8:
			ledmatrix7219d88_setmatrix(3,LETTER_K);
			ledmatrix7219d88_setmatrix(2,LETTER_E);
			ledmatrix7219d88_setmatrix(1,LETTER_R);
			ledmatrix7219d88_setmatrix(0,BLANK);
			break;
			case 9:
			ledmatrix7219d88_setmatrix(3,LETTER_E);
			ledmatrix7219d88_setmatrix(2,LETTER_R);
			ledmatrix7219d88_setmatrix(1,BLANK);
			ledmatrix7219d88_setmatrix(0,BLANK);
			break;
			case 10:
			ledmatrix7219d88_setmatrix(3,LETTER_R);
			ledmatrix7219d88_setmatrix(2,BLANK);
			ledmatrix7219d88_setmatrix(1,BLANK);
			ledmatrix7219d88_setmatrix(0,BLANK);
			break;
			case 11:
			ledmatrix7219d88_setmatrix(3,BLANK);
			ledmatrix7219d88_setmatrix(2,BLANK);
			ledmatrix7219d88_setmatrix(1,BLANK);
			ledmatrix7219d88_setmatrix(0,BLANK);
			counter = 0;
			break;
			default:
			break;
		}
		break;
	}
}
#pragma endregion StateMachines

int main(void)
{
	//INPUT PORT
	DDRA = 0x00; PORTA = 0xFF;
	//OUTPUT PORT
	DDRD = 0xFF; PORTD = 0x00;
	//DEBUG PORTS
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;

	//INITIALIZE LED MATRICIES AND SET INTENSITY
	ledmatrix7219d88_init();
	for(unsigned char i = 0; i < MAX_LEVEL/8; i++){
		ledmatrix7219d88_setintensity(i,2);
	}

	//STATE MACHINE TIMERS
	unsigned long timerPeriod = 10;
	unsigned long GL_elapsedTime = 10;
	unsigned long MM_elapsedTime = 10;
	unsigned long BL_elapsedTime = 250;
	unsigned long WN_elapsedTime = 1000;
	unsigned long LS_elapsedTime = 1000;
	unsigned long ST_elapsedTime = 750;
	
	//TIMER DEFINITIONS
	TimerSet(timerPeriod);
	TimerOn();

	//INTIALIZING STATEMACHINES
	GLSTATE = GL_Reset;
	MMSTATE = MM_Start;
	BLSTATE = BL_Start;
	WNSTATE = WN_Wait;
	LSSTATE = LS_Wait;
	STSTATE = ST_Wait;

	while(1){
		play_button = ~PINA & 0x01;
		reset_button  = ~PINA & 0x02;

		if(GL_elapsedTime >= 10){ //10ms period
			GL_Tick();
			GL_elapsedTime = 0;
		}
		if(MM_elapsedTime >= 10){ //10ms period
			MM_Tick();
			MM_elapsedTime = 0;
		}
		if(BL_elapsedTime >= 250){ //250ms period
			BL_Tick();
			BL_elapsedTime = 0;
		}
		if(WN_elapsedTime >= 1000){ //1000ms period
			WN_Tick();
			WN_elapsedTime = 0;
		}
		if(LS_elapsedTime >= 1000){ //1000ms period
			LS_Tick();
			LS_elapsedTime = 0;
		}
		if(ST_elapsedTime >= 750){ //750ms period
			ST_Tick();
			ST_elapsedTime = 0;
		}

		while(!TimerFlag);
		TimerFlag = 0;
		
		//DEBUG OUTs
		PORTC = GLSTATE;

		GL_elapsedTime += timerPeriod;
		MM_elapsedTime += timerPeriod;
		BL_elapsedTime += timerPeriod;
		WN_elapsedTime += timerPeriod;
		LS_elapsedTime += timerPeriod;
		ST_elapsedTime += timerPeriod;
	}
}
