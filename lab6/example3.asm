.ORIG x3000 ;tests branches/loops and JSR, try doing JSRR too
ADD R0, R0, #14
ADD R1, R0, R0
ADD R3, R3, #7
BRp label
ADD R2,R2,#7
label ADD R3,R3,#-3
BRp label
LEA R0, testJSR
JSRR R0
ADD R4, R4, #8
HALT
testJSR ADD R6, R6, #4
RET
.END