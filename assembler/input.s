.data

c DEF 65
f DEF c
i: DD 71, 6 DUP c, 12 DUP ?

.global q, a

ORG 0x50

.text.12
e: DW 11
 LOAD R1, $a
 a: DW 15
 LOAD R2, #a + (e-a)*2
 LOAD R3, [R4]
 x: JZ R6, [R5]
 j: DW ?
.bss

.end
