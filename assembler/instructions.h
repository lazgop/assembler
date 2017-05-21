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
   vector<char> instruction;
   int locationCounter;
public:
   Instruction(vector<string> text){
      switch(getType(text[0])){
         case 0: { // JMP
            type = "JMP";
            switch (getJumpType(text[0])) {
               case 0: // "INT"
                  size = 4;
                  break;
               case 1: case 2: // "JMP", "CALL"
                  if (Addressing::isRegisterIndirect(text[1])) {
                     size = 4;
                  } else {
                     size = 8;
                  }
                  break;
                case 3: // "RET"
                  size = 4;
                  break;
               case 4: case 5: case 6: case 7: case 8: case 9: // "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"
                  if (Addressing::isRegisterIndirect(text[2])) {
                     size = 4;
                  } else {
                     size = 8;
                  }
                  break;
               default:
                  break;
            }
            break;
         }
         case 1: { // LS
            type = "LS";
            if (Addressing::isRegisterDirect(text[2])) {
               size = 4;
            } else if (Addressing::isRegisterIndirect(text[2])) {
               size = 4;
            } else {
               size = 8;
            }
            break;
         }
         case 2: { // STACK
            type = "STACK";
            size = 4;
            break;
         }
         case 3: { // AL
            type = "AL";
            size = 4;
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
   
   // Constructor for second pass
   Instruction(vector<string> text, int lc) {
      instruction = vector<char>();
      this->locationCounter = lc;
      switch(getType(text[0])){
         case 0: { // JMP
            type = "JMP";
            int jmpType = getJumpType(text[0]);
            switch (jmpType) {
               case 0: {// "INT"
                  size = 4;
                  instruction.push_back(0);
                  string rem = getRemainderFromVectorPosition(text, 1);
                  int reg0 = getRegNum(rem);
                  instruction.push_back(reg0);
                  instruction.push_back(0);
                  instruction.push_back(0);
                  break;
               }
               case 1: case 2: {// "JMP", "CALL"
                  instruction.push_back(jmpType + 1);
                  string rem = getRemainderFromVectorPosition(text, 1);
                  
                  switch (Addressing::getType(rem)) {
                     case 2:{ // MemDir
                        instruction.push_back(0b11000000);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int addr = getMemoryDirectAddress(rem);
                        for (int i = 0; i < 4; i++) {
                           int c = addr >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 3: {// RegInd
                        int adrAndReg = 0b010;
                        int reg0 = getRegNum(rem);
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        break;
                     }
                     case 4:{ // RegIndPom
                        int adrAndReg = 0b111;
                        int reg0;
                        string displ;
                        if (isRegister(rem.substr(1, 2))) {
                           reg0 = getRegNum(rem.substr(1,2));
                           displ = rem.substr(3, rem.length() - 4);
                        } else {
                           reg0 = getRegNum(rem.substr(1,3));
                           displ = rem.substr(4, rem.length() - 5);
                        }
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(0);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int offset = getExpressionValue(displ);
                        for (int i = 0; i < 4; i++) {
                           int c = offset >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     default:
                        break;
                  }

                  if (Addressing::isRegisterIndirect(text[1])) {
                     size = 4;
                  } else {
                     size = 8;
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
                  string rem = getRemainderFromVectorPosition(text, 2);
                  int reg = -1;
                  if (isRegister(text[1])) {
                     reg = getRegNum(text[1]);
                  } else {
                     throw exception();
                  }
                  switch (Addressing::getType(rem)) {
                     case 2:{ // MemDir
                        instruction.push_back(0b11000000);
                        instruction.push_back(reg << 3);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int addr = getMemoryDirectAddress(rem);
                        for (int i = 0; i < 4; i++) {
                           int c = addr >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     case 3: {// RegInd
                        int adrAndReg = 0b010;
                        int reg0 = getRegNum(rem);
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(reg << 3);
                        instruction.push_back(0);
                        break;
                     }
                     case 4:{ // RegIndPom
                        int adrAndReg = 0b111;
                        int reg0;
                        string displ;
                        if (isRegister(rem.substr(1, 2))) {
                           reg0 = getRegNum(rem.substr(1,2));
                           displ = rem.substr(3, rem.length() - 4);
                        } else {
                           reg0 = getRegNum(rem.substr(1,3));
                           displ = rem.substr(4, rem.length() - 5);
                        }
                        adrAndReg = adrAndReg << 5;
                        adrAndReg += reg0;
                        instruction.push_back(adrAndReg);
                        instruction.push_back(reg << 3);
                        instruction.push_back(0);
                        locationCounter += 4;
                        int offset = getExpressionValue(displ);
                        for (int i = 0; i < 4; i++) {
                           int c = offset >> (8*(3-i));
                           instruction.push_back(char(c));
                        }
                        locationCounter += 4;
                        break;
                     }
                     default:
                        break;
                  }
                  if (Addressing::isRegisterIndirect(text[2])) {
                     size = 4;
                  } else {
                     size = 8;
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
            int lsInsType = getLSType(text[0]);
            int reg1 = -1;
            if (isRegister(text[1])) {
               reg1 = getRegNum(text[1]);
            } else {
               throw exception();
            }
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
                  cout << "LS INSTRUCTION" << endl;
                  throw exception();
                  break;
            }
            
            string rem = getRemainderFromVectorPosition(text, 2);
            
            switch (Addressing::getType(rem)) {
               case 0: {// Immed
                  if (lsInsType == 5 || lsInsType == 6 || lsInsType == 7) { // "STORE", "STOREB", "STOREW"
                     cout << "Error: Cannot use immediate addressing in STORE instructions!" << endl;
                     throw exception();
                  }
                  instruction.push_back(0b10000000);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int value = getMemoryDirectAddress(rem.substr(1, rem.length() - 1));
                  for (int i = 0; i < 4; i++) {
                     int c = value >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter +=4;
                  break;
               }
               case 1: { // RegDir
                  int adrAndReg = 0b000;
                  int reg0 = getRegNum(rem);
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  break;
               }
               case 2:{ // MemDir
                  instruction.push_back(0b11000000);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int addr = getMemoryDirectAddress(rem);
                  for (int i = 0; i < 4; i++) {
                     int c = addr >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter += 4;
                  break;
               }
               case 3: {// RegInd
                  int adrAndReg = 0b010;
                  int reg0 = getRegNum(rem);
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  break;
               }
               case 4:{ // RegIndPom
                  int adrAndReg = 0b111;
                  int reg0;
                  string displ;
                  if (isRegister(rem.substr(1, 2))) {
                     reg0 = getRegNum(rem.substr(1,2));
                     displ = rem.substr(3, rem.length() - 4);
                  } else {
                     reg0 = getRegNum(rem.substr(1,3));
                     displ = rem.substr(4, rem.length() - 5);
                  }
                  adrAndReg = adrAndReg << 5;
                  adrAndReg += reg0;
                  instruction.push_back(adrAndReg);
                  instruction.push_back(reg1 << 3);
                  instruction.push_back(getLSTypeCode(lsInsType));
                  locationCounter += 4;
                  int offset = getExpressionValue(displ);
                  for (int i = 0; i < 4; i++) {
                     int c = offset >> (8*(3-i));
                     instruction.push_back(char(c));
                  }
                  locationCounter += 4;
                  break;
               }
               default:
                  break;
            }

            
            if (Addressing::isRegisterDirect(text[2]) || Addressing::isRegisterIndirect(text[2])) {
               size = 4;
            } else {
               size = 8;
            }
            break;
         }
         case 2: { // STACK
            type = "STACK";
            size = 4;
            if (text[0] == "PUSH") {
               instruction.push_back(0x20);
            } else {
               instruction.push_back(0x21);
            }
            string rem = getRemainderFromVectorPosition(text, 1);
            if (!isRegister(rem)) {
               cout << "Error: Calling stack instructions with unresolved register!" << endl;
               throw exception();
            }
            instruction.push_back(getRegNum(rem));
            instruction.push_back(0);
            locationCounter += 4;
            break;
         }
         case 3: { // AL
            type = "AL";
            size = 4;
            int alInsType = getALType(text[0]);
            instruction.push_back(0x30 | alInsType);
            if (!isRegister(text[1]) || !isRegister(text[2]) || !isRegister(text[3])) {
               cout << "Error: Calling arithmetic instructions with unresolved register!" << endl;
               throw exception();
            }
            int reg0 = getRegNum(text[1]);
            int reg1 = getRegNum(text[2]);
            int reg2 = getRegNum(text[3]);
            
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
            if (word.compare(SymbolTable::entries[i].name)) {
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
