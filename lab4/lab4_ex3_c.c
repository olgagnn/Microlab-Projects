#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

unsigned char z;

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
    
    unsigned int input;
    char adc_h;
    char vout;
    
    DDRB = 0x02;       //PORTB as input and PB1 as output
    DDRD = 0xFF;       //PORTD as output (LCD)
    DDRC = 0x00;       //PORTC as input
    PORTD = 0xFF;      //pull up 
    
    //non-inverted fast PWM, prescaler=8
    TCCR1A = (1<<WGM11)|(0<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM13) | (1<<WGM12);

    //REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0001 => select ADC1(pin PC1)
    //ADLAR=1 => Left adjust the ADC result
    ADMUX = 0b01100001;
    
    //ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    //ADIE=0 => enable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10001111;
    
    
    sei();
    
    lcd_init();
    _delay_ms(2);
    
    while (1) {
        
        TCCR1B = (1<<WGM13) | (1<<WGM12); 
        
        input = PINB;
        //input = ~input;
        input = input & 0x04;
        
        
        if(input == 0x00)
        {
            //PB2
            while ( input == 0x00){
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            
            ICR1H = 0x1;        //TOP = ICR1
            ICR1L = 0x8F;       //ICR1 = 0x18F
       
           //PB2: DC = 20% 
            
            OCR1AH = 0x00;
            OCR1AL = 0x50;
            
            //diavasma tou ADC
            
            //upologismos tou Vout 
            //Vout = ADC x DC x 5 /1024
            
           
            //sthn 2h grammh
            //Vout -> X.XX
            
             input = PINB;
            //input = ~input;
            input = input & 0x04;
                
            }  
            //grapse sthn othoni '20%'
            lcd_data(0b0011001);  //'2'
            _delay_ms(2);
            lcd_data(0b0011000);  //'0'
            _delay_ms(2);
            lcd_data(0b00100101); //'%'
            _delay_ms(2);
            
            
            _delay_ms(2000);
            
                
        }
        
        else if(input == 0xF7)
        {
            //PB3
            while (PINB == 0xF7){
            
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            
            ICR1H = 0x1;        //TOP = ICR1
            ICR1L = 0x8F;       //ICR1 = 0x18F
            
            //PB3: DC = 40%
            OCR1AH = 0x00;
            OCR1AL = 0xA0;
            
             //grapse sthn othoni '40%'
            lcd_data(0b00110100); //'4'
            _delay_ms(2);
            lcd_data(0b0011000);  //'0'
            _delay_ms(2);
            lcd_data(0b00100101); //'%'
            _delay_ms(2);
           
            }       
                
        }
        
        else if(input == 0xEF)
        {
            //PB4
            while (PINB == 0xEF){
                
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            
            ICR1H = 0x1;         //TOP = ICR1
            ICR1L = 0x8F;       //ICR1 = 0x18F
            
            //PB4: DC = 60%
            OCR1AH = 0x00;
            OCR1AL = 0xF0;
            
             //grapse sthn othoni '60%'
            lcd_data(0b00110110); //'6'
            _delay_ms(2);
            lcd_data(0b0011000);  //'0'
            _delay_ms(2);
            lcd_data(0b00100101); //'%'
            _delay_ms(2);
            }       
                
        }
        
        else if(input == 0xDF)
        {
            //PB5
            while (PINB == 0xDF){
                
            TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
            
            ICR1H = 0x1;        //TOP = ICR1
            ICR1L = 0x8F;       //ICR1 = 0x18F
            
            //PB5: DC = 80%
            OCR1AH = 0x01;
            OCR1AL = 0x3F;
            
             //grapse sthn othoni '80%'
            lcd_data(0b00111000); //'8'
            _delay_ms(2);
            lcd_data(0b0011000);  //'0'
            _delay_ms(2);
            lcd_data(0b00100101); //'%'
            _delay_ms(2);
            
            
     	}          
      }
    }
  }
    

