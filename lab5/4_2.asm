.include "m328PBdef.inc"	; Definitions
.equ PD2=2
.equ PD3=3
.def temp=r16
.def ADC_L=r17
.def ADC_H=r18
.def temp2=r19 
.def flag=r22

.org 0x00
    rjmp reset	   
    
.org 0x2A
    rjmp ADC_ISR		; For ADC interrupt service routine
				
reset:
    ldi temp, high(RAMEND)
    out SPH, temp
    ldi temp, low(RAMEND)
    out SPL, temp
    
    ldi temp, 0xFF
    out DDRB, temp		; Set PORTB as output 
    
    ldi temp, 0x00
    out DDRC, temp		; Set PORTC as input(for ADC)
   
    ldi temp, 0xFF		; Set PORTD as output(for lcd)
    out DDRD, temp
    
    ; REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0011 => select ADC3(pin PC3),
    ; ADLAR=0 => Right adjust the ADC result 
    ldi temp, 0b01000011
    sts ADMUX, temp
    
    ; ADEN=1 => ADC Enable, ADSC=1 => Free Running Mode, ADATE=1 => Auto triggering ON
    ; ADIE=1 => Enable ADC interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ldi temp, 0b11101111
    sts ADCSRA, temp
    
    ; ADTS[2:0]= 000 => Free Running Mode
    ldi temp, 0b00000000
    sts ADCSRB, temp  
    
    clr flag
    
    rcall lcd_init
   
    ldi r24, low(2*16)		; Wait for 2msec
    ldi r25, high(2*16)
    rcall wait_msec
   
    sei
    
main:
    rjmp main			; loop here until ADC interrupt
    
ADC_ISR:			; Interrupt Service Routine for ADC
	  			; Where the magic happens
    lds ADC_L, ADCL		; Read ADC result
    lds ADC_H, ADCH
    
    cpi	ADC_L,low(205)		; If ADC > 70ppm(205) continue else
    ldi	temp2,high(205)		; 'CLEAR' and leds OFF
    cpc	ADC_H,temp2
    brsh cntn
    brlo level_0
    
cntn: 
    cpi flag, 0x01
    breq here 
    ldi flag, 0x01
    
    ldi r24 ,0x01		; clearing screen
    rcall lcd_command
    
    ldi r24 ,low(1530*16)	
    ldi r25 ,high(1530*16) 	   
    rcall wait_usec
    
    ldi r24, 0b01000111		; 'G'
    rcall lcd_data
    
    ldi r24, 0b01000001		; 'A'
    rcall lcd_data
    
    ldi r24, 0b01010011		; 'S'
    rcall lcd_data
 
    ldi r24, 0b00100000		; ' '
    rcall lcd_data
    
    ldi r24, 0b01000100		; 'D'
    rcall lcd_data
    
    ldi r24, 0b01000101		; 'E'
    rcall lcd_data
    
    ldi r24, 0b01010100		; 'T'
    rcall lcd_data
    
    ldi r24, 0b01000101		; 'E'
    rcall lcd_data
    
    ldi r24, 0b01000011		; 'C'
    rcall lcd_data
  
    ldi r24, 0b01010100		; 'T'
    rcall lcd_data
    
    ldi r24, 0b01000101		; 'E'
    rcall lcd_data
    
    ldi r24, 0b01000100		; 'D'
    rcall lcd_data

here:
  
    cpi	ADC_L,low(337)		
    ldi	temp2,high(337)	
    cpc	ADC_H,temp2	    
    brlo level_1
	
    cpi	ADC_L,low(469)		
    ldi	temp2,high(469)	
    cpc	ADC_H,temp2
    brlo level_2
    
    cpi	ADC_L,low(601)		
    ldi	temp2,high(601)	
    cpc	ADC_H,temp2
    brlo level_3
    
    cpi	ADC_L,low(733)		
    ldi	temp2,high(733)	
    cpc	ADC_H,temp2
    brlo level_4
    
    cpi ADC_L,low(865)		
    ldi	temp2,high(865)	
    cpc	ADC_H,temp2
    brlo level_5
    
    cpi ADC_L,low(865)		
    ldi	temp2,high(865)	
    cpc	ADC_H,temp2
    brsh level_6
    
level_0:			; ADC: 0 
    
    ldi r20, 0x01
    out PORTB, r20
    
    cpi flag, 0x01
    brne done
    clr flag
    
    ldi r24 ,0x01		; clearing screen
    rcall lcd_command
    
    ldi r24 ,low(1530*16)	
    ldi r25 ,high(1530*16) 	   
    rcall wait_usec
    
    ldi r24, 0b01000011		; 'C'
    rcall lcd_data
    
    ldi r24, 0b01001100		; 'L'
    rcall lcd_data
    
    ldi r24, 0b01000101		; 'E'
    rcall lcd_data
    
    ldi r24, 0b01000001		; 'A'
    rcall lcd_data
    
    ldi r24, 0b01010010		; 'R'
    rcall lcd_data
    
    rjmp done
    
