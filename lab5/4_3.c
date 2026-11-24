#define F_CPU 16000000UL        //Microcontroller operating frequency in MHz
#include <avr/io.h>                         
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
                               
char z;                         //Definitions and functions

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
    
    DDRB = 0x02;     //PORTB as input for PWM 
    DDRC = 0x00;     //PORTC as input for ADC 
    DDRD = 0xFF;     //PORTD as output for lcd screen 
    
    TCCR1A = (1<<WGM11)|(0<<WGM10) | (1<<COM1A1);  
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
    
    //REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0001 => select ADC1(pin PC1),
    //ADLAR=1 => Left adjust the ADC result 
    ADMUX = 0b01100001;
    
    //ADEN=1 => ADC Enable, ADSC=0 => Single Conversion Mode, ADATE=0 => Auto triggering OFF
    //ADIE=0 => Disable ADC interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10000111;
   
    char adc_h;
    unsigned int temp;
    unsigned int temp2;
    
    lcd_init();     //LCD screen initialization
    
    _delay_ms(2);   //Wait for 2msec
    
    while(1) {
        
        lcd_init(); 
        _delay_ms(2);   //Wait for 2msec
        ICR1 = 0x18F;       //ICR1 = 0x18F
        //input = PINB;
        
       if((PINB & 0x04) == 0x00)
        {   
            
           //PB2: DC = 20% 
            
            OCR1A = 0x50;
           
            //grapse sthn othoni '20%'
            lcd_data(0b00110010);  //'2'       
            lcd_data(0b00110000);  //'0'         
            lcd_data(0b00100101); //'%'
            
          
                
        }
        
        else if((PINB & 0x8) == 0x00)
        {   
          
            //PB3: DC = 40%
            OCR1A = 0xA0;
                   
             //grapse sthn othoni '40%'
            lcd_data(0b00110100); //'4'        
            lcd_data(0b00110000);  //'0'         
            lcd_data(0b00100101); //'%'
                  
        }
        
        else if((PINB & 0x10)== 0x00)
        {   
            //PB4: DC = 60%
            OCR1A = 0xF0;     
            
            //grapse sthn othoni '60%'
            lcd_data(0b00110110); //'6'
            lcd_data(0b00110000);  //'0'         
            lcd_data(0b00100101); //'%'
        }  
            
  
        else if((PINB & 0x20) == 0x00)
        {   
            
            
            //PB5: DC = 80%
            OCR1A = 0x13F;   
          
        
            //grapse sthn othoni '80%'
            lcd_data(0b00111000); //'8'
            lcd_data(0b00110000);  //'0'         
            lcd_data(0b00100101); //'%'
            
            
         }
        else{
              OCR1A = 0x00;   
          
        }
            
        
        lcd_command(0xC0);		// clear display 
        _delay_us(40);
        
        ADCSRA |= (1<<ADSC);            //Set ADSC flag of ADCSRA
        while(ADCSRA & (1<<ADSC));      //Wait until ADSC flag of ADCSRA becomes 0 
           
        adc_h = ADCH;     
        temp = adc_h * 5;
        
        temp2 = temp & 0xFF00; //krataw mono to high meros
        
        temp2 = temp2 >> 8;  //olistisi deksia kata 8 theseis 
                    //temp = 1os arithmos gia ektupwsh
        
        temp2 = temp2 | 0b00110000;
        lcd_data(temp2);     //emafanish 1ou arithmou
        
        temp2 = 0b00101110;
        lcd_data(temp2);     //emfanish '.'
        
        temp2 = temp & 0xFF;  //krataw mono to low meros
        temp2 = temp2 * 10;
        temp = temp2;
        temp2 = temp2 & 0xFF00; //krataw mono to high meros
        temp2 = temp2 >> 8;       //olistisi deksia kata 8 theseis 
                                //temp = 2os arithmos gia ektupwsh
        temp2 = temp2 | 0b00110000;
        lcd_data(temp2);     //emafanish 2ou arithmou
        
        temp = temp & 0xFF; //krataw mono to low meros
        temp = temp * 10;   //olistisi deksia kata 8 theseis 
                            //temp = 3os arithmos gia ektupwsh
        
        temp = temp & 0xFF00; //krataw mono to high meros
        temp = temp >> 8;       //olistisi deksia kata 8 theseis 
                                //temp = 3os arithmos gia ektupwsh
        temp = temp | 0b00110000;
        lcd_data(temp);     //emafanish 3ou arithmou
        
        _delay_ms(200);
}
    
}