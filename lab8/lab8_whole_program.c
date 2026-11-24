#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>

//Defining char arrays for ESP messages and payload 
char connect[] = "ESP:connect\n";
char success[] = "\"Success\"\n";
char fail[] = "\"Fail\"\n";
char url[] = "ESP:url:\"http://192.168.1.250:5000/data\"\n"; 
char transmit[] = "ESP:transmit\n";
char restart[] = "restart\n";
char payNurse[] = "ESP:payload:[{\"name\": \"temperature\",\"value\": \"35.8\"},{\"name\": \"pressure\",\"value\": \"11.0\"},{\"name\": \"team\",\"value\": \"49\"},{\"name\": \"status\",\"value\": \"NURSECALL\"}]\n";    
char payTemp[] = "ESP:payload:[{\"name\": \"temperature\",\"value\": \"35.8\"},{\"name\": \"pressure\",\"value\": \"11.0\"},{\"name\": \"team\",\"value\": \"49\"},{\"name\": \"status\",\"value\": \"CHECKTEMP\"}]\n";    
char payOK[] = "ESP:payload:[{\"name\": \"temperature\",\"value\": \"35.8\"},{\"name\": \"pressure\",\"value\": \"11.0\"},{\"name\": \"team\",\"value\": \"49\"},{\"name\": \"status\",\"value\": \"OK\"}]\n";    
char payPress[] = "ESP:payload:[{\"name\": \"temperature\",\"value\": \"35.8\"},{\"name\": \"pressure\",\"value\": \"11.0\"},{\"name\": \"team\",\"value\": \"49\"},{\"name\": \"status\",\"value\": \"CHECKPRESSURE\"}]\n"; 
   
char z;
int status;

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

void print_success(){
    
    //prin kalesw thn sunartisi vazw lcd_data('1')/('2')
    _delay_ms(2);
    lcd_data('.');
    _delay_ms(2);
    lcd_data('S');
    _delay_ms(2);
    lcd_data('u');
    _delay_ms(2);
    lcd_data('c');
    _delay_ms(2);
    lcd_data('c');
    _delay_ms(2);
    lcd_data('e');
    _delay_ms(2);
    lcd_data('s');
    _delay_ms(2);
    lcd_data('s');
    _delay_ms(2);
   
}

void print_fail(){
    
    //prin kalesw thn sunartisi vazw lcd_data('1')/('2')
    _delay_ms(2);
    lcd_data('.');
    _delay_ms(2);
    lcd_data('F');
    _delay_ms(2);
    lcd_data('a');
    _delay_ms(2);
    lcd_data('i');
    _delay_ms(2);
    lcd_data('l');
    _delay_ms(2);
    
}

void print_server_response(char *response){
    
    DDRD = 0xFF;                                //PORTD as output
    lcd_init();
    _delay_ms(2);
    
    lcd_data('4');
    _delay_ms(2);
    lcd_data('.');
    
    if(response[5] == 'K') {
        _delay_ms(2);
        lcd_data('2');
        _delay_ms(2);
        lcd_data('0');
        _delay_ms(2);
        lcd_data('0');
        _delay_ms(2);
        lcd_data(' ');
        _delay_ms(2);
        lcd_data('O');
        _delay_ms(2);
        lcd_data('K');
        _delay_ms(2);
    }
    
    else {
        for(int i = 0; response[i] != '\n'; i++){
    
        _delay_ms(2);
        lcd_data(response[i]);
        _delay_ms(10);
    }
  }
    
}

/* Routine: usart_init
Description:
This routine initializes the
usart as shown below.
------- INITIALIZATIONS -------
Baud rate: 9600 (Fck= 8MH)
Asynchronous mode
Transmitter on
Reciever on
Communication parameters: 8 Data ,1 Stop, no Parity
--------------------------------
parameters: ubrr to control the BAUD.
return value: None.*/

