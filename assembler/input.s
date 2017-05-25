.data

c DEF 65
f DEF c

a: DW 15

e: DW 11
ORG 0x11

.text.12
 .global a
 LOAD R1, $a
 LOAD R2, #a + (e-a)*2
 LOAD R3, [R4]
 x: JZ R6, [R5]
.bss

.end


bla bla bla

- shvatiti sta tacno ide u tabelu relokacija za sekciju
- promeniti sve upise u tabelu simbola da ne postoji -1 vec da ako ne pripada nijednoj sekciji to je UND sekcija sa indeksom 0
  - zajedno sa ovim treba promeniti sva mesta na kojim se upisuje u tabelu simbola
- promeniti da se u tabelu relokacija upisuje samo indeks sekcije u kojoj je definisan simbol i da je za lokalne simbole relativna relokacija
  - za globalne je apsolutna
  - potrebno je onda dodati na izraz vrednost pomeraja simbola u odnosu na sekciju u kojoj je definisan
  - pcrel rel.zapis je u odnosu na sledecu instrukciju, i zato na mesto gde nam se javlja simbol ........
- ubaciti znakove pitanja kao konstantan izraz prilikom definicije (valjda samo za DD,DW,DB)
- srediti izlaz da izgleda kao zahtevan
- videti sta se desava sa flagovima za sekcije


- ako nam je globalan simbol mi ga ostavljamo u rel.tab. (njegov indeks)
 i onda samo ugradimo u instrukciju adresut trenutnog locationcountera - adresu prve sledece instrukcije
 zato sto kazemo da je pcrel relokacija
- ako nam je lokalan simbol onda ugradjujemo u instrukciju njegovu vrednost + (adresut trenutnog locationcountera - adresu prve sledece instrukcije) a u rel.tab. stavljamo index njegove sekcije
 i isto koristimo indeks njegove sekcije
- kada koristimo apsolutnu relokaciju i sta ugradjujemo onda ????
- kaze ako imamo skok na labelu u istoj sekciji onda ne mora u rel.tab.
