#define F_CPU 16000000UL                   //Microcontroller operating frequency in MHz
#include <avr/io.h>                         
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

bool flag;                                //Definitions and functions
char z;
unsigned int res;

void write_2_nibbles(char x) {
    z = PIND;
    PORTD = (x & 0xF0) + (z & 0x0F);
    PORTD = PORTD | 0b00001000;
    PORTD = PORTD & 0b11110111;
    PORTD = ((x<<4) & 0xF0) + (z & 0x0F);
    PORTD = PORTD | 0b00001000;
    PORTD = PORTD & 0b11110111;
}

void lcd_data(char x) {
    PORTD = PORTD | 0b00000100;
    write_2_nibbles(x);
    _delay_us(43);
}

void lcd_command(char x) {
    PORTD = PORTD & 0b11111011;
    write_2_nibbles(x);
    _delay_us(39);
}

void lcd_init() {
    _delay_ms(40);
    
    PORTD = 0x30;
    PORTD = PORTD | 0b00001000;
    PORTD = PORTD & 0b11110111;
    _delay_us(39);
    
    PORTD = 0x30;
    PORTD = PORTD | 0b00001000;
    PORTD = PORTD & 0b11110111;
    _delay_us(39);
    
    PORTD = 0x20;
    PORTD = PORTD | 0b00001000;
    PORTD = PORTD & 0b11110111;
    _delay_us(39);
    
    lcd_command(0x28);
    z = 0x0C;
    lcd_command(0x0C);
    z=0x01;
    lcd_command(0x01);
    
    _delay_us(1530);
    
    z = 0x06;
    lcd_command(0x06);
}

int main(void) {
    
    DDRB = 0xFF;     //PORTB as output for the leds
    DDRC = 0x00;     //PORTC as input for ADC 
    DDRD = 0xFF;     //PORTD as output for lcd screen 
    
    //REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0011 => select ADC3(pin PC3),
    //ADLAR=0 => Right adjust the ADC result 
    ADMUX = 0b01000011;
    
    //ADEN=1 => ADC Enable, ADSC=0 => Single Conversion Mode, ADATE=0 => Auto triggering OFF
    //ADIE=0 => Disable ADC interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10000111;
    
    flag = false;
    
    lcd_init();     //LCD screen initialization
    
    _delay_ms(2);   //Wait for 2usec
    
    sei();
    
    while(1) {
        
        ADCSRA |= (1<<ADSC);            //Set ADSC flag of ADCSRA
        
        while(ADCSRA & (1<<ADSC));      //Wait until ADSC flag of ADCSRA becomes 0 
        
        res = ADCL;                     //Get ADC the Result
        res += (ADCH << 8);
        
        if(res < 205) {                 //If CO levels < 70ppm (ADC = 205)
         
            PORTB = 0x01;               //Stop blinking
            
            if (flag) {
            
                flag = false;
                lcd_command(0x01);		// clear display 
                lcd_data('C');
                lcd_data('L');
                lcd_data('E');
                lcd_data('A');
                lcd_data('R');
                
            }
        }
        else{                           //If CO levels > 70ppm 
            
            if (!flag) {
            
                flag = true;
                lcd_command(0x01);		// clear display 
                lcd_data('G');
                lcd_data('A');
                lcd_data('S');
                lcd_data(' ');
                lcd_data('D');
                lcd_data('E');
                lcd_data('T');
                lcd_data('E');
                lcd_data('C');
                lcd_data('T');
                lcd_data('E');
                lcd_data('D');
                
            }
              
            if(res < 337) {             //Level 1
            
                PORTB = 0x01;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
            
            if(res < 469) {             //Level 2
            
                PORTB = 0x03;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
            
            if(res < 601) {             //Level 3
            
                PORTB = 0x07;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
            
            if(res < 733) {             //Level 4
            
                PORTB = 0x0F;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
            
            if(res < 865) {             //Level 5
            
                PORTB = 0x1F;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
            
            if(res >= 865) {            //Level 6
            
                PORTB = 0x3F;
                _delay_ms(100);
                PORTB = 0x00;
                _delay_ms(100);
                
            }
        }
    }
    _delay_ms(100);                 //Check CO sensor every 100ms
}