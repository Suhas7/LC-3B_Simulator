.ORIG x3000
ADD R6, R6, #7
LEA R4, tmpAddy
LDW R0, R4, #0
AND R5, R5, #0
ADD R5, R5, #1
STB R5, R0, #0
LEA R4, mask
LDW R5, R4, #0
LEA R4, startAddy
LDW R0, R4, #0
AND R1, R1, #0
ADD R1, R1, #10
ADD R1, R1, #10
AND R2, R2, #0
loop LDB R3, R0, #0
     AND R3, R3, R5
	 ADD R2, R2, R3
	 ADD R0, R0, #1
	 ADD R1, R1, #-1
	 BRp loop
LEA R0, destAddy
LDW R0, R0, #0
STW R2, R0, #0
HALT
tmpAddy .FILL x4000
startAddy .FILL xC000
destAddy .FILL xC017
mask .FILL x00FF
;**change to 0000 for protection except**
;change to C017 to test unaligned
.END
