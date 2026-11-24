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
    
    DDRB = 0xFF; //PORTB as output
    DDRD = 0xFF; //PORTD as output
    DDRC = 0x00; //PORTC as input
    
    //REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0010 => select ADC2(pin PC2)
    //ADLAR=1 => Left adjust the ADC result
    ADMUX = 0b01100010;
    
    //ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    //ADIE=0 => disable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10000111;
            
    lcd_init(); 
    _delay_ms(2);

    unsigned int counter;
    char adc_h;
    unsigned int temp;
    unsigned int temp2;
    
    counter = 0;
    
    while (1) {
        
        lcd_init(); //clearing screen
        _delay_ms(2);
        
        ADCSRA |= (1<<ADSC);         //Set ADSC flag of ADCSRA
        
        while(ADCSRA & (1<<ADSC)){} //Wait until ADSC flag of ADCSRA becomes 0 
        
        adc_h = ADCH;               //Get ADC the Result
        
        //increase counter
        counter++;
        
        //Display in PORTB LEDS 
        PORTB = counter;
        
        temp = adc_h * 5;
        
        temp2 = temp & 0xFF00;  //krataw mono to high meros
        
        temp2 = temp2 >> 8;     //olistisi deksia kata 8 theseis 
                                //temp = 1os arithmos gia ektupwsh
        
        temp2 = temp2 | 0b00110000;
        lcd_data(temp2);        //emafanish 1ou arithmou
        
        temp2 = 0b00101110;
        lcd_data(temp2);        //emfanish '.'
        
        temp2 = temp & 0xFF;    //krataw mono to low meros
        temp2 = temp2 * 10;
        temp = temp2;
        temp2 = temp2 & 0xFF00; //krataw mono to high meros
        temp2 = temp2 >> 8;     //olistisi deksia kata 8 theseis 
                                //temp = 2os arithmos gia ektupwsh
        temp2 = temp2 | 0b00110000;
        lcd_data(temp2);        //emafanish 2ou arithmou
        
        temp = temp & 0xFF;     //krataw mono to low meros
        temp = temp * 10;       //olistisi deksia kata 8 theseis 
                                //temp = 3os arithmos gia ektupwsh
        
        temp = temp & 0xFF00;   //krataw mono to high meros
        temp = temp >> 8;       //olistisi deksia kata 8 theseis 
                                //temp = 3os arithmos gia ektupwsh
        temp = temp | 0b00110000;
        lcd_data(temp);         //emafanish 3ou arithmou
         
        _delay_ms(1000);
        
    }
}