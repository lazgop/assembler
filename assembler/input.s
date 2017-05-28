ORG 128
.data
a: DD 128, 12
.text.12
.global START
START:
 LOAD R1, #-25
 PUSH R1
 POP R2
 STORE R1, $a
 LOAD R2, a
 LOAD R3, #a
 LOAD R4, a + 4
 STORE R1, [R3 + 4]
 STORE R1, R3
 LOAD R4, a + 4
 INT 0
.end
