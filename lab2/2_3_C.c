#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

bool flag = false;

ISR (INT1_vect)
{   
        //Avoid spinthirismo
        while(EIFR != 0){
            EIFR = 1;
            _delay_ms(5);
        }
        // me patima
        if(flag == true){
            sei();
            PORTB = 0xFF;
            _delay_ms(500);
            PORTB = 0x01;
            _delay_ms(3500);
            PORTB = 0x00;
        }
        // xwris patima
        else{
            sei();
            flag = true;
            PORTB = 0x01;
            _delay_ms(4000);
            PORTB = 0x00; 
        }
       
}
   

int main(void) {
    
    while(1){
    //Interrupt on rising edge of INT1 pin
    EICRA=(1 << ISC11) | (1 << ISC10);
    //Enable the INT1 interrupt(PD3)
    EIMSK=(1 << INT1);
    sei();              //Enable global interrupts
    
    DDRB=0xFF;          //Set PORTB as output
    flag = false;
   
    }
}
