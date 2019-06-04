#include <avr/io.h>0
#include "io.c"
#include "shift.h"
#include "timer.h"
#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

unsigned char i, b1, k, b2, b3, b4, next1 = 0x00, next2 = 0x00, next3 = 0x00, next4 = 0x00, level = 0x01, j, score = 49, highScore, c, threeDig2 = 0x20, threeDig = 0x20;
unsigned speed = 200;
const unsigned char tasksNum = 1;
const unsigned long int tasksPeriodGCD = 5;
task tasks[1];
enum LEDMatrixStates {initMatrix, MatrixIdle, MatrixNextLevel, stop};
enum ButtonStates {initButton, wait, onPress, onRelease};
unsigned char PLAY[4]={63, 207, 243, 252};
unsigned char S[3]={3, 7, 15};
char customChar[9] = {
	0x0A,
	0x0A,
	0x0A,
	0x00,
	0x11,
	0x1B,
	0x0E,
	0x00,
	'\0',
};
void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	unsigned char i;
	if(loc<8)
	{
		LCD_WriteCommand (0x40 + (loc*8));	/* Command 0x40 and onwards forces the device to point CGRAM address */
		for(i=0;i<8;i++)	/* Write 8 byte for generation of 1 character */
		LCD_WriteData(msg[i]);
	}
}
	
int LEDMatrixTick(int state) {
	highScore = eeprom_read_byte((uint8_t*)1);
	switch(state) { // State Transitions
		case initMatrix:
		state = MatrixIdle;
		break;
		case stop:
		break;
		case MatrixIdle:
		state = (next1 || next2 || next3 || next4) ? MatrixNextLevel : MatrixIdle;
		break;
		case MatrixNextLevel:
		state = MatrixIdle;
		next1 = 0x00;
		next2 = 0x00;
		next3 = 0x00;
		next4 = 0x00;
		if (speed > 75) speed -= 25;
		break;
		default:
		state = initMatrix;
		break;
	} // End State Transitions
	switch(state) { // State Actions
		case initMatrix:
		break;
		case stop:
		
		break;
		case MatrixIdle:
		j = rand() % 4;
		k = rand() % 3;
		for(i=0;i<8;i++) {
			ShRegWrite(PLAY[j]);
			ShRegWrite(S[k]<<i);
			delay_ms(speed);
			b1 = ~PINB & 0x01;
			b2 = ~PINB & 0x02;
			b3 = ~PINB & 0x04;
			b4 = ~PINB & 0x08;
			if ((next1 && !b1 && j == 0 && i >= 8-(k+2)) || (next2 && !b2 && j == 1 && i >= 8-(k+2)) || (next3 && !b3 && j == 2 && i >= 8-(k+2)) || (next4 && !b4 && j == 3 && i >= 8-(k+2))) {
				state = MatrixNextLevel;	
				score++;
				if (score >= 58) {
					if (score >= highScore && (threeDig2 == 0x20 || threeDig == 49)) {
						
						eeprom_update_byte((uint8_t*) 1, (uint8_t) 48);
						highScore = 48;
						threeDig2 = 49;
					}
					
					eeprom_update_byte((uint8_t*) 4, (uint8_t) 49);
					score = 48;
					threeDig = 49;
				}
				if (score > highScore && (threeDig2 == 0x20 || threeDig == 49)) {
					eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
					
					highScore = score;
				}
				if (score == 0) c = ' ';
				else c = '0';
				char str [11] = {'S', 'c', 'o', 'r', 'e', ':', ' ', threeDig, ' ', c, '\0'};
				str[8] = score;
				LCD_ClearScreen();
				LCD_DisplayString(1, str);
				LCD_Cursor(12);
				LCD_WriteData(0);
				char str2 [16] = {'H', 'i', 'g', 'h', ' ', 'S', 'c', 'o', 'r', 'e', ':', ' ', threeDig2, ' ', c, '\0'};
				str2[13] = highScore;
				LCD_Cursor(1);
				LCD_DisplayString(17, str2);
				break;
			}
			else if (b1)	next1 = 0x01;
			else if (b2)	next2 = 0x01;
			else if (b3)	next3 = 0x01;
			else if (b4)	next4 = 0x01;
			else if (next1 || next2 || next3 || next4 || i == 7) {
				state = stop;
				break;
			}
		}

		break;
		case MatrixNextLevel:
			//LCD_DisplayString(1, str);
		break;
	}	// End State Actions	
	
	return state;
}
//int ButtonTick(int state) {
	//switch(state) { // State Transitions
		//case initButton:
		//state = wait;
		//break;
		//case wait:
			//b1 = ~PINB & 0x01;
			//if (b1) state = onPress;
		//break;
		//case onPress:
			//b1 = ~PINB & 0x01;
			//if (!b1) state = onRelease;
		//break;
		//case onRelease:
			//next1 = 0x01;
			//state = wait;
		//break;
		//default:
		//state = initButton;
		//break;
	//} // End State Transitions	
	//
	//return state;
//}

int main(void)
{
	/* Replace with your application code */
	ShRegInit();
	DDRC = 0xFF; PORTC = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	DDRA = 0xFF; PORTA = 0X00;
	DDRD = 0XFF; PORTD = 0X00;
	if(eeprom_read_byte((uint8_t*)1) == 255) {
		eeprom_update_byte((uint8_t*)1, (uint8_t) 0);
	}
	if(eeprom_read_byte((uint8_t*)4) == 255) {
		eeprom_update_byte((uint8_t*)4, (uint8_t) 0);
	}
	
	highScore = eeprom_read_byte((uint8_t*)1);
	threeDig2 = eeprom_read_byte((uint8_t*)4);
	
	LCD_init();
	LCD_Custom_Char(0, customChar);
	
	if (score == 0) c = ' ';
	else c = '0';
	char str [11] = {'S', 'c', 'o', 'r', 'e', ':', ' ', ' ' , ' ', '\0'};
	str[8] = 48;
	LCD_ClearScreen();
	LCD_DisplayString(1, str);
	LCD_Cursor(12);
	LCD_WriteData(0);
	char str2 [16] = {'H', 'i', 'g', 'h', ' ', 'S', 'c', 'o', 'r', 'e', ':', ' ', threeDig2, ' ', c, '\0'};
	str2[13] = highScore;
	LCD_Cursor(1);
	LCD_DisplayString(17, str2);
	
	i = 0; 
	tasks[i].state = -1;
	tasks[i].period = 10;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &LEDMatrixTick;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 10;
	tasks[i].elapsedTime = 0;
	//tasks[i].TickFct = &ButtonTick;
	i++;
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	while (1) {
		sleep_mode();
	}
}