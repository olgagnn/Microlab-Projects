.include "m328PBdef.inc"
.equ FOSC_MHZ = 16
.equ DEL_mS = 500
.equ DEL_NU = FOSC_MHZ * DEL_mS
.def flag = r19
    
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1 
.org 0x1A
rjmp ISR_TIMER1_OVF ; timer1
     
;Energopoihsh INT1 & TCNT1
reset:  
    
    ;init stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ;Energopoihsh diakopis INT1
    ldi r24, (1<<ISC11) | (1<<ISC10)
    sts EICRA, r24
    ldi r24, (1<<INT1)
    out EIMSK, r24
   
    ;Energopoihsh diakopis overflow gia timer1
    ldi r24, (1<<TOIE1) 
    sts TIMSK1, r24     
    
    sei 
   
    ;PORTB as output
    ser r24
    out DDRB, r24 
    
    ;PORTC as input
    clr r26
    out DDRC, r26
    
    ;Flag gia kanoniko blink/endiameso blink
    clr flag
    
main:
   
    sei 
    
    in r26, PINC
    push r24
    push r25
    ldi r24, low(16*200) ;set delay for 100ms
    ldi r25, high(16*200)
    rcall delay_mS 
    pop r25
    pop r24
    com r26
    andi r26, 0x20  ;loop sti main mexri na patisoume PC5
    cpi r26, 0x20    
    brne main        
 
    cpi flag, 0        
    brne endiameso      ;an flag = 1, endiameso blink(0,5sec ola ta leds/3,5 to PB0 
    
    ;xwris endiamesh diakopi/patima PC5 
    sei
     
    ldi r17, 1
    out PORTB, r17	; PB0 --> ON
     
    ldi r24 ,(1<<CS12) | (0<<CS11) | (1<<CS10) ; CK/1024 
    sts TCCR1B, r24
    ldi r24, HIGH(3035) ;Arxikopoihsh tou TCNT1
    sts TCNT1H, r24     ;Gia uperxeilisi meta apo 4 sec 
    ldi r24, LOW(3035)  ;4×15625 - 46875
    sts TCNT1L, r24
    ser flag
    
    rjmp main
   
    ;endiamsh diakopi 
endiameso:
    ldi r17, 255
    out PORTB, r17
    ldi r24, low(16*500)    ;set delay for 0,5s
    ldi r25, high(16*500)
    rcall delay_mS 
    
       
    ldi r17, 1
    out PORTB, r17
    
    ldi r24 ,(1<<CS12) | (0<<CS11) | (1<<CS10) ; CK/1024 
    sts TCCR1B, r24
    ldi r24, HIGH(10848)    ; Arxikopoihsh tou TCNT1
    sts TCNT1H, r24	    ; Gia uperxeilisi meta apo 4 sec 
    ldi r24, LOW(10848)	    ; (3,5×15625) - 46875
    sts TCNT1L, r24	
 
    rjmp main
    
;eksuphrethsh diakopis
 ISR1:
    
    push r25
    push r24
    in r24, SREG
    push r24
    
    cpi flag, 0
    breq loop
    
    ;me endiameso patima
    ldi r17, 255
    out PORTB, r17
    ldi r24, low(16*500)    ;set delay for 0,5s
    ldi r25, high(16*500)
    rcall delay_mS 
    
    ldi r24 ,(1<<CS12) | (0<<CS11) | (1<<CS10) ; CK/1024 
    sts TCCR1B, r24
    ldi r24, HIGH(10848)    ; Arxikopoihsh tou TCNT1
    sts TCNT1H, r24	    ; Gia uperxeilisi meta apo 4 sec 
    ldi r24, LOW(10848)	    ; (3,5×15625) - 46875
    sts TCNT1L, r24	
    
    ldi r17, 1
    out PORTB, r17
    
    rjmp end
    
;an den exoume endiamesi diakopi
loop:
    sei
    ser flag
     
    ldi r24 ,(1<<CS12) | (0<<CS11) | (1<<CS10) ; CK/1024 
    sts TCCR1B, r24
    ldi r24, HIGH(3035) ;Arxikopoihsh tou TCNT1
    sts TCNT1H, r24     ;Gia uperxeilisi meta apo 4 sec 
    ldi r24, LOW(3035)  ;4×15625 - 46875
    sts TCNT1L, r24
    
    ldi r17, 1
    out PORTB, r17	; PB0 --> ON
    
 end:   
  
    pop r24
    out SREG, r24
    pop r24
    pop r25

    reti
      
;Eksuphrethsh overflow timer1
ISR_TIMER1_OVF:
    
    clr r16	    
    out PORTB, r16  ;svisimo tou PB0
   
    clr flag	    ;init flag
  
    reti  
    
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