.data
n: DD 5555
ORG 0x100
.text
.global START
 START:
 LOAD R1, #timer ; get address of timer routine
 STORE R1, 4*4 ; store address of timer routine in IVT entry 4
 LOAD R1, #keyboard ; get address of keyboard routine
 STORE R1, 5*4 ; store address of keyboard routine in IVT entry 5
 LOAD R1, #error ; get address of error routine
 STORE R1, 3*4 ; store address of error routine in IVT entry 3
 LOAD R1, #65
 STOREB R1, 32*4
 LOAD R1, #66
 STOREB R1, 32*4
 LOAD R1, #48 ; store ascii for '0' in R1
 NOT R2, R2
loop: ; loop until '0' is entered on keyboard
 SUB R0, R1, R5
 JNZ R0, loop
 INT 0

error:
  INT 0

timer:
  RET

keyboard:
  ;LOADUB R5, 33*4 ; read char from inputreg
  RET
.end
