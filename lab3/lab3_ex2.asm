.include "m328PBdef.inc"
.cseg
    
.org 0x0
rjmp reset
    
    
reset: 
    
    ;init stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
 
    ;set TMR1A in PWM 8bit mode
    ldi r24 ,(1<<WGM10) | (1<<COM1A1) | (1<<COM1B1) 
    sts TCCR1A, r24
    ldi r24 ,(0<<WGM12) | (1<<CS12) | (1<<CS10) ; prescaler = 1024
    sts TCCR1B, r24
    
    ;PORTB as output
    ldi r26, 0xFF   ;(PB0-PB5)
    out DDRB, r26 
    
    ;PORTD as input
    ldi r26, 0x00   ;(PD0-PD3)
    out DDRD, r26 
    
    ldi ZH,HIGH(Table*2) 
    ldi ZL,LOW(Table*2)
    
main:    
    ADIW ZL, 6  ;deixnoume sto 4o stoixeio
    lpm         ;diavasma LSB
		
    mov r24, r0
    sts OCR1AL, r24
    
 check:
    in r26, PIND
    push r24
    push r25
    ldi r24, low(16*200) ;set delay for 200ms to avoid spinthirismo
    ldi r25, high(16*200)
    rcall delay_mS 
    pop r25
    pop r24
    
    com r26
    cpi r26, 0x04   ; PD2
    breq pd2
    cpi r26, 0x02   ; PD1  
    brne check
    
    cpi r24, 0xF9	    ; An ftasei sto maximum DC stamataei 
    breq check		    ; kai kanei jump gia na synexisei elegxo 
    ADIW ZL, 1
    lpm
    mov r24, r0		    ; Otan patame PD1 auxanetai to DC 
    
    sts OCR1AL, r24
    rjmp check
    
pd2:
    cpi r24, 0x05	    ; An ftasei sto minimun DC stamataei 
    breq check              ; kai kanei jump gia na synexisei elegxo
    SBIW ZL, 1
    LPM 
    MOV r24, r0		    ; Otan patame PD2 meiwnetai to DC 
    
    sts OCR1AL, r24
    rjmp check

Table:			    ; Pinakas me times gia Duty Cycle
    .DW 0x1905, 0x422D, 0x6B56, 0x937F, 0xBCA8, 0xE5D1, 0x00F9 
    
delay_mS:
    sei
    ldi r23, 249
    loop_inn:
	dec r23
	nop
	brne loop_inn
    
    sbiw r24,1
    brne delay_mS
    cli
    
    ret