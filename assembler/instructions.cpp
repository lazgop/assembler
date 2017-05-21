//
//  instructions.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include "instructions.h"

string Instruction::jumpIns[] = {"INT", "JMP", "CALL", "RET", "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"};
int Instruction::jumpInsLength = 10;
string Instruction::lsIns[] = {"LOAD", "LOADUB", "LOADSB", "LOADUW", "LOADSW", "STORE", "STOREB", "STOREW"};
int Instruction::lsInsLength = 8;
string Instruction::stackIns[] = {"PUSH", "POP"};
int Instruction::stackInsLength = 2;
string Instruction::alIns[] = {"ADD", "SUB", "MUL", "DIV", "MOD", "AND", "OR", "XOR", "NOT", "ASL", "ASR"};
int Instruction::alInsLength = 11;


