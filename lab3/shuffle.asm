.ORIG x3000
AND r0, r0, #0
ADD R0, R0, #10
ADD R0, R0, #-16
RSHFL r2, r0, #3
rshfa r1, r0, #3
lea r0, l1
ldw r1, r0, #0
ldw r2, r0, #2
stw r2, r0, #5
stw R1, R0, #-14cd
lea r0, l3
ldb r3, r0, #0
ldb r4, r0, #3
stb r3, r0, #7
stb r4, r0, #14

LSHF r3, r3, #6
RSHFL r3, r3, #3
RSHFA r3, r3, #3
lea r6, sub
jmp r6

sub add r0, r0, r1
    add r0, r0, r1
    add r0, r0, r1
    JSR sub

HALT

BR lab

HALT
l1 .FILL x0123
l2 .FILL x4567
l3 .FILL x89AB
l4 .FILL xCDEF

lab JSRR R6


; lsbmask .FILL x00FF
; msbmask .FILL xFF00
.END