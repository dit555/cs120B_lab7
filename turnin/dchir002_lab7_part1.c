/*	Author: Dumitru Chiriaclab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum State { Start, Wait, Inc, Inc2, Dec, Dec2, Reset, Reset2} state;

unsigned char temp2 = 0x00;

void Tick(){
        unsigned char temp = ~PINA;
	unsigned char *letter = "0";

        switch(state){ //transitions
                case Start: temp2 = 0x07; state = Wait; break;
                case Wait:
                        if (temp == 0x01){
                                state = Inc;
                                if(temp2 != 9) temp2++;
                        }
                        else if (temp == 0x02){
                                state = Dec;
                                if(temp2 > 0) temp2--;
                        }
                        else if (temp == 0x03)
                                state = Reset;
                        else
                                state = Wait;
                        break;
                case Inc2: state = Wait; break;
                case Inc: state = (temp == 0x01) ? Inc : Inc2; break;
                case Dec2: state = Wait; break;
                case Dec: state = (temp == 0x02) ? Dec : Dec2; break;
                case Reset: state = Reset2; break;
                case Reset2: state = (temp == 0x03) ? Reset : Wait; break;
                default: state = Start; break;

        }

        switch(state){
                case Start: break;
                case Wait: break;
                case Dec2: break;
                case Inc2: break;
                case Reset: temp2 = 0x00; break;
                default: break;

        }
        //LCD_ClearScreen();
	letter[0] = temp2 + letter[0];
	//LCD_Cursor(1);
	//LCD_WriteCommand(48);
	LCD_DisplayString (1, "0");
	delay_ms(100);
}


int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    LCD_init();

    LCD_ClearScreen();
    LCD_Cursor(1);
    while (1) {
	Tick();
    }
    return 1;
}
