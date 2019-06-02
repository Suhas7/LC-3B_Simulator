.ORIG x3000
LEA R4, startAddy
LDW R0, R4, #0
AND R1, R1, #0
ADD R1, R1, #10
ADD R1, R1, #10
AND R2, R2, #0
loop
LDB R3, R0, #0
ADD R2, R2, R3
ADD R0, R0, #1
ADD R1, R1, # -1
BRp loop
LDW R0, R4, # 1
STW R2, R0, #0
tmpAddy .FILL 0x4000
startAddy .FILL 0xC000
destAddy .FILL 0xC014 
;**change to 0000 for protection except**
;change to C017 to test unaligned
.END