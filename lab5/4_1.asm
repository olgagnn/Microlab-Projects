.include "m328PBdef.inc"
    
.def temp = r16 
.def temp2 = r17
.def ADC_L = r21 
.def ADC_H = r22
.def counter = r19
.equ PD2 = 2
.equ PD3 = 3   
    
.org 0x00
    jmp reset   

.org 0x2A 
    rjmp ADC_ISR
 
reset:
    
    ;init stack pointer
    ldi temp, high(RAMEND)
    out SPH,temp
    ldi temp, low(RAMEND) 
    out SPL,temp
    
    ;PORTB as output
    ldi temp, 0xFF 
    out DDRB, temp
    
    ;PORTD as output
    ldi temp, 0xFF 
    out DDRD, temp
    
    ;PORTC as input
    ldi temp, 0x00 
    out DDRC, temp
    
    clr counter

    ;REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0010 => select ADC2(pin PC2)
    ; ADLAR=1 => Left adjust the ADC result
    ldi temp, 0b01100010 
    sts ADMUX, temp

    ; ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    ; ADIE=1 => enable adc interrupt, ADPS[2:0]=11 => fADC=16MHz/128=125KHz
    ldi temp, 0b10001111
    sts ADCSRA, temp
    
    ; ADTS[2:0]= 000 => Free Running Mode
    ldi temp, 0b00000000
    sts ADCSRB, temp
    
    rcall lcd_init
   
    ldi r24, low(2*16)		; Wait for 2msec
    ldi r25, high(2*16)
    rcall wait_msec
    
    sei
    
main: 
    
    lds temp, ADCSRA	 ;ksekinaei h metatropi
    ori temp, 0b01100000 ;ADSC = 1 && ADATE = 1
    sts ADCSRA, temp 
    
    
    inc counter 
    cpi counter, 255
    brne here
    clr counter
    
    here:
    out PORTB, counter
    
    ldi r24 ,0x01		; clearing screen
    rcall lcd_command
    
    ldi r24 ,low(1530*16)	
    ldi r25 ,high(1530*16) 	   
    rcall wait_usec
   
    ldi r18, 5
    mul r18, ADC_H ;r0,r1 <-ADC x 5 
    mov ADC_L, r0
    mov ADC_H, r1
   
    mov temp2,ADC_H
    ori temp2, 0b00110000  
    mov r24, temp2
    rcall lcd_data	;emfanish 1ou arithmou
    
    clr ADC_H
    ldi temp2, 0b00101110
    mov r24, temp2
    rcall lcd_data	;emfanish '.'
    
    ldi r18, 10
    mul r18, ADC_L      ;ADC_L x 10
    mov ADC_L, r0
    mov ADC_H, r1
    
    mov temp2,ADC_H
    ori temp2, 0b00110000
    mov r24, temp2
    rcall lcd_data	;emfanish 2ou arithmou
    
    clr ADC_H
    ldi r18, 10
    mul r18, ADC_L	; ADC_L x 10
    mov ADC_L, r0
    mov ADC_H, r1
  
    mov temp2,ADC_H
    ori temp2, 0b00110000
    mov r24, temp2
    rcall lcd_data	;emfanish 3ou arithmou
   
   
    sei
    
    ldi r24 ,low(150*16)	
    ldi r25 ,high(150*16) 	   
    rcall wait_msec
    
    rjmp main

;Routina eksuphrethshs diakopis oloklirosis metatropis
ADC_ISR:
    
    lds ADC_H,ADCH 
   
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
    rcall wait_usec		 
    
    ret
    
lcd_init:
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