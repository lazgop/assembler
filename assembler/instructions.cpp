//
//  instructions.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

#include "instructions.h"
#include "keywordsutil.h"
#include "addressing.hpp"
#include "symboltable.h"
#include "symboltableentry.h"
#include "relocationtable.hpp"
#include "relocationtableentry.hpp"
#include "sections.hpp"
#include "inputline.h"

string Instruction::jumpIns[] = {"INT", "JMP", "CALL", "RET", "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"};
int Instruction::jumpInsLength = 10;
string Instruction::lsIns[] = {"LOAD", "LOADUB", "LOADSB", "LOADUW", "LOADSW", "STORE", "STOREB", "STOREW"};
int Instruction::lsInsLength = 8;
string Instruction::stackIns[] = {"PUSH", "POP"};
int Instruction::stackInsLength = 2;
string Instruction::alIns[] = {"ADD", "SUB", "MUL", "DIV", "MOD", "AND", "OR", "XOR", "NOT", "ASL", "ASR"};
int Instruction::alInsLength = 11;


Instruction::Instruction(string keyWord, string afterKeyword) {
   vector<string> operands = InputLine::splitOperands(afterKeyword);
   switch (getType(keyWord)) {
      case 0: { // JMP
         type = "JMP";
         switch (getJumpType(keyWord)) {
            case 0: // "INT"
               if (operands.size() != 1) {
                  cout << "Error: Mismatch number of operands for INT instruction!" << endl;
                  throw exception();
               }
               size = 4;
               break;
            case 1: case 2: // "JMP", "CALL"
               if (operands.size() != 1) {
                  cout << "Error: Mismatch number of operands for JMP/CALL instruction!" << endl;
                  throw exception();
               }
               if (Addressing::isRegisterIndirect(operands[0])) {
                  size = 4;
               } else {
                  size = 8;
               }
               break;
            case 3: // "RET"
               if (operands.size() != 0) {
                  cout << "Error: Mismatch number of operands for RET instruction!" << endl;
                  throw exception();
               }
               size = 4;
               break;
            case 4: case 5: case 6: case 7: case 8: case 9: {// "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"
               if (operands.size() != 2) {
                  cout << "Error: Mismatch number of operands for Conditional Jump instruction!" << endl;
                  throw exception();
               }
               string reg1 = operands[0];
               if (!isRegister(reg1)) {
                  cout << "Error: Conditional jump first operand must be a register!" << endl;
                  throw exception();
               }
               string op = operands[1];
               if (Addressing::isRegisterIndirect(op)) {
                  size = 4;
               } else {
                  size = 8;
               }
               break;
            }
         }
         break;
      }
      case 1: { // LS
         if (operands.size() != 2) {
            cout << "Error: Mismatch number of operands for Load/Store instruction!" << endl;
            throw exception();
         }
         type = "LS";
         string reg1 = operands[0];
         if (!isRegister(reg1)) {
            cout << "Error: Load/Store instructions first operand must be a register!" << endl;
            throw exception();
         }
         
         int lsInsType = getLSType(keyWord);
         
         if (Addressing::isImmediate(operands[1]) && (lsInsType == 5 || lsInsType == 6 || lsInsType == 7)) { // "STORE", "STOREB", "STOREW"
            cout << "Error: Cannot use immediate addressing in STORE instructions!" << endl;
            throw exception();
         }
         
         string op = operands[1];
         if (Addressing::isRegisterDirect(op)) {
            size = 4;
         } else if (Addressing::isRegisterIndirect(op)) {
            size = 4;
         } else {
            size = 8;
         }
         break;
      }
      case 2: { // STACK
         if (operands.size() != 1) {
            cout << "Error: Mismatch number of operands for Stack instruction!" << endl;
            throw exception();
         }
         string reg1 = operands[0];
         if (!isRegister(reg1)) {
            cout << "Error: Stack instructions first operand is not a register!" << endl;
            throw exception();
         }
         type = "STACK";
         size = 4;
         break;
      }
      case 3: { // AL
         if (operands.size() != 3) {
            cout << "Error: Mismatch number of operands for A/L instruction!" << endl;
            throw exception();
         }
         string reg0 = operands[0];
         string reg1 = operands[1];
         string reg2 = operands[2];
         if (!isRegister(reg1) || !isRegister(reg1) || !isRegister(reg2)) {
            cout << "Error: A/L instruction operands must be registers" << endl;
            throw exception();
         }
         type = "AL";
         size = 4;
         break;
      }
      default: {
         type = "NONE";
         break;
      }
   }
}
// ***************************
// ***************************
// ***************************
// ***************************
// ***************************
// ***************************
// ***************************
// ***************************
// ***************************

// Constructor for second pass
Instruction::Instruction(string keyWord, string afterKeyword, int lc) {
   instruction = vector<char>();
   this->locationCounter = lc;
   vector<string> operands = InputLine::splitOperands(afterKeyword);
   int instructionType = getType(keyWord);
   switch(instructionType){
      case 0: { // JMP
         type = "JMP";
         int jmpType = getJumpType(keyWord);
         switch (jmpType) {
            case 0: {// "INT"
               // Only register direct
               size = 4;
               instruction.push_back(0); // opcode
               // IVT has only 32 entries, so entry number can be coded directly into instruction in the place of reg0
               int intNum = getExpressionValue(operands[0]);
               instruction.push_back(intNum); // addressing and reg0
               instruction.push_back(0);
               instruction.push_back(0);
               locationCounter += 4;
               break;
            }
            case 1: case 2: {// "JMP", "CALL"
               instruction.push_back(jmpType + 1); // opcode
               
               // This func pushes part of instruction // addressing and reg0
               vector<int> arrayOfAllowedAddressings = vector<int>();
               arrayOfAllowedAddressings.push_back(2); //memdir
               arrayOfAllowedAddressings.push_back(3); // regind
               arrayOfAllowedAddressings.push_back(4); // regindpom
               int displacement = Addressing::resolveAddressingAndReturnDisplacement(operands[0], this, arrayOfAllowedAddressings);
               
               instruction.push_back(0); // byte 1
               instruction.push_back(0); // byte 0
               
               if (size == 8) { // only in this case we have displacement, otherwise it is ignored
                  for (int i = 0; i < 4; i++) {
                     int c = displacement >> (8*i);
                     instruction.push_back(char(c));
                  }
               }
               break;
            }
            case 3: // "RET"
               size = 4;
               instruction.push_back(1); // opcode
               instruction.push_back(0); // addressing
               instruction.push_back(0);
               instruction.push_back(0);
               break;
            case 4: case 5: case 6: case 7: case 8: case 9: {// "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"
               instruction.push_back(jmpType);
               
               vector<int> arrayOfAllowedAddressings = vector<int>();
               arrayOfAllowedAddressings.push_back(2); //memdir
               arrayOfAllowedAddressings.push_back(3); // regind
               arrayOfAllowedAddressings.push_back(4); // regindpom
               arrayOfAllowedAddressings.push_back(5); // $
               int displacement = Addressing::resolveAddressingAndReturnDisplacement(operands[1], this, arrayOfAllowedAddressings);
               
               int reg1 = getRegNumFromOp(operands[0]);
               instruction.push_back(reg1 << 3);
               instruction.push_back(0);
               
               if (size == 8) {
                  for (int i = 0; i < 4; i++) {
                     int c = displacement >> (8*i);
                     instruction.push_back(char(c));
                  }
               }
               break;
            }
         }
         break;
      }
      case 1: { // LS
         type = "LS";
         
         vector<int> arrayOfAllowedAddressings = vector<int>();
         switch (getLSType(keyWord)) {
            case 0: case 1: case 2: case 3: case 4: { // "LOAD", "LOADUB", "LOADSB", "LOADUW", "LOADSW"
               instruction.push_back(0x10);
               arrayOfAllowedAddressings.push_back(0); // immed
               break;
            }
            case 5: case 6: case 7: { // "STORE", "STOREB", "STOREW"
               instruction.push_back(0x11);
               break;
            }
            default:
               cout << "Unknown LS INSTRUCTION!" << endl;
               throw exception();
         }
         arrayOfAllowedAddressings.push_back(1); // regdir
         arrayOfAllowedAddressings.push_back(2); // memdir
         arrayOfAllowedAddressings.push_back(3); // regind
         arrayOfAllowedAddressings.push_back(4); // regindpom
         arrayOfAllowedAddressings.push_back(5); // $
         int displacement = Addressing::resolveAddressingAndReturnDisplacement(operands[1], this, arrayOfAllowedAddressings);
         
         int reg1 = getRegNumFromOp(operands[0]);
         instruction.push_back(reg1 << 3);
         instruction.push_back(getLSTypeCode(getLSType(keyWord)));
         
         if (size == 8) {
            for (int i = 0; i < 4; i++) {
               int c = displacement >> (8*i);
               instruction.push_back(char(c));
            }
            cout << endl;
         }
         break;
      }
      case 2: { // STACK
         size = 4;
         
         type = "STACK";
         if (keyWord == "PUSH") {
            instruction.push_back(0x20);
         } else {
            instruction.push_back(0x21);
         }
         instruction.push_back(getRegNumFromOp(operands[0]));
         instruction.push_back(0);
         locationCounter += 4;
         break;
      }
      case 3: { // AL
         size = 4;
         
         type = "AL";
         int alInsType = getALType(keyWord);
         instruction.push_back(0x30 | alInsType);
         
         int reg0 = getRegNum(operands[0]);
         int reg1 = getRegNum(operands[1]);
         int reg2 = getRegNum(operands[2]);
         
         int insRem = 0b000;
         insRem = (insRem << 8) + reg0;
         insRem = (insRem << 5) + reg1;
         insRem = (insRem << 5) + reg2;
         insRem = insRem << 6;
         
         for (int i = 0; i < 3; i++) {
            int c = insRem >> (8*(2-i));
            instruction.push_back(char(c));
         }
         locationCounter += 4;
         break;
      }
      default: {
         type = "NONE";
         break;
      };
   }
   
   if (type != "NONE") {
      if (Sections::entries.size() == 0) {
         cout << "Error: Trying to execute instruction outside of .text section!" << endl;
         throw exception();
      }
      Section currentSection = Sections::entries[Sections::entries.size() - 1];
      if ((SymbolTable::entries[currentSection.numID].flags.find("X") == string::npos)) {
         cout << "Error: Trying to execute instruction outside of .text section!" << endl;
         throw exception();
      }
   }
}

