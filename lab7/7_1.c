#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>


uint8_t one_wire_reset() {
    
    uint8_t r24;
    
    DDRD = 0x10;                            //set PD4 as output DDRD = 0x10
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
    
    DDRD = 0x10;                            //set PD4 as output DDRD |= (1 << PD4);         
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
    
        r24 = one_wire_receive_byte(); //LSB
        r25 = one_wire_receive_byte(); //MSB
    
        result = r25;
        result = (result << 8) & 0xFF00;
        
        result = result + r24;
    
        return result;
    
    }
    else return 0x8000;
    
}