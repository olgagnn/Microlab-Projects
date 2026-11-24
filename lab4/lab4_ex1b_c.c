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
    
    //TIMSK1 = (1<<TOIE1); //;Energopoihsh diakopis overflow gia timer1
    //TCCR1B = (0<<CS12) | (0<<CS11) | (0<<CS10); //apenergopoisi timer1
    
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
    unsigned int  adc_h;
    char check;
    unsigned int temp;
    unsigned int temp2;
    
    counter = 0;
    
    while (1) {
        
        ADCSRA = ADCSRA || 0b01000000;  //ADSC = 1 ->ksekinaei h metatropi
        
        check = ADCSRA & 0b01000000;
        
        while(check == 0b01000000)
        {
            //+elegxos gia ton counter
            
            check = ADCSRA & 0b01000000;
        }
        
        adc_h = ADCH;
       
        lcd_command(0x01); //clearing screen
         _delay_ms(150);
        
        adc_h = adc_h * 5;
        
        temp = adc_h & 0xFF00; //krataw mono to high meros
        
        temp >> 8;  //olistisi deksia kata 8 theseis 
                    //temp = 1os arithmos gia ektupwsh
        
        temp = temp | 0b00110000;
        lcd_data(temp);     //emafanish 1ou arithmou
        
        temp = 0b00101110;
        lcd_data(temp);     //emfanish '.'
        
        temp = adc_h & 0xFF;  //krataw mono to low meros
        temp = temp * 10;
        temp2 = temp; //apothikevw ston temp2
        temp = temp & 0xFF00; //krataw mono to high meros
        temp = temp >> 8;  //olistisi deksia kata 8 theseis 
                    //temp = 2os arithmos gia ektupwsh
        
       
        
        temp = temp | 0b00110000;
        lcd_data(temp);     //emafanish 2ou arithmou
        
        temp = temp2 * 10;
        temp = temp & 0xFF00; //krataw mono to high meros
        temp = temp >> 8;  //olistisi deksia kata 8 theseis 
                    //temp = 3os arithmos gia ektupwsh
        
        
        temp = temp | 0b00110000;
        lcd_data(temp);     //emafanish 3ou arithmou
         
        
    }
}