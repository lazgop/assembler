.data

c DEF 65
f DEF c

a: DD 2 DUP 234243+123, a, c, printf
.global printf
ORG 20

.text.12
 .global a
 LOAD R1, a
 LOAD R2, #a
 LOAD R3, [R4]
 x: JZ R6, [R5]
.bss

.end


bla bla bla
