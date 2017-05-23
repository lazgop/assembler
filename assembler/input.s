.data

c DEF 65
f DEF c

ORG 0x11
.text.12
 .global a
 LOAD R1, f
 LOAD R2, #a
 LOAD R3, [R4]
 x: JZ R6, [R5]
.bss

.end


bla bla bla

- uraditi $
- uraditi isCalculatable i isConstExpr
- naci sve instrukcije/direktive gde se moze pojaviti simbol i proveravati da li ima simbol/simbol +/- expr i obratiti te slucajeve
- podesiti tako da svaka linija koda mora biti u odgovarajucoj sekciji (ne sme LOAD van text itd.)
- srediti da .bss sekcija mora da bude prazna u outputu
- shvatiti sta tacno ide u tabelu relokacija za sekciju
- srediti izlaz da izgleda kao zahtevan
- videti sta se desava sa flagovima za sekcije