void usart_init(unsigned int ubrr){
    
    UCSR0A=0;
    UCSR0B=(1<<RXEN0)|(1<<TXEN0);
    UBRR0H=(unsigned char)(ubrr>>8);
    UBRR0L=(unsigned char)ubrr;
    UCSR0C=(3 << UCSZ00);
    
    return;
}

/* Routine: usart_transmit
Description:
This routine sends a byte of data
using usart.
parameters:
data: the byte to be transmitted
return value: None. */

void usart_transmit(uint8_t data){
    
    while(!(UCSR0A&(1<<UDRE0)));
        UDR0=data;
}

/* Routine: usart_receive
Description:
This routine receives a byte of data
from usart.
parameters: None.
return value: the received byte */

uint8_t usart_receive(){
    
    while(!(UCSR0A&(1<<RXC0)));
        return UDR0;
}

//Send a string to ESP
void send_usart_string(char *c) {

    for(int i = 0; c[i] != '\0'; i++){
    
        usart_transmit(c[i]);
        
    }
}

//Receive a string from ESP
char * receive_usart_string() {
    char * data;
    int i = 0;
    while((data[i] = usart_receive()) != '\n') {
        i++; 
    }
    return data;
}

#define PCA9555_1_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz
//Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2

// PCA9555 REGISTERS
typedef enum {
 REG_INPUT_0 = 0,
 REG_INPUT_1 = 1,
 REG_OUTPUT_0 = 2,
 REG_OUTPUT_1 = 3,
 REG_POLARITY_INV_0 = 4,
 REG_POLARITY_INV_1 = 5,
 REG_CONFIGURATION_0 = 6,
 REG_CONFIGURATION_1 = 7,
} PCA9555_REGISTERS;

//----------- Master Transmitter/Receiver -------------------
#define TW_START 0x08
#define TW_REP_START 0x10

//---------------- Master Transmitter ----------------------
#define TW_MT_SLA_ACK 0x18
#define TW_MT_SLA_NACK 0x20
#define TW_MT_DATA_ACK 0x28

//---------------- Master Receiver ----------------
#define TW_MR_SLA_ACK 0x40
#define TW_MR_SLA_NACK 0x48
#define TW_MR_DATA_NACK 0x58

#define TW_STATUS_MASK 0b11111000
#define TW_STATUS (TWSR0 & TW_STATUS_MASK)

//initialize TWI clock
void twi_init(void)
{
 TWSR0 = 0; // PRESCALER_VALUE=1
 TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}

// Read one byte from the twi device ( request more data from device)
unsigned char twi_readNak(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}

// Issues a start condition and sends address and transfer direction.
// return 0 = device accessible, 1= failed to access device
unsigned char twi_start(unsigned char address)
{
 uint8_t twi_status;
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) return 1;
// send device address
TWDR0 = address;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wail until transmission completed and ACK/NACK has been received
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK) )
 {
 return 1;
 }
return 0;
}

// Send start condition, address, transfer direction.
// Use ack polling to wait until device is ready
void twi_start_wait(unsigned char address)
{
 uint8_t twi_status;
 while ( 1 )
 {
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

 // wait until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) continue;

 // send device address
 TWDR0 = address;
 TWCR0 = (1<<TWINT) | (1<<TWEN);

 // wail until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status == TW_MT_SLA_NACK )||(twi_status ==TW_MR_DATA_NACK) )
 {
 /* device busy, send stop condition to terminate write operation */
 TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

 // wait until stop condition is executed and bus released
 while(TWCR0 & (1<<TWSTO));

 continue;
 }
break;
 }
}

// Send one byte to twi device, Return 0 if write successful or 1 if write failed
unsigned char twi_write( unsigned char data )
{
// send data to the previously addressed device
TWDR0 = data;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
if( (TW_STATUS & 0xF8) != TW_MT_DATA_ACK) return 1;
return 0;
}

// Send repeated start condition, address, transfer direction
//Return: 0 device accessible
// 1 failed to access device
unsigned char twi_rep_start(unsigned char address)
{
 return twi_start( address );
}

