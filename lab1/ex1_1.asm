.include "m328PBdef.inc"	; delay of xmsec
    
reset:
    ldi r24, low(RAMEND)	; initialize stack pointer 1 cycle (1 usec)
    out SPL, r24		;  1 cycle (1 usec)
    ldi r24, high(RAMEND)	;  1 cycle (1 usec)
    out SPH, R24		;  1 cycle (1 usec)
    
    ldi r20, 0x14		;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    
loop_nop:
    
    nop				;  1 cycle (1 usec)
    dec r20			;  1 cycle (1 usec)
    brne loop_nop
    
    ldi r24, low(100)		;  1 cycle (1 usec) 
    ldi r25, high(100)		;  1 cycle (1 usec)
    rcall main			;  3 cycles (3 usec)
                    
main:
    rcall wait_x_msec		; 3 cycles (3 usec)
    rjmp end			; 2 cycles (2 usec)
    
wait_x_msec:
    rcall wait1m		; 3 cycles (3 usec)
    sbiw r24, 0x01		; 2 cycles (2 usec)
    brne wait_x_msec		; 1 or 2 cycles (1 or 2 usec) 
    ret				; 4 cycles (4 usec)
    
wait4:
    ret				; 4 cycles (4 usec)
    
wait1m:
    ldi r26, 98			; 1 cycle (1 usec) 
    
loop:
    rcall wait4			;  3+4=7 cycles = 7 usec
    dec r26			;  1 cycle (1 usec)
    brne loop			;  1 or 2 cycles (1 or 2 usec) 
    
    nop				;  1 cycle (1 usec)  
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    nop				;  1 cycle (1 usec)
    ret				;  4 cycles (4 usec)
    
end: