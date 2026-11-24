.include "m328PBdef.inc"

.org 0x0
rjmp reset
    
reset: 
    
    ;init stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
 
    ;set TMR1A in fast PWM 8bit mode
    ldi r24 ,(1<<WGM11)| (0<<WGM10) | (1<<COM1A1)
    sts TCCR1A, r24
    ldi r24 ,(1<<WGM13)| (1<<WGM12)
    sts TCCR1B, r24
    
    ;PORTB as output
    ldi r26, 0xFF ;(PB1)
    out DDRB, r26 
    
    ;PORTD as input
    ldi r26, 0 
    out DDRD, r26 
   
    
main:   
    ldi r24 , (1<<WGM13)| (1<<WGM12) 
    sts TCCR1B, r24
    
    in r22, PIND	;Checking PINS
    com r22
    cpi r22, 0x01   
    breq pd0
    
    cpi r22, 0x02
    breq pd1
    
    cpi r22,0x04
    breq pd2
    
    cpi r22, 0x08
    breq pd3
    
    rjmp main
    
pd0:
    ldi r24 , (1<<WGM13)| (1<<WGM12) |(0<<CS12) | (1<<CS11) | (0<<CS10) ; prescaler = 8
    sts TCCR1B, r24
    ldi r21, 0x3E
    sts ICR1H, r21	    ;TOP in ICR1
    ldi r21, 0x7F
    sts ICR1L, r21	    ;Fpwm = 125 
   
//OCR1A = ICR1 / 2 (50%)   
 
    ldi r20, 0xF
    sts OCR1AH, r20	    ;Duty Cycle in OCR1
    ldi r20, 0x3F
    sts OCR1AL, r20
    
    in r22, PIND
    com r22
    cpi r22, 0x01
    breq pd0
    
    rjmp main
    
    
pd1: 
    ldi r24 ,(1<<WGM13)| (1<<WGM12)| (0<<CS12) | (1<<CS11) | (0<<CS10) ; prescaler = 8
    sts TCCR1B, r24
    ldi r21, 0x1F
    sts ICR1H, r21
    ldi r21, 0x3F
    sts ICR1L, r21	;Fpwm = 250
   
   
    ldi r20, 0xF
    sts OCR1AH, r20
    ldi r20, 0x9F
    sts OCR1AL, r20
    
    in r22, PIND
    com r22
    cpi r22, 0x02
    breq pd1
    
    rjmp main
    
pd2:
    ldi r24 , (1<<WGM13)| (1<<WGM12)|(0<<CS12) | (1<<CS11) | (0<<CS10) ; prescaler = 8
    sts TCCR1B, r24
    
    ldi r21, 0xF
    sts ICR1H, r21
    ldi r21, 0x9F
    sts ICR1L, r21	    ;Fpwm = 500

    
    ldi r20, 0x7
    sts OCR1AH, r20
    ldi r20, 0xCF
    sts OCR1AL, r20
    
    in r22, PIND
    com r22
    cpi r22, 0x04
    breq pd2
    
    rjmp main
    
pd3:
    
    ldi r24 , (1<<WGM13)| (1<<WGM12)|(0<<CS12) | (1<<CS11) | (0<<CS10) ; prescaler = 8
    sts TCCR1B, r24
    
    ldi r21, 0x7
    sts ICR1H, r21
    ldi r21, 0xCF
    sts ICR1L, r21	    ;Fpwm = 1000
    
   
    ldi r20, 0x3
    sts OCR1AH, r20
    ldi r20, 0xE7
    sts OCR1AL, r20
    
    
    in r22, PIND
    com r22
    cpi r22, 0x08
    breq pd3
    
    
    
    rjmp main