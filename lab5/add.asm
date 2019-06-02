.ORIG x3000
LEA R4, mask ;22
LDW R5, R4, #0;41
LEA R4, startAddy;22
LDW R0, R4, #0;41
AND R1, R1, #0;22
ADD R1, R1, #10;22
ADD R1, R1, #10;22
AND R2, R2, #0;22 - 277 completed after this
loop LDB R3, R0, #0;41 starts at 
     AND R3, R3, R5;22
	 ADD R2, R2, R3;22
	 ADD R0, R0, #1;22
	 ADD R1, R1, #-1;22
	 BRp loop		;23
LEA R0, destAddy
LDW R0, R0, #0
STW R2, R0, #0
JMP R2
HALT
tmpAddy .FILL x3001
startAddy .FILL xC000
destAddy .FILL xC014
mask .FILL x00FF
;**change to 0000 for protection except**
;change to C017 to test unaligned
.END
