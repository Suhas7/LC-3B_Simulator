;suhas raja- scr2469
;simon hoque- th29344
.ORIG x3000
AND R0, R0, #0
ADD R0, R0, #4
LEA R1, start
LDW R1, R1, #0
loop LDB R2, R1, #0
    LDB R3, R1, #1
    STB R2, R1, #1
    STB R3, R1, #0
    ADD R1, R1, #2
    ADD R0, R0, #-1
    BRp loop
HALT
start .FILL x3050
.END