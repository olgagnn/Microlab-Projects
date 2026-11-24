.include "m328PBdef.inc"    ; ATmega328P microcontroller definitions
.equ FOSC_MHZ=16        ; Microcontroller operating frequency in MHz
.equ DEL_mS=500            ; Delay in mS (valid number from 1 to 4095)  
.equ DEL_NU=FOSC_MHZ*DEL_mS    ; delay_mS routine: (1000*DEL_NU+6) cycles
.DEF count=r22            ; Counting INT1 range(0-31)
    
;Instructions that enable INT0 interrupt (Sxima 2.3)
    
.org 0x0
rjmp reset 
.org 0x4            ; for INT1 --> 0x004 
rjmp ISR1            ; Routine for INT1
    
reset:
    
    ;PORTC as output
    ser r19
    out DDRC, r19
             
    ;Interrupt on rising edge of INT1 pin 
    ldi r24, (1 << ISC11) | (1 << ISC10)
    sts EICRA, r24
    
    ;Enable the INT1 interrupt(PD3)
    ldi r24, (1 << INT1)
    out EIMSK, r24
    
    ;Sets the Global Interrupt Flag
    sei

    ;Init Stack Pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ;Init PORTB as output
    ser r26
    out DDRB, r26
    
    ;Clearing count=r22
    clr count
    
loop1:
    clr r26
loop2:
    out PORTB, r26
    
    ldi r24, low(DEL_NU)    ; 
    ldi r25, high(DEL_NU)    ; Set delay (number of cycles)
    rcall delay_mS        ;
    
    inc r26
    
    cpi r26, 16            ; Compare r26 with 16
    breq loop1
    rjmp loop2
;Interrupt Service Routine (Sxima 2.3)    
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
    in r24, SREG        ; Save r24, r25, SREG
    push r24
    
    ;PORTD as input
    clr r24            
    out DDRD, r24        
    
    check:
    in r24, PIND
    com r24            
    andi r24, 0x80
    cpi r24, 0x80
    breq end        ; an einai patimeno
    
    rt:                ; an den einai patimeno
    inc count
    cpi count, 32        ; if r22 = 31 --> clear
    brne cntn
    clr count
    
    cntn:             
    out PORTC, count    ; PC4-PC0 as output
    
    end:
    pop r24            
    out SREG, r24        ; Restore r24, r25, SREG
    pop r24
    pop r25
    
    reti
    
;delay of 1000*F1+6 cycles (almost equal to 1000*F1 cycles)
delay_mS:
    ;total delay of next 4 instructions group = 1+(249*4-1) = 996 cycles
    ldi r23, 249        ; (1 cycle)
    loop_inn:
    dec r23            ; 1 cycle
    nop                ; 1 cycle
    brne loop_inn        ; 1 or 2 cycles
    
    sbiw r24, 1            ; 2 cycles
    brne delay_mS        ; 1 or 2 cycles
    
    ret                ; 4 cycles