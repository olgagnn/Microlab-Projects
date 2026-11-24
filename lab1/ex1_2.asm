.include "m328PBdef.inc"

.DEF A=r16		; Define the registers
.DEF AN=r17
.DEF B=r18
.DEF BN=r19
.DEF C=r20
.DEF D=r21
.DEF DN=r22
.DEF F=r23
.DEF F0=r24
.DEF F1=r25
.cseg 
.org 0 

ldi r26, 0x06		; Initialize the registers 
ldi A, 0x55
ldi B, 0x43
ldi C, 0x22
ldi D, 0x02

START:
    mov AN, A		
    com AN
    mov BN, B
    com BN
    mov DN, D
    com DN
    mov F0, A
    and F0, DN
    or F0, B		; Compute ((A'B')+(B'D))'--> AD'+B
    mov F, A
    or F, C
    mov F1, B
    or F1, DN
    and F1, F		; Compute (A+C)(B+D')
    dec r26		; Repeat the computing of F0 & F1 6 times
    breq END
    brne UP_AND_LOOP
    
UP_AND_LOOP:
    subi A, -0x02	; Increase registers' values at every loop
    subi B, -0x03
    subi C, -0x04
    subi D, -0x05
    rjmp START
    
END: