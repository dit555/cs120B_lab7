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

enum State {Start, Wait, Inc, Dec, Reset} state;

unsigned char time = 10;
unsigned char temp2 = 0x00;
void Tick(){
        unsigned char temp = ~PINA;
        unsigned char *letter = "0";
        switch(state){ //transitions
                case Start: temp2 = 0x07; state = Wait; break;
                case Wait:
                        if (temp == 0x01){
                                state = Inc;
			}
                        else if (temp == 0x02){
                                state = Dec;
                        }
                        else if (temp == 0x03)
                                state = Reset;
                        else
                                state = Wait;
                        break;
                
                case Inc:
			if ((temp == 0x01) && (time > 0)){
			 	state = Inc;
				time--;
			}
			else{	
				time = 10;
				state = Wait; break;
			}
                	break;
                case Dec:
		       if ((temp == 0x02) && (time > 0)){
                                state = Dec;
                                time--;
                        }
                        else{
                                time = 10;
                                state = Wait; break;
                        }	
			break;
                case Reset: state = Wait; break;
                
                default: state = Start; break;

        }

        switch(state){
                case Start: break;
                case Wait: break;
                case Dec: if(temp2 > 0 && time == 10) temp2--; break;
                case Inc: if(temp2 != 9 && time == 10) temp2++; break;
                case Reset: temp2 = 0x00; break;
                default: break;

        }
    	letter[0] = temp2 + letter[0];
	LCD_DisplayString(1, letter);
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    TimerSet(100);
    TimerOn();
    //unsigned char tmpB = 0x00;
    LCD_init();
    LCD_ClearScreen();
    LCD_Cursor(1);
    
    while (1) {
	
	
	while (!TimerFlag);
	Tick();
	TimerFlag = 0;
    }
    return 1;
}