level_1:			; ADC: 205-336 
    ldi r20, 0x01
    rjmp blink
	
level_2:			; ADC: 337-468 
    ldi r20, 0x03
    rjmp blink
	
level_3:			; ADC: 469-600 
    ldi r20, 0x07
    rjmp blink
    
level_4:			; ADC: 601-732
    ldi r20, 0x0F
    rjmp blink
    
level_5:			; ADC: 733-864 
    ldi r20, 0x1F
    rjmp blink
	    
level_6:			; ADC: 865-1023 
    ldi r20, 0x3F
    rjmp blink
    
 blink:
    mov r21, r20
    out PORTB, r21
    ldi r24 ,low(10*16)	
    ldi r25 ,high(10*16) 	   
    rcall wait_msec
    ldi r21, 0x00
    out PORTB, r21
    ldi r24 ,low(10*16)	
    ldi r25 ,high(10*16) 	   
    rcall wait_msec
	
done:
    reti 
   
lcd_data:
    sbi PORTD ,PD2		; PD2 = 1 => choose data register
    rcall write_2_nibbles	; Send byte
    
    ldi r24 ,low(43*16)	
    ldi r25 ,high(43*16) 	   
    rcall wait_usec		; 
    
    ret
    
write_2_nibbles:
    push r24			; sending 4 MSB
    in r25 ,PIND		; reading 4 LSB and resending them
    andi r25 ,0x0f		; de xalame previous state
    andi r24 ,0xf0		; krataw mono 4 MSB
    add r24 ,r25		; combine with 4 existing LSB
    out PORTD ,r24		; ready for output
    sbi PORTD, PD3		; Enable palmos ston PD3		
    cbi PORTD, PD3		; PD3 = 1 and then PD3 = 0
    pop r24			; sending 4 LSB. Receiving byte
    swap r24			; swapping LSB->MSB
    andi r24 ,0xf0		; pou me ti seira tous apostellontai
    add r24 ,r25		
    out PORTD,r24 
    sbi PORTD,PD3		; Neos palmos Enable 
    cbi PORTD,PD3 
    
    ret
    
lcd_command:
    cbi PORTD, PD2		; choosing instruction register (PD2 = 1)
    rcall write_2_nibbles	; send instruction snd wait for 39usec
    
    ldi r24 ,low(39*16)	
    ldi r25 ,high(39*16) 	   
    rcall wait_usec		; 
    
    ret
    
lcd_init:
    cli
    ldi r24 ,low(40*16)	
    ldi r25 ,high(40*16) 	   
    rcall wait_msec		; wait for 40msec
    
    ldi r24 ,0x30		; for 8-bit mode
    out PORTD ,r24		; cause we can never be too sure
    sbi PORTD ,PD3		; for input mode of lcd microcontroller
    cbi PORTD ,PD3		; so we send it twice
    ldi r24 ,low(39*16)		; wait for 39usec 
    ldi r25 ,high(39*16)		; 
    rcall wait_usec		; 
    
    ldi r24 ,0x30		
    out PORTD ,r24 
    sbi PORTD ,PD3 
    cbi PORTD ,PD3 
    ldi r24 ,low(39*16)		; wait for 39usec 
    ldi r25 ,high(39*16)		; 
    rcall wait_usec		; 
    
    ldi r24 ,0x20		; for 4-bit mode
    out PORTD ,r24 
    sbi PORTD ,PD3 
    cbi PORTD ,PD3 
    ldi r24 ,low(39*16)		; wait for 39usec 
    ldi r25 ,high(39*16)		; 
    rcall wait_usec		; 
    
    ldi r24 ,0x28		; characters 5x8 dots
    rcall lcd_command		; 2 lines showing on lcd
    
    ldi r24 ,0x0c		; screen ON/ cursor OFF
    rcall lcd_command
    
    ldi r24 ,0x01		; clearing screen
    rcall lcd_command
    
    ldi r24 ,low(1530*16)	
    ldi r25 ,high(1530*16) 	   
    rcall wait_usec
    
    ldi r24 ,0x06		; enabling automati auxisi kata 1 ths address 
    rcall lcd_command		; tou metriti address
				; disabling scaling of the whole screen
    ret
    
wait_msec:
    push r24 
    push r25 
    ldi r24 , low(998) 
    ldi r25 , high(998) 
    rcall wait_usec 
    
    pop r25 
    pop r24 
    sbiw r24 , 1 
    brne wait_msec 
    ret 
    
wait_usec:
    sbiw r24 ,1 
    nop 
    nop 
    nop 
    nop 
    brne wait_usec 
    ret