#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

bool flag = false;

void endiameso(){
    PORTB = 0xFF; //anama twn leds
    _delay_ms(500);
                    
    PORTB = 0x01; //anama tou PB0
    //cli(); //epeidh den prepei na ginei diakopi gia tcnt
    TCCR1B = (1<<CS12) | (0<<CS11) | (1<<CS10); 
    TCNT1 = 10848;   // for 3,5 sec at 16 MHz	
    //TCNT1H = 0x2A;
    //TCNT1L = 0x60;
               
    //sei();
}

void normal(){
                  
    PORTB = 0x01; //anama tou PB0
    //cli();
    TCCR1B = (1<<CS12) | (0<<CS11) | (1<<CS10); 
    TCNT1 = 3035;   // for 4 sec at 16 MHz	
    //TCNT1H = 0x0B;
    //TCNT1L = 0xDB;
}

ISR (TIMER1_OVF_vect)    // Timer1 ISR
{   
    PORTB = 0x00; //svisimo PORTB LEDS
    flag = false;
}


ISR (INT1_vect)
{   
        // me patima
        if(flag == true){
            sei();
            endiameso();
        }
        // xwris patima
        else{
            sei();
            flag = true;
            normal();
        }
       
}

int main(void) {
    char input;
    //Interrupt on rising edge of INT1 pin
    EICRA=(1 << ISC11) | (1 << ISC10);
    //Enable the INT1 interrupt(PD3)
    EIMSK=(1 << INT1);
     
    TIMSK1 =  (1<<TOIE1); //;Energopoihsh diakopis overflow gia timer1

    sei();              //Enable global interrupts
    
    DDRB = 0xFF; //PORTB as output
   
    DDRC = 0x00; //PORTC as input
    
    while (1) {
        input = PINC; 
        input = input & 0x20; 
        
        if(input != 0x20) //an PC5 pathmeno
        {
            if (flag == false) 
            {   
                sei();
                flag = true;	//kanoniko blink 
                normal();
                 
            }
            else if(flag == true)
            {
                sei();
                endiameso();	//me endiameso patima PC5/PD3
            }
        }
    }
}