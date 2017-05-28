ORG 300
.data
a: DD 128, 12
ORG 128
.text.12
.global START
START:
 LOAD R1, #-25
 LOAD R2, #26
CALL subtr
 ADD R0, R1, R2
 LOAD R0, #0
JMP exit
subtr:
 SUB R0, R1, R2
 RET
exit:
  INT 0
.text.13
  SUB R0, R1, R2
.data.11
DD 30 DUP 8
.end
