.data

c DEF 65
f DEF 2

a: DD 2 DUP 234243+123, a, c, printf
.global printf
ORG 20

.text.12
 .global a
 LOAD R1, a
 LOAD R2, #a
 LOAD R3, [r4]
 x: JZ R0, [R1]
.bss

.end


bla bla bla
