#define F_CPU 16000000UL
#include <avr/io.h>
#include<stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main() 
{
    
    unsigned char duty;
    int x; //,input;
    
    ; //prescaler = 1024
    TCCR1A = (1<<WGM10) | (1<<COM1A1) | (1<<COM1B1)
    TCCR1B = (0<<WGM12) | (1<<CS12) | (1<<CS10) 
    
    DDRB = 0xFF;//|= 0b00000010;
    DDRD = 0x00;       //PORTD as input
    
    const int table[]= {0x05,0x19,0x2D,0x42,0x56,0x6B,
                        0x7F,0x93,0xA8,0xBC,0xD1,0xE5,0xF9};
    //{5,25,66,45,107,86,147,127,188,168,229,209,0,249};
    
    duty = table[6];
    x = 6;
    
    OCR1AL = duty;
    
    while (1)
    {
        //input = PIND;
        
        if (PIND == 0xFB) //0x04 anapoda  
        { 
            //PD2 -8%
        
            if (x!=0)
            {
                x = x - 1;
                duty = table[x];
                OCR1AL = duty;
                _delay_ms(200);
                       
            }
            
        }
        
        else if (PIND == 0xFD) //0x02 anapoda
        {
            //PD1 +8%
            
           if (x != 12)
            {
                x = x + 1;
                duty = table[x];
                OCR1AL = duty;
                _delay_ms(200);
                       
            }
        }
                
    }
}