.ORIG x1200
STW R0, R6, #-2
STW R1, R6, #-4
STW R2, R6, #-6
STW R3, R6, #-8
LEA R0, pagetablemask
LDW R1, R0, #0
LEA R0, count
LDW R3, R0, #0
LEA R0, pagetableaddy
LDW R0, R0, #0
loop LDW R2, R0, #0
	AND R2, R2, R1
	STW R2, R0, #0
	ADD R0, R0, #2
	ADD R3, R3, #-1
	BRp loop
LDW R0, R6, #-2
LDW R1, R6, #-4
LDW R2, R6, #-6
LDW R3, R6, #-8
RTI
pagetableaddy .FILL x1000
pagetablemask .FILL xFFFE
count .FILL x0080
.END
;7*41+22+2^7*(170)
;299 for start, 396  for start of int, 
;22563 for end of int