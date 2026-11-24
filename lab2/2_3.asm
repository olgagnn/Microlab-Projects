.include "m328PBdef.inc"
.equ FOSC_MHZ = 16
.equ DEL_mS = 500
.equ DEL_NU = FOSC_MHZ * DEL_mS
.DEF flag=r19
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1 
     
;energopoihsh INT1
reset: 
 
    ldi r24, (1<<ISC11) | (1<<ISC10)
    sts EICRA, r24
    ldi r24, (1<<INT1)
    out EIMSK, r24
    
    sei
    
    ;init stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ;Count for 4sec
    clr r22
    
    ;PORTB as output
    ser r26
    out DDRB, r26 
    
main:
    rjmp main
    
    ;eksuphrethsh diakopis
 ISR1:
    
    ;Avoid spinthirismo
    
    ;SET INTF1=0
    ldi r24, (1 << INTF1)    ; Set to 0 when in ISR1
    out EIFR, r24
    
    ldi r24, low(5*16)     
    ldi r25, high(5*16)        ; Set delay for e.g = 5msec (number of cycles)
    rcall delay_mS        
    
    in r24, EIFR        ; Read EIFR.1
    andi r24, 0x02        ; Isws gia apomonwsi tou EIFR.1 *Den epireazei
    cpi r24, 0            ; Compare with zero
    brne ISR1            ; If it's 0 repeat until it's not
  
    push r25
    push r24
    in r24, SREG
    push r24
    
    cpi flag, 0
    breq loop
    
    ;me endiameso patima
    ldi r21, 255
    out PORTB, r21
    ldi r24, low(16*500) ;set delay for 0,5s
    ldi r25, high(16*500)
    rcall delay_mS 
    
    ldi r21, 1
    out PORTB, r21
    
    ldi r17,7
    
loop_again:
    sei
    ldi r24, low(16*500) ;set delay for 0,5s
    ldi r25, high(16*500)
    rcall delay_mS 
    dec r17
    cpi r17,0
    brne loop_again
    rjmp end
    
;an den exoume endiamesi diakopi
loop:
    sei
    ser flag
    ldi r21, 1
    out PORTB, r21
    inc r22
    
    ldi r24, low(16*1000) ;set delay for 1s
    ldi r25, high(16*1000)
    rcall delay_mS 
    cpi r22, 04
    cli
    brne loop
    
 end:   
    clr r21
    out PORTB, r21
    
    clr r22
    clr flag
    pop r24
    out SREG, r24
    pop r24
    pop r25

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