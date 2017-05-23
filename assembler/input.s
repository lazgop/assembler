.data

c DEF 65
f DEF c

a: DD 2 DUP 2 + c, a, c, printf
.global printf

.text.12
 .global a
 LOAD R1, f
 LOAD R2, #a
 LOAD R3, [R4]
 x: JZ R6, [R5]
.bss

.end


bla bla bla
