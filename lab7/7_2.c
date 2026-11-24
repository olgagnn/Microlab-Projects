#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

char z;

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

uint8_t one_wire_reset() {
    
    uint8_t r24;
    
    DDRD = 0x10;                            //set PD4 as output 
    PORTD = 0xEF;                           //clear PD4   
     
    _delay_us(480);                         //480usec reset pulse
    
    DDRD = 0xEF;                            //set PD4 as input
    PORTD = 0xEF;                           //disable pull-up 
    
    _delay_us(100);                         //wait 100usec for connected devices 
                                            //to transmit presence pulse
    
    r24 = PIND;                             //read PIND
    
    _delay_us(380);    
    
    if((r24 & 0x10) == 0x00) r24 = 0x01;    //if a connected device is 
    else r24 = 0x00;                        //detected(PD4=0) return 1 
                                            //else return 0
    return r24;
}

uint8_t one_wire_receive_bit() { 
    
    uint8_t r24;
    
    DDRD = 0x10;                            //set PD4 as output         
    PORTD = 0xEF;                           //clear PD4   
     
    _delay_us(2);                           //2usec reset pulse
    
    DDRD = 0xEF;                            //set PD4 as input
    PORTD = 0xEF;                           //disable pull-up 
    
    _delay_us(10);                          //wait 10usec for connected devices 
                                            //to transmit presence pulse
    
    if((PIND & 0x10) == 0) r24 = 0x00;
    else r24 = 0x01;
                      
    _delay_us(49);                          //delay 49usec to meet the standards
    
    return r24;

}

void one_wire_transmit_bit(uint8_t r24) {
    
    uint8_t temp;
    
    temp = r24;
    
    DDRD = 0x10;                            //set PD4 as output 
    PORTD = 0xEF;                           //clear PD4   
     
    _delay_us(2);                           //2usec reset pulse
    
    temp = (temp & 0x01);
    
    if(temp == 0x00) PORTD = 0xEF;          //clear PD4 
    else PORTD = 0x10;                      //set PD4   
    
    _delay_us(58);                          //wait 58usec for connected devices 
                                            //to transmit presence pulse
    
    DDRD = 0xEF;                            //set PD4 as input
    PORTD = 0xEF;                           //disable pull-up 
                      
    _delay_us(1);                           //delay 1usec to meet the standards
    
}

uint8_t one_wire_receive_byte() {
    
    uint8_t temp, r26, r24;
    
    r26 = 0;
    
    for(int i = 0; i < 8; i++) {
    
        r24 = one_wire_receive_bit();
        
        r26 = r26 >> 1;
        
        temp = (r24 & 0x01);
        
        if(temp == 0x00) r26 = r26 | r24;
        else {
            r24 = 0x80;
            r26 = r26 | r24;
        }
    }
    
    return r26;
}

void one_wire_transmit_byte(uint8_t r24) {

    uint8_t r26, temp;
    
    r26 = r24;
    
    for(int i = 0; i < 8; i++) {
        
        temp = (r26 & 0x01);
        r24 = 0;
        if(temp == 0x01) r24 = 0x01;
        one_wire_transmit_bit(r24);
        
        r26 = r26 >> 1;
        
    }
}

int count_temperature () {

    uint8_t r24, r25;
    int result;
    
    if(one_wire_reset() == 0x01) {
    
        one_wire_transmit_byte(0xCC);
        one_wire_transmit_byte(0x44);
    
        while(one_wire_receive_bit() != 1);
    
        one_wire_reset();
    
        one_wire_transmit_byte(0xCC);
        one_wire_transmit_byte(0xBE);
    
        r24 = one_wire_receive_byte();          //LSB
        r25 = one_wire_receive_byte();          //MSB
    
        result = r25;
        result = (result << 8) & 0xFF00;
        
        result = result + r24;
    
        return result;
    
    }
    else return 0x8000;
    
}

int main(void){
   
    DDRD = 0xFF;                                //PORTD as output
    lcd_init();
    _delay_ms(2);
    
    int x, ak, temp, temp2;
    float res;
    
    res = 0;
    
    while(1){
        
        while(one_wire_reset() == 0x00) { 
            DDRD = 0xFF;                        //output 
            lcd_init();                         // clear display 
            _delay_ms(2);
            lcd_data('N');
            lcd_data('O');
            lcd_data(' ');
            lcd_data('D');
            lcd_data('e');
            lcd_data('v');
            lcd_data('i');
            lcd_data('c');
            lcd_data('e');
            _delay_ms(1000);
        }
        
        while(one_wire_reset() == 0x01) { 
            
            x = count_temperature();
          
            DDRD = 0xFF;                        //output 
            lcd_init();                         // clear display 
            _delay_ms(2);
            
             
            res = x*0.0625;
            
            if ((x & 0xF800) == 0x00) {         //an thermokrasia > 0
                lcd_data('+');
                _delay_ms(2);
            }
            
            else if ((x & 0xF800) == 0xF800){   //an thermokrasia < 0
                lcd_data('-');
                _delay_ms(10);
                res = - res;
            }
         
            ak = res;                           //krataw mono to akeraio
            temp = ak / 100;        
            
            if (temp != 0 ) {                   //1o psifio 
                temp = temp | 0b00110000;       //an den einai 0 to emfanizw sthn othoni
                lcd_data(temp);
                _delay_ms(2);
            }
    
            temp2 = ak % 100;       
            temp2 = temp2 /10;   
            
            if (temp2 != 0 && temp == 0 ) {     //2o psifio 
                temp2 = temp2 | 0b00110000;     //an einai 0 kai htan kai to prohgoumeno 
                lcd_data(temp2);                //den to emfanizw
                _delay_ms(2);
            }
   
            temp = ak % 10;         
            temp = temp | 0b00110000;           //3o psifio 
            lcd_data(temp);                     //to ektupwnw akoma kai 0 
            _delay_ms(2);
    
            temp = 0b00101110;
            lcd_data(temp);                     //emfanish '.'
            _delay_ms(2);
    
            res = res - ak;                     //afairw to akeraio, res: mono to dekadiko
    
            for (int k=0; k<3; k++) {
                res = res * 10;                 //gia thn emfanish tou dekadikou merous
                temp = res;
                temp = temp % 10;   
                temp = temp | 0b00110000;
                lcd_data(temp);
                _delay_ms(2);
            }
    
            lcd_data(223);                      //emfanish '°'
            _delay_ms(2);
    
            temp = 0b01000011;
            lcd_data(temp);                     //emfanish 'C'
            _delay_ms(2);
    
            _delay_ms(1000);
            
        }    
    }
}