.data

a: DD a
ORG 0x20

.text.12

 LOAD R1, a
 LOAD R2, #a
 LOAD R3, $a
 LOAD R3, [r4]
 x: JZ R0, [R1]
JZ R0, $x

.end


bla bla bla
