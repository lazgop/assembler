.data

c DEF 65
f DEF c

a: DW 15

ORG 0x11

.text.12
 .global a
 LOAD R1, $a
 LOAD R2, #a
 LOAD R3, [R4]
 x: JZ R6, [R5]
.bss

.end


bla bla bla

- shvatiti sta tacno ide u tabelu relokacija za sekciju
- srediti izlaz da izgleda kao zahtevan
- videti sta se desava sa flagovima za sekcije
