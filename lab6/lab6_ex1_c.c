#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
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
 REG_CONFIGURATION_1 = 7
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

void twi_init(void)
{
 TWSR0 = 0; // PRESCALER_VALUE=1
 TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}

// Read one byte from the twi device (request more data from device)
unsigned char twi_readAck(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}

//Read one byte from the twi device, read is followed by a stop condition
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
 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_write(value);
 twi_stop();
}
uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
uint8_t ret_val;

 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_rep_start(PCA9555_0_ADDRESS + TWI_READ);
 ret_val = twi_readNak();
 twi_stop();

 return ret_val;
}

/*NEW CODE*/
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

/*END OF NEW CODE*/

int main(void) {
    
    DDRD = 0xFF;     //PORTD as output for lcd screen 
    
    uint8_t ch;
    twi_init();
    lcd_init();
    _delay_ms(2);
  
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set MSB(IO1_7-IO1_4) as input and LSB(IO1_0-IO1_3) as output
    
    while(1) {
        ch = keypad_to_ascii();
        if (ch != 0) {
            lcd_command(0x01);		// clear display 
            _delay_ms(2);
            lcd_data(ch);
        }
        
    }

}