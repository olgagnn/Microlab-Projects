#define F_CPU 16000000UL
#include <avr/io.h>
#include<stdio.h>

void main(void) {
    
    char input;
    
    TCCR1A = (1<<WGM11)|(0<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM13) | (1<<WGM12);
    
    DDRB = 0xFF;     //PORTB as output |
    DDRD = 0x00;     //PORTD as input |
    
    while (1){
        TCCR1B = (1<<WGM13) | (1<<WGM12);
        
        input = PIND;
        
        if(input == 0xFE)   //Checking PINS
        {
            //PD0
            while (PIND == 0xFE){   //Fpwm = 125 
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            ICR1H = 0x3E;
            ICR1L = 0x7F;       //TOP in ICR1
       
            OCR1AH = 0xF;
            OCR1AL = 0x3F;      //Duty Cycle in OCR1
            }       
                
        }
        
        else if(input == 0xFD)
        {
            //PD1
            while (PIND == 0xFD){   //Fpwm = 250
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            ICR1H = 0x1F;
            ICR1L = 0x3F;
       
            OCR1AH = 0xF;
            OCR1AL = 0x9F;
            }       
                
        }
        
        else if(input == 0xFB)
        {
            //PD2
            while (PIND == 0xFB){  //Fpwm = 500
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            ICR1H = 0xF;
            ICR1L = 0x9F;
       
            OCR1AH = 0x7;
            OCR1AL = 0xCF;
            }       
                
        }
        
        else if(input == 0xF7)
        {
            //PD3
            while (PIND == 0xF7){  //Fpwm = 1000
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            ICR1H = 0x7;
            ICR1L = 0xCF;
       
            OCR1AH = 0x3;
            OCR1AL = 0xE7;
            }       
                
        }
        
    }
    
}