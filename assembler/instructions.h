//
//  instructions.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef instructions_hpp
#define instructions_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "keywordsutil.h"
#include "addressing.hpp"
#include "symboltable.h"
#include "symboltableentry.h"
#include "relocationtable.hpp"
#include "relocationtableentry.hpp"
#include "sections.hpp"
#include "inputline.h"

using namespace std;

class Instruction{
   static string jumpIns[];
   static int jumpInsLength;
   static string lsIns[];
   static int lsInsLength;
   static string stackIns[];
   static int stackInsLength;
   static string alIns[];
   static int alInsLength;
   
   string type;
   int size;
   
   int locationCounter;
public:
   vector<char> instruction;
   Instruction(string keyWord, string afterKeyword) {
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
                  if (!isRegister(operands[0])) {
                     cout << "Error: INT instruction operand must be a register!" << endl;
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
   Instruction(string keyWord, string afterKeyword, int lc) {
      instruction = vector<char>();
      this->locationCounter = lc;
      vector<string> operands = InputLine::splitOperands(afterKeyword);
      switch(getType(keyWord)){
         case 0: { // JMP
            type = "JMP";
            int jmpType = getJumpType(keyWord);
            switch (jmpType) {
               case 0: {// "INT"
                  // Only register direct
                  size = 4;
                  instruction.push_back(0);
                  int reg0 = getRegNumFromOp(operands[0]);
                  instruction.push_back(reg0);
                  instruction.push_back(0);
                  instruction.push_back(0);
                  locationCounter += 4;
                  break;
               }
               case 1: case 2: {// "JMP", "CALL"
                  instruction.push_back(jmpType + 1);
                  switch (Addressing::getType(operands[0])) {
                     case 2:{ // MemDir
                        size = 8;
                        
                        instruction.push_back(0b11000000);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        locationCounter += 4;
                        
                        int addr = getMemoryDirectAddress(operands[0]);
                        for (int i = 0; i < 4; i++) {
                           int c = addr >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 3: {// RegInd
                        size = 4;
                        
                        int adrAndReg = 0b010;
                        int reg0 = getRegNumFromOp(operands[0]);
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        locationCounter += 4;
                        break;
                     }
                     case 4:{ // RegIndPom
                        size = 8;
                        
                        int adrAndReg = 0b111;
                        
                        string offsetString;
                        int reg0 = getRegNumFromOp(operands[0], &offsetString);
                        
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int offset = getExpressionValue(offsetString);
                        for (int i = 0; i < 4; i++) {
                           int c = offset >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 5: { //PCRelPom --- $
                        size = 8;
                        // TODO: - Finish this case
                        break;
                     }
                     default:
                        cout << "Error: Unknown addressing in " + keyWord + " instruction!" << endl;
                        throw exception();
                  }
                  break;
               }
               case 3: // "RET"
                  size = 4;
                  instruction.push_back(1);
                  instruction.push_back(0);
                  instruction.push_back(0);
                  instruction.push_back(0);
                  break;
               case 4: case 5: case 6: case 7: case 8: case 9: {// "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"
                  instruction.push_back(jmpType);
                  int reg1 = getRegNumFromOp(operands[0]);
                  string op = operands[1];
                  switch (Addressing::getType(op)) {
                     case 2:{ // MemDir
                        size = 8;
                        instruction.push_back(0b11000000);
                        instruction.push_back(reg1 << 3);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int addr = getMemoryDirectAddress(op);
                        for (int i = 0; i < 4; i++) {
                           int c = addr >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 3: {// RegInd
                        size = 4;
                        int adrAndReg = 0b010;
                        int reg0 = getRegNumFromOp(op);
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(reg1 << 3);
                        instruction.push_back(0);
                        break;
                     }
                     case 4:{ // RegIndPom
                        size = 8;
                        int adrAndReg = 0b111;
                        string offsetString;
                        int reg0 = getRegNumFromOp(op, &offsetString);
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(reg1 << 3);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int offset = getExpressionValue(offsetString);
                        for (int i = 0; i < 4; i++) {
                           int c = offset >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 5: { // PCRelPom -- $
                        size = 8;
                        // TODO: - Finish this case
                        break;
                     }
                     default:
                        cout << "Error: Unknown addressing in " + keyWord + " instruction!" << endl;
                        throw exception();
                  }
                  break;
               }
               default:
                  break;
            }
            break;
         }
         case 1: { // LS
            type = "LS";
            int lsInsType = getLSType(keyWord);
            switch (lsInsType) {
               case 0: case 1: case 2: case 3: case 4: { // "LOAD", "LOADUB", "LOADSB", "LOADUW", "LOADSW"
                  instruction.push_back(0x10);
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
            int reg1 = getRegNum(operands[0]);
            string op = operands[1];
            switch (Addressing::getType(op)) {
               case 0: {// Immed
                  size = 8;
                  
                  instruction.push_back(0b10000000);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int value = getMemoryDirectAddress(op.substr(1, op.length() - 1));
                  for (int i = 0; i < 4; i++) {
                     int c = value >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter +=4;
                  break;
               }
               case 1: { // RegDir
                  size = 4;
                  
                  int adrAndReg = 0b000;
                  int reg0 = getRegNumFromOp(op);
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  break;
               }
               case 2:{ // MemDir
                  size = 8;
                  
                  instruction.push_back(0b11000000);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int addr = getMemoryDirectAddress(op);
                  for (int i = 0; i < 4; i++) {
                     int c = addr >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter += 4;
                  break;
               }
               case 3: {// RegInd
                  size = 4;
                  
                  int adrAndReg = 0b010;
                  int reg0 = getRegNumFromOp(op);
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  break;
               }
               case 4:{ // RegIndPom
                  size = 8;
                  
                  int adrAndReg = 0b111;
                  string offsetString;
                  int reg0 = getRegNumFromOp(op, &offsetString);
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int offset = getExpressionValue(offsetString);
                  for (int i = 0; i < 4; i++) {
                     int c = offset >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter += 4;
                  break;
               }
               case 5: { // PCRELPOM $
                  size = 8;
                  //  TODO: - Finish this case
                  break;
               }
               default:
                  break;
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
   }
   
   //*********************************
   //*********************************
   //*********************************
   //*********************************
   //*********************************
   
   // Functions
   
   int getSize() {
      return size;
   }
   
   string getType() {
      return type;
   }
   
   int getType(string instruction) {
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
   
   
   int getJumpType(string instruction) {
      for (int i=0; i < jumpInsLength; i++) {
         if (instruction.compare(jumpIns[i]) == 0) {
            return i;
         }
      }
      return -1;
   }
   
   int getLSType(string instruction) {
      for (int i=0; i < lsInsLength; i++) {
         if (instruction.compare(lsIns[i]) == 0) {
            return i;
         }
      }
      return -1;
   }
   
   int getALType(string instruction) {
      for (int i=0; i < alInsLength; i++) {
         if (instruction.compare(alIns[i]) == 0) {
            return i;
         }
      }
      return -1;
   }
   
   int getMemoryDirectAddress(string word) {
      if (isValidString(word)){
         bool labelFound = false;
         int labelLocation = -1;
         for (int i=0; i < SymbolTable::entries.size(); i++) {
            if (word.compare(SymbolTable::entries[i].name) == 0) {
               labelLocation = i;
               labelFound = true;
               break;
            }
         }
         if (!labelFound) {
            SymbolTableEntry ste = SymbolTableEntry();
            ste.name = word;
            ste.addr = -1;
            ste.flags = "?";
            ste.numID = (int)SymbolTable::entries.size();
            ste.size = 0;
            ste.sectionID = -1;
            ste.type = "SYM";
            SymbolTable::pushBack(ste);
            labelLocation = ste.numID;
         }
         if (SymbolTable::entries[labelLocation].flags == "ABS") {
            return SymbolTable::entries[labelLocation].addr;
         }
         RelocationTableEntry rte = RelocationTableEntry();
         rte.address = locationCounter;
         rte.numID = labelLocation;
         rte.type = SymbolTable::entries[labelLocation].flags == "L" ? "R" : "A";
         Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
         return 0;
      }
      
      int firstPlusLocation = -1;
      firstPlusLocation = (int)word.find('+');
      int firstMinusLocation = -1;
      firstMinusLocation = (int)word.find('-');
      
      if (firstPlusLocation != -1 || firstMinusLocation != -1) {
         if (firstPlusLocation != -1 && firstMinusLocation != -1) {
            int signLocation = firstPlusLocation < firstMinusLocation ? firstPlusLocation : firstMinusLocation;
            if (isValidString(word.substr(0, signLocation)) && isConstantExpression(word.substr(signLocation, word.length() - signLocation))) {
               string label = word.substr(0, signLocation);
               bool labelFound = false;
               int labelLocation = -1;
               for (int i=0; i < SymbolTable::entries.size(); i++) {
                  if (label.compare(SymbolTable::entries[i].name)) {
                     labelLocation = i;
                     labelFound = true;
                     break;
                  }
               }
               if (!labelFound) {
                  SymbolTableEntry ste = SymbolTableEntry();
                  ste.name = label;
                  ste.addr = -1;
                  ste.flags = "?";
                  ste.numID = (int)SymbolTable::entries.size();
                  ste.size = 0;
                  ste.sectionID = -1;
                  ste.type = "SYM";
                  SymbolTable::pushBack(ste);
                  labelLocation = ste.numID;
               }
               
               RelocationTableEntry rte = RelocationTableEntry();
               rte.address = locationCounter;
               rte.numID = labelLocation;
               rte.type = SymbolTable::entries[labelLocation].flags == "L" ? "R" : "A";
               Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
               return getExpressionValue(word.substr(signLocation, word.length() - signLocation));
            }
         } else {
            int signLocation = firstPlusLocation != -1 ? firstPlusLocation : firstMinusLocation;
            if (isValidString(word.substr(0, signLocation)) && isConstantExpression(word.substr(signLocation, word.length() - signLocation))) {
               
               string label = word.substr(0, signLocation);
               bool labelFound = false;
               int labelLocation = -1;
               for (int i=0; i < SymbolTable::entries.size(); i++) {
                  if (label.compare(SymbolTable::entries[i].name)) {
                     labelLocation = i;
                     labelFound = true;
                     break;
                  }
               }
               if (!labelFound) {
                  SymbolTableEntry ste = SymbolTableEntry();
                  ste.name = label;
                  ste.addr = -1;
                  ste.flags = "?";
                  ste.numID = (int)SymbolTable::entries.size();
                  ste.size = 0;
                  ste.sectionID = -1;
                  ste.type = "SYM";
                  SymbolTable::pushBack(ste);
                  labelLocation = ste.numID;
               }
               
               RelocationTableEntry rte = RelocationTableEntry();
               rte.address = locationCounter;
               rte.numID = labelLocation;
               rte.type = SymbolTable::entries[labelLocation].flags == "L" ? "R" : "A";
               Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
               return getExpressionValue(word.substr(signLocation, word.length() - signLocation));
            }
         }
      }
      
      if (isConstantExpression(word)) {
         return getExpressionValue(word);
      }
      return 0;
   }
   
   int getLSTypeCode(int lsInsType) {
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
};

#endif /* instructions_hpp */
