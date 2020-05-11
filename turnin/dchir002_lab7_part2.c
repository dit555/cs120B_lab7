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

enum State {Start, s1, s2, s22, s3, stopP, stopN} state;

void Tick(){
	unsigned char temp = ~PINA;
	switch(state){
		case Start: state = s1; break;
		case s1: state = (temp == 0x01) ? stopP : s2 ; break;
		case s2: state = (temp == 0x01) ? stopP : s3; break;
		case s3: state = (temp == 0x01) ? stopP: s22; break;
		case s22: state = (temp == 0x01) ? stopP : s1; break;
		case stopP: state = (temp == 0x01) ? stopP : stopN; break;
		case stopN: state = (temp == 0x00) ? stopN : s1; break;
		
		default: state = Start; break;
	}

	switch(state){
		case Start: PORTB = 0x01; break;
		case s1: PORTB = 0x01; break;
		case s2: PORTB = 0x02; break;
		case s22: PORTB = 0x02; break;
		case s3: PORTB = 0x04; break;
		default: break;
	}
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(300);
    TimerOn();
    //unsigned char tmpB = 0x00;

    
    while (1) {
	
	
	while (!TimerFlag);
	Tick();
	TimerFlag = 0;
    }
    return 1;
}