// Terminates the data transfer and releases the twi bus
void twi_stop(void)
{
 // send stop condition
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
// wait until stop condition is executed and bus released
while(TWCR0 & (1<<TWSTO));
}

void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
 twi_start_wait(PCA9555_1_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_write(value);
 twi_stop();
}

uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
uint8_t ret_val;

 twi_start_wait(PCA9555_1_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_rep_start(PCA9555_1_ADDRESS + TWI_READ);
 ret_val = twi_readNak();
 twi_stop();

 return ret_val;

}

uint8_t scan_row() {
   
    uint8_t r;
    
    r = PCA9555_0_read(REG_INPUT_1); 
    r &= 0xF0;
    
    return r;
}

uint8_t scan_keypad() {
    
    uint8_t temp, res = 0xFF;
    
    PCA9555_0_write(REG_OUTPUT_1, 0xFE);     //Activate 1st row on keyboard 
    temp = scan_row();
    if (temp != 0xF0) res = (temp | 0x0E);
    
    PCA9555_0_write(REG_OUTPUT_1, 0xFD);     //Activate 2nd row on keyboard
    temp = scan_row();
    if (temp != 0xF0) res = (temp | 0x0D);
             
    PCA9555_0_write(REG_OUTPUT_1, 0xFB);     //Activate 3rd row on keyboard  
    temp = scan_row();
    if (temp != 0xF0) res = (temp | 0x0B);
            
    PCA9555_0_write(REG_OUTPUT_1, 0xF7);     //Activate 4th row on keyboard 
    temp = scan_row();
    if (temp != 0xF0) res = (temp | 0x07);

    return res;
    
}

uint8_t scan_keypad_rising_edge() {
    
    uint8_t temp1, temp2;
    
    temp1 = scan_keypad();
    _delay_ms(20);
    temp2 = scan_keypad();
    
    if(temp1 == temp2) {
        temp1 = 0;
    }
    return temp1;

}

