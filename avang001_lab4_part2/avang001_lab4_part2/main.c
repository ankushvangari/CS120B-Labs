//3 State Machines
//PWM triggered on input

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "bit.h"
#include "scheduler.h"

#define BALL_SIZE 8
static unsigned char ball = 0;
static unsigned char ball_arr[BALL_SIZE] = {
	0x00,0x24,0x5a,0x3c,0x3c,0x5a,0x24,0x00
};

static unsigned char ballSelect = 0;
static unsigned char ballSelect_arr[BALL_SIZE] = {
	0x00,0x42,0x24,0x18,0x18,0x24,0x42,0x00
};

#define CONV_SIZE 4
static unsigned char convergeBall = 0;
static unsigned char convergeBall_arr[CONV_SIZE] = {
	0x81, 0x42, 0x24, 0x18
};

static unsigned char convergeBallSelect = 0;
static unsigned char convergeBallSelect_arr[CONV_SIZE] = {
	0x81, 0x42, 0x24, 0x18
};


void displayMatrixStart() {
	const unsigned short SOLID_PERIOD = 1500;
	const unsigned short CONVERGE_PERIOD = 1500;
	const unsigned short SOLID_STATE = 375;
	const unsigned short CONV_STATE = 375;
	static unsigned char i = 0;
	static unsigned short startTimer = 0;
	static unsigned char color_toggle = 0;
	switch (startMatrixSubstate) {
		case(Init_Start_SS):
		clearMatrix();
		startTimer = 0;
		color_toggle = 0;
		i = 0;
		startMatrixSubstate = Display_Converge_SS;
		break;
		case(Display_Converge_SS):
		if (startTimer >= SOLID_PERIOD) {
			startTimer = 0;
			i = 0;
			startMatrixSubstate = Display_Solid_SS;
			break;
		}
		if (startTimer % CONV_STATE == 0) {
			convergeBallSelect = convergeBallSelect_arr[i];
			convergeBall = convergeBall_arr[i];
			clearMatrix();
			transmit_data(B_SR, convergeBallSelect);
			if (color_toggle) {
				transmit_data(BOT_SR, convergeBall);
			}
			else {
				transmit_data(TOP_SR, convergeBall);
			}
			
			if (i < BALL_SIZE) {
				++i;
			}
			else {
				i = 0;
			}
		}
		++startTimer;
		startMatrixSubstate = Display_Converge_SS;
		break;
		case(Display_Solid_SS):
		if (startTimer >= CONVERGE_PERIOD) {
			startTimer = 0;
			i = 0;
			color_toggle = ~color_toggle;
			startMatrixSubstate = Display_Converge_SS;
			break;
		}
		if (startTimer <= SOLID_STATE ||
		(startTimer >= SOLID_STATE * 2 &&
		startTimer <= SOLID_STATE * 3)) {
			ballSelect = ballSelect_arr[i];
			ball = ball_arr[i];
			clearMatrix();
			transmit_data(B_SR, ballSelect);
			if (color_toggle) {
				transmit_data(BOT_SR, ball);
			}
			else {
				transmit_data(TOP_SR, ball);
			}
			
			if (i < BALL_SIZE) {
				++i;
			}
			else {
				i = 0;
			}
		}
		else {
			transmit_data(B_SR, 0x00);
		}
		++startTimer;
		startMatrixSubstate = Display_Solid_SS;
		break;
		default:
		startMatrixSubstate = Init_Start_SS;
		break;
	}
}




void transmit_data ( unsigned char data ) {
	// for each bit of data
	// Set SRCLR to 1 allowing data to be set
	// Also clear SRCLK in preparation of sending data
	// set SER = next bit of data to be sent.
	// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
	// end for each bit of data
	// set RCLK = 1. Rising edge copies data from the "Shift" register to the "Storage"
	register
	// clears all lines in preparation of a new transmission
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;

	static task task2, task1; //task3;
	task *tasks[] = { &task2, &task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	//Task 1
	task2.state = -1; //task init state
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &SM1_Tick;


	task1.state = -1; //task init state
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &inputTick;
	//Task 2


	//ADC_init();
	TimerSet(50);
	TimerOn();
	PWM_on();

	unsigned char i;
	while (1)
	{
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 50;
		}
		while(!TimerFlag) { }
		TimerFlag = 0;
	}
}