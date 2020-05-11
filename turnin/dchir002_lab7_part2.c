/*	Author: Dumitru Chiriac lab
 *  Partner(s) Name: 
 *	Lab Section: 026
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;

}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum State {Start, s1, s2, s22, s3, stopP, stopN, vict} state;

unsigned char score = 5;
unsigned char win = 0;

void Tick(){
	unsigned char temp = ~PINA;
	unsigned char *letter = "0";
	switch(state){
		case Start: state = s1; break;
		case s1: 
			if (temp == 0x01){
				state = stopP;
		 		score--;		
			}
			else
				state = s2;
			break;
		case s2: 
			if (temp == 0x01){ 
                                if (score >= 8)
                                	state = vict;
				else {
					state = stopP;
                                	score++;
                        	}
			}
                        else
                                state = s3;	break;
		case s3: 
		       if (temp == 0x01){ 
                                state = stopP;
                                score--;
                        }
                        else
                                state = s22;
			break;
		case s22: 
		       if (temp == 0x01){
			      	if (score >= 8)
                                        state = vict; 
				else {
					state = stopP;
                                	score++;
				}
                        }
                        else
                                state = s1;
			break;
		case stopP: state = (temp == 0x01) ? stopP : stopN; break;
		case stopN: state = (temp == 0x00) ? stopN : s1; break;
		case vict: state = vict; break;
		default: state = Start; break;
	}

	switch(state){
		case Start: PORTB = 0x01; break;
		case s1: PORTB = 0x01; break;
		case s2: PORTB = 0x02; break;
		case s22: PORTB = 0x02; break;
		case s3: PORTB = 0x04; break;
		case vict: win = 1; PORTB = 0xFF; break;
		default: break;
	}
	if(win == 0){
		letter[0] = letter[0] + score;
		LCD_DisplayString(1, letter);
	}
	else
		LCD_DisplayString(1, "Victory!");
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    TimerSet(300);
    TimerOn();

    LCD_init();

    LCD_ClearScreen();
    LCD_Cursor(1);
    //unsigned char tmpB = 0x00;

    
    while (1) {
	
	
	while (!TimerFlag);
	Tick();
	TimerFlag = 0;
    }
    return 1;
}