//*********************************
//*********************************
//*********************************
//*********************************
//*********************************

// Functions

int Instruction::getSize() {
   return size;
}

string Instruction::getType() {
   return type;
}

int Instruction::getType(string instruction) {
   for(int i=0; i<jumpInsLength; i++){
      if(instruction.compare(jumpIns[i]) == 0) {
         return 0;
      }
   }
   for(int i=0; i<lsInsLength; i++){
      if(instruction.compare(lsIns[i]) == 0) {
         return 1;
      }
   }
   for(int i=0; i<stackInsLength; i++){
      if(instruction.compare(stackIns[i]) == 0) {
         return 2;
      }
   }
   for(int i=0; i<alInsLength; i++){
      if(instruction.compare(alIns[i]) == 0) {
         return 3;
      }
   }
   return -1;
}


int Instruction::getJumpType(string instruction) {
   for (int i=0; i < jumpInsLength; i++) {
      if (instruction.compare(jumpIns[i]) == 0) {
         return i;
      }
   }
   return -1;
}

int Instruction::getLSType(string instruction) {
   for (int i=0; i < lsInsLength; i++) {
      if (instruction.compare(lsIns[i]) == 0) {
         return i;
      }
   }
   return -1;
}

int Instruction::getALType(string instruction) {
   for (int i=0; i < alInsLength; i++) {
      if (instruction.compare(alIns[i]) == 0) {
         return i;
      }
   }
   return -1;
}

int Instruction::getLSTypeCode(int lsInsType) {
   switch (lsInsType) {
      case 0: // LOAD
         return 0b00000000;
      case 1: // LOADUB
         return 0b00011000;
      case 2: // LOADSB
         return 0b00111000;
      case 3: // LOADUW
         return 0b00001000;
      case 4: // LOADSW
         return 0b00101000;
      case 5: // STORE
         return 0b00000000;
      case 6: // STOREB
         return 0b00111000;
      case 7: // STOREW
         return 0b00101000;
      default:
         break;
   }
   return -1;
}


