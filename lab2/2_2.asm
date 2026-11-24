.include "m328PBdef.inc"    ;ATmega328P microcontroller definitions
.equ FOSC_MHZ=16        ; Microcontroller operating frequency in MHz
.equ DEL_mS=600            ; Delay in mS (valid number from 1 to 4095)    
.equ DEL_NU=FOSC_MHZ*DEL_mS    ; delay_mS routine: (1000*DEL_NU+6) cycles
    
; Instructions that enable INT0 interrupt (Sxima 2.3)
    
.org 0x0
rjmp reset 
.org 0x2
rjmp ISR0
    
reset:
    ;Interrupt on rising edge of INT0 pin 
    ldi r24, (1 << ISC01) | (1 << ISC00)
    sts EICRA, r24
    
    ;Enable the INT0 interrupt(PD2)
    ldi r24, (1 << INT0)
    out EIMSK, r24
    
    ;Sets the Global Interrupt Flag
    sei
    
    ;Init Stack Pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ;Init PORTC as output
    ser r26
    out DDRC, r26
    
    ;Init PORTB as input
    clr r24
    out PORTB, r24     
    
loop1:
    clr r26
loop2:
    out PORTC, r26
    
    ldi r24, low(DEL_NU)     
    ldi r25, high(DEL_NU)    ; Set delay (number of cycles)
    rcall delay_mS        
    
    inc r26
    
    cpi r26, 32            ; compare r26 with 16
    breq loop1
    rjmp loop2
;Interrupt Service Routine (Sxima 2.3)    
ISR0:
    push r25            
    push r24            
    in r24, SREG        ; Save r24, r25, SREG
    push r24
    
    in r22, PINB
    
    andi r22, 0x3F        ;kratame mono PB0-PB5
    ldi r21, 6

    clr r19            ;counter
    clr r18            ;LEDs-C

    counting:
    ROR r22
    brcc next            ;an Carry=0 ->Next

    ;an carry = 1
    inc r19
    dec r21
    cpi r21,0
    breq go
    rjmp counting

   next: 
    dec r21
    cpi r21, 0
    breq go
    rjmp counting

    go:
    inc r19            ;px 4

    go1:
    lsr r18
    dec r19
    cpi r19, 0
    breq end
    ori r18, 0x80
    rjmp go1

    end:
    lsr r18
    com r18
    out PORTC, r18
  
    ldi r24, low(DEL_NU*4)    
    ldi r25, high(DEL_NU*4)    ; Set delay (number of cycles)
    rcall delay_mS        

 
    pop r24         
    out SREG, r24        ; Restore r24, r25, SREG
    pop r24
    pop r25

    reti
    
;delay of 1000*F1+6 cycles (almost equal to 1000*F1 cycles)
delay_mS:
    ; total delay of next 4 instructions group = 1+(249*4-1) = 996 cycles
    ldi r23, 249        ; (1 cycle)
    loop_inn:
    dec r23            ; 1 cycle
    nop            ; 1 cycle
    brne loop_inn        ; 1 or 2 cycles
    
    sbiw r24, 1        ; 2 cycles
    brne delay_mS        ; 1 or 2 cycles
    
    ret            ; 4 cycles