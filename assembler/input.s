.data
n: DW 10000
ORG 0x100
.text
.global START
 START:
 LOAD R1, #timer ; get address of timer routine
 STORE R1, 4*4 ; store address of timer routine in IVT entry 4
 LOAD R1, #1
loop:
 LOADSW R0, n
 SUB R0, R0, R1
 STOREW R0, n
 JNZ R0, loop
 INT 0

timer:
  LOAD R4, #4
  RET
.end