uint8_t keypad_to_ascii() {
    
    uint8_t res, temp, temp1, i, j, k;
    
    char c[4][4] = {{'*','0','#','D'}, {'7','8','9','C'}, {'4','5','6','B'}, {'1','2','3','A'}};
    
    res = scan_keypad_rising_edge(); 
    temp = res;
    
    if (res == 0) return 0;
    if (res == 0xFF)  return 0;
    
    for (k = 0; k < 8; k++) {
        
        if ((temp1 = (temp & 0x01)) == 0x00) {
            if (k < 4)
                i = k;
            else 
                j = k;
        }
        temp = temp >> 1;
    }
    j = j - 4;
    
    return c[i][j];
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

float temperature_(){
    
    DDRD = 0xFF;                                //PORTD as output
    lcd_init();
    _delay_ms(2);
    
    int x, ak, temp, temp2;
    float res, res1;
    
    res = 0;
        
    if(one_wire_reset() == 0x01) { 
            
            x = count_temperature();
          
            DDRD = 0xFF;                        //output 
            lcd_init();                         // clear display 
            _delay_ms(2);
            
             
            res = x*0.0625;
            res = res + 12;                     //gia na einai konta sto 36
         
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
    
            res1 = res - ak;                    //afairw to akeraio, res: mono to dekadiko
    
            
                res1 = res1 * 10;               //gia thn emfanish tou dekadikou merous
                temp = res1;
                temp = temp | 0b00110000;
                lcd_data(temp);
                _delay_ms(2);
            
    
            _delay_ms(1000);
            
        }
    return res;
}
   

float count_pressure() {
    
    DDRC = 0x00; //PORTC as input
    
    //REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0000 => select AD0(pin PC0)
    //ADLAR=1 => Left adjust the ADC result
    ADMUX = 0b01100000;
    
    //ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    //ADIE=0 => disable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10000111;

    int ak, temp, temp2;
    float res, res1;
    
    lcd_data(' ');
        
    ADCSRA |= (1<<ADSC);                       //Set ADSC flag of ADCSRA
        
    while(ADCSRA & (1<<ADSC)){}                //Wait until ADSC flag of ADCSRA becomes 0 
        
                                               //Get ADC the Result
    res = ADCH * 20 / 256;
       
    ak = res;                                  //krataw mono to akeraio
    temp = ak / 100;        
            
    if (temp != 0 ) {                          //1o psifio 
      temp = temp | 0b00110000;                //an den einai 0 to emfanizw sthn othoni
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
    
            res1 = res - ak;                    //afairw to akeraio, res: mono to dekadiko
    
            
                res1 = res1 * 10;               //gia thn emfanish tou dekadikou merous
                temp = res1;
                temp = temp | 0b00110000;
                lcd_data(temp);
                _delay_ms(2);
            
            return res;
}

//Function that sends the payload to ESP(temperature from sensor and pressure from ADC)
void get_status(float temperature, float pressure){
 
 twi_init();

 DDRB = 0xFF; //PORTB as output
 uint8_t ch;
 
 int t1, t2, t3;
 int p1, p2, p3;
 
 PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set MSB(IO1_7-IO1_4) as input and LSB(IO1_0-IO1_3) as output
 
     while ((ch = keypad_to_ascii()) == 0) {}
        
	//Keeping results from temp and pres in variables
        t1 = temperature/10;	
        t2 = temperature;
        t2 = t2%10;
        t3 = temperature*10;
        t3 = t3%10;
        
        p1 = pressure/10;
        p2 = pressure;
        p2 = p2%10;
        p3 = pressure*10;
        p3 = p3%10;
        
        DDRD = 0xFF;
        if(status == 1) {	//Checking status according to temp/pres values
             if (ch == '#'){
                 if (temperature < 34.0 || temperature > 37.0) status = 3;
                 else if (pressure > 12.0 || pressure < 4.0) status = 2;
                 else status = 4;
             }
        }
        else {
            if(ch == '9')
                status = 1;
        }
 
        lcd_command(0xC0);		// allagi grammhs
        _delay_ms(40);
        
        if(status == 1) {		//Message to lcd screen according to status and pressed keyboard button  
            lcd_data('N');
            _delay_ms(2);
            lcd_data('U');
            _delay_ms(2);
            lcd_data('R');
            _delay_ms(2);
            lcd_data('S');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2);
            lcd_data(' ');
            _delay_ms(2);
            lcd_data('C');
            _delay_ms(2);
            lcd_data('A');
            _delay_ms(2);
            lcd_data('L');
            _delay_ms(2);
            lcd_data('L');
            _delay_ms(2000);
           
            payNurse[46] = t1 + '0';	//Transform digits into chars and putting them in the payload
            payNurse[47] = t2 + '0';
            payNurse[49] = t3 + '0' ;
            
            payNurse[83] = p1 + '0';
            payNurse[84] = p2 + '0';
            payNurse[86] = p3 + '0';

            send_usart_string(payNurse); //Sending the payload with renewed values to ESP 
        }
        else if(status == 2) {
            lcd_data('C');
            _delay_ms(2);
            lcd_data('H');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2);
            lcd_data('C');
            _delay_ms(2);
            lcd_data('K');
            _delay_ms(2);
            lcd_data(' ');
            _delay_ms(2);
            lcd_data('P');
            _delay_ms(2);
            lcd_data('R');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2);
            lcd_data('S');
            _delay_ms(2);
            lcd_data('S');
            _delay_ms(2);
            lcd_data('U');
            _delay_ms(2);
            lcd_data('R');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2000);
            
            payPress[46] = t1 + '0';
            payPress[47] = t2 + '0';
            payPress[49] = t3 + '0';
            
            payPress[83] = p1 + '0';
            payPress[84] = p2 + '0';
            payPress[86] = p3 + '0';

            send_usart_string(payPress); //Sending the payload with renewed values to ESP 
        }
        else if(status == 3) {
            lcd_data('C');
            _delay_ms(2);
            lcd_data('H');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2);
            lcd_data('C');
            _delay_ms(2);
            lcd_data('K');
            _delay_ms(2);
            lcd_data(' ');
            _delay_ms(2);
            lcd_data('T');
            _delay_ms(2);
            lcd_data('E');
            _delay_ms(2);
            lcd_data('M');
            _delay_ms(2);
            lcd_data('P');
            _delay_ms(2000);
            
            payTemp[46] = t1 + '0';
            payTemp[47] = t2 + '0';
            payTemp[49] = t3 + '0';
            
            payTemp[83] = p1 + '0';
            payTemp[84] = p2 + '0';
            payTemp[86] = p3 + '0';

            send_usart_string(payTemp); //Sending the payload with renewed values to ESP 
        }
        else if (status == 4){
            lcd_data('O');
            _delay_ms(2);
            lcd_data('K');
            _delay_ms(2000);
            
            payOK[46] = t1 + '0';
            payOK[47] = t2 + '0';
            payOK[49] = t3 + '0';
            
            payOK[83] = p1 + '0';
            payOK[84] = p2 + '0';
            payOK[86] = p3 + '0';

            send_usart_string(payOK); //Sending the payload with renewed values to ESP 
        }
                 
      
}

int main (void) {
    
    usart_init(103);	//initializing usart 
    status = 5;
    
    char *str;
    float tmp, prs;
    while(1){		//Looping for constant functioning 

    	send_usart_string(connect);	//Sending connect to ESP
        
    	str = receive_usart_string();	//Receiving ESP response  
        
    	if(str[0] == '"') { 
        
        	if(str[1] == success[1]) {
            
            	DDRD = 0xFF;                                
            	lcd_init();
            	_delay_ms(2);
            	lcd_data('1');
            	print_success();	//If response is "Success" --> show in lcd
            
        	}
        	else {
            
            	DDRD = 0xFF;                                
            	lcd_init();
            	_delay_ms(2);
            	lcd_data('1');
            	print_fail();		//If response is "Fail" --> show in lcd
            
        	}    
        
        _delay_ms(2000);		//Delay to appear on lcd		
    	}
    
    	send_usart_string(url);		//Sending url
        
    	str = receive_usart_string();	//Receiving ESP response  
    
    	if(str[0] == '"') { 
        
        	if(str[1] == success[1]) {
            
            	DDRD = 0xFF;                                
            	lcd_init();
            	_delay_ms(2);
            	lcd_data('2');
            	print_success();
            
        	}
        	else {
            
            	DDRD = 0xFF;                                
            	lcd_init();
            	_delay_ms(2);
            	lcd_data('2');
            	print_fail();  
            
        	}  
        
        _delay_ms(2000);		//Delay to appear on lcd
    	}             
    
    	DDRD = 0xFF;                    //PORTD as output
    	lcd_init();
    	_delay_ms(2);
   
    	tmp = temperature_();		//Get temperature from sensor				
    	prs = count_pressure();		//Get pressure from ADC
        
    	get_status(tmp, prs); 		//Put the values in the payload
        
    	str = receive_usart_string();	//Receive ESP response after sending the payload  

    	if(str[0] == '"') { 
        
    		if(str[1] == success[1]) {
            
        		DDRD = 0xFF;                                
                	lcd_init();
                	_delay_ms(2);
                	lcd_data('3');
                	print_success();
            
        	}
        	else {
            
        		DDRD = 0xFF;                                
                	lcd_init();
                	_delay_ms(2);
                	lcd_data('3');
                	print_fail();  
            
        	}    
     	}
        
     	_delay_ms(2000);		//Delay to appear on lcd
        
     	send_usart_string(transmit);	//Sending trasmit to ESP
        
     	str = receive_usart_string();	//Receive server's response
        
     	DDRD = 0xFF;                                
     	lcd_init();
     	_delay_ms(2);
       
     	print_server_response(str);	//Showing server's response on lcd
     	_delay_ms(2000);		//Delay to appear on lcd
     	DDRD = 0xFF;                                
     	lcd_init();
     	_delay_ms(2);
        
    }  
}