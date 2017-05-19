.data

a: DD a, 2 DUP 234243+123
ORG 0x20

.text.12

 LOAD R1, a
 LOAD R2, #a
 LOAD R3, $a
 LOAD R3, [r4]
 x: JZ R0, [R1]
JZ R0, $x
.bss

.end


bla bla bla
