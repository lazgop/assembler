.text
START:
.global START
LOAD R0, currentTimeCounter
LOAD R1, #1
ADD R0, R0, R1
STORE R0, currentTimeCounter
LOAD R2, currentTimeCounter
SUB R2, R2, R1

JNZ R2, kraj

LOAD R0, outputRegAddress
LOAD R2, #'O'
STORE R2, [R0]

LOAD R0, rodatavalue
LOAD R1, #1
ADD R0, R0, R1
STORE R0, rodatavalue
LOAD R2, rodatavalue
SUB R2, R2, R1
JZ R2, kraj

LOAD R0, outputRegAddress
LOAD R2, #'K'
STORE R2, [R0]

kraj:
INT 0

.data
currentTimeCounter: DD 0

.rodata

rodatavalue: DD 0
inputRegAddress: DD 33*4
outputRegAddress: DD 32*4
timerAddress: DD 4*4
errorAddress: DD 3*4
keyboardAddress: DD 5*4
