.include "m328PBdef.inc"
    
ldi r16, 0x01		; T = 1 for left-right
bst r16, 0x01
clr r17
clr r18 
out DDRD, r18		;r18 output
inc r17
out PORTD, r17
ldi r24, 0xF4		; low byte 
ldi r25, 0x01		; high byte 
rcall delay		; delay of 0.5 sec
rjmp againl
    
toMSB: 
    out PORTD, r17
    ldi r24, 0xDC	; low byte 
    ldi r25, 0x05	; high byte 
    rcall delay		; delay of 1.5 sec

againl: 
    lsl r17
    cpi r17, 0x80
    breq toLSB
    out PORTD,r17
    ldi r24, 0xF4	; low byte 
    ldi r25, 0x01	; high byte 
    rcall delay		; delay of 0.5 sec
    rjmp againl

toLSB:
    clr r16		; T = 0 for right-left
    bst r16, 0x01
    out PORTD, r17
    ldi r24, 0xDC	; low byte 
    ldi r25, 0x05	; high byte 
    rcall delay		; delay of 1.5 sec

againr:
    lsr r17
    cpi r17, 0x01
    breq toMSB
    out PORTD, r17
    ldi r24, 0xF4	    ; low byte 
    ldi r25, 0x01	    ; high byte 
    rcall delay		    ; delay of 0.5 sec
    rjmp againr
    
delay:
    
    main:
	rcall wait_x_msec   ; 3 cycles (3 usec)
	rjmp done	    ; 2 cycles (2 usec)
    
    wait_x_msec:
	rcall wait1m	    ; 3 cycles (3 usec)
	sbiw r24, 0x01	    ; 2 cycles (2 usec)
	brne wait_x_msec    ; 1 or 2 cycles (1 or 2 usec) 
	ret		    ; 4 cycles (4 usec)
    
    wait4:
	ret		    ; 4 cycles (4 usec)
    
    wait1m:
	ldi r26, 99	    ; 1 cycle (1 usec) 
    
    loop:
	rcall wait4	    ;  3+4=7 cycles = 7 usec
	dec r26		    ;  1 cycles (1 usec)
	brne loop	    ;  1 or 2 cycles (1 or 2 usec) 
    
	nop		    ;  1 cycle (1 usec)  
	nop		    ;  1 cycle (1 usec)
	nop		    ;  1 cycle (1 usec)
	ret		    ;  4 cycles (4 usec)
    
    done:
	ret