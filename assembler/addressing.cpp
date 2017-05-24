//
//  addressing.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 23/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iostream>
#include "addressing.hpp"
#include "keywordsutil.h"
#include "symboltable.h"
#include "relocationtable.hpp"
#include "sections.hpp"
#include "inputline.h"

bool Addressing::isRegisterDirect(string word) {
   return isRegister(word);
}

bool Addressing::isRegisterIndirect(string word) {
   return isRegister(word.substr(1, word.length() - 2)) && word[0] == '[' && word[word.length() - 1] == ']';
}

bool Addressing::isRegisterIndirectWithOffset(string word) {
   if (word[0] == '[' && word[word.length() - 1] == ']') {
      if (isRegister(word.substr(1, 3))) {
         if (isConstantExpression(word.substr(4, word.length() - 5))) {
            return true;
         }
      } else if (isRegister(word.substr(1, 2))) {
         if (isConstantExpression(word.substr(3, word.length() - 4))) {
            return true;
         }
      }
   }
   return false;
}

bool Addressing::isMemoryDirect(string word) {
   if (isValidString(word)){
      return true;
   }
   int firstPlusLocation = -1;
   firstPlusLocation = (int)word.find('+');
   int firstMinusLocation = -1;
   firstMinusLocation = (int)word.find('-');
   
   if (firstPlusLocation != -1 || firstMinusLocation != -1) {
      if (firstPlusLocation != -1 && firstMinusLocation != -1) {
         int signLocation = firstPlusLocation < firstMinusLocation ? firstPlusLocation : firstMinusLocation;
         if (isValidString(word.substr(0, signLocation)) && isConstantExpression(word.substr(signLocation, word.length() - signLocation))) {
            return true;
         }
      } else {
         int signLocation = firstPlusLocation != -1 ? firstPlusLocation : firstMinusLocation;
         if (isValidString(word.substr(0, signLocation)) && isConstantExpression(word.substr(signLocation, word.length() - signLocation))) {
            return true;
         }
      }
   }
   
   if (isConstantExpression(word)) {
      return true;
   }
   return false;
}

bool Addressing::isImmediate(string word) {
   if (word[0] == '#' && isConstantExpression(word.substr(1, word.length() - 1))) {
      return true;
   } else if (word[0] == '#' && isValidString(word.substr(1, word.length() - 1))) {
      return true;
   }
   
   return false;
}

bool Addressing::isPCRelPom(string word) {
   if (word[0] == '$' && isValidString(word.substr(1, word.length() - 1))) {
      return true;
   }
   return false;
}

int Addressing::getType(string word) {
   if (isImmediate(word)) {
      return 0;
   }
   if (isRegisterDirect(word)) {
      return 1;
   }
   if (isRegisterIndirect(word)) {
      return 3;
   }
   if (isRegisterIndirectWithOffset(word)) {
      return 4;
   }
   if (isPCRelPom(word)) {
      return 5;
   }
   if (isMemoryDirect(word)) {
      return 2;
   }
   return -1;
}

int Addressing::resolveAddressingAndReturnDisplacement(string op, Instruction *instr, vector<int> allowedAddrs) {
   unsigned char adrField;
   unsigned char reg0Field;
   int displacement = 0;
   int addrType = Addressing::getType(op);
   
   bool found = false;
   for (int i=0; i<allowedAddrs.size(); i++) {
      if (allowedAddrs[i] == addrType) {
         found = true;
         break;
      }
   }
   
   if (!found) {
      cout << "Error: Wrong type of addressing for instruction of type " << instr->type << endl;
      throw exception();
   }
   
   switch (addrType) {
      case 0: {// Immed
         instr->size = 8;
         adrField = 0b100;
         
         reg0Field = 0b00000;
         instr->locationCounter += 4;
         op = op.substr(1);
         // TODO: - move this func to addressing and refactor it
         displacement = Addressing::getMemoryDirectAddress(op, instr->locationCounter);
         instr->locationCounter +=4;
         break;
      }
      case 1: { // RegDir
         instr->size = 4;
         adrField = 0b000;
         
         reg0Field = getRegNumFromOp(op) & 0b00011111;
         displacement = 0;
         instr->locationCounter += 4;
         break;
      }
      case 2:{ // MemDir
         instr->size = 8;
         adrField = 0b110;
         reg0Field = 0b00000;
         
         instr->locationCounter += 4;
         //check if label than extract expression...
         displacement = Addressing::getMemoryDirectAddress(op, instr->locationCounter);
         instr->locationCounter += 4;
         break;
      }
      case 3: {// RegInd
         instr->size = 4;
         adrField = 0b010;
         reg0Field = getRegNumFromOp(op) & 0b00011111;
         displacement = 0;
         instr->locationCounter += 4;
         break;
      }
      case 4:{ // RegIndPom
         instr->size = 8;
         adrField = 0b111;
         string offsetString;
         reg0Field = getRegNumFromOp(op, &offsetString) & 0b00011111;
         instr->locationCounter += 4;
         displacement = getExpressionValue(offsetString);
         instr->locationCounter += 4;
         break;
      }
      case 5: { // PCRELPOM $
         instr->size = 8;
         adrField = 0b010;
         reg0Field = 0b10001;
         op = op.substr(1);
         if (!isValidString(op)) {
            cout << "Error: Addressing using $ must use a valid label!" << endl;
            throw exception();
         }
         
         bool labelFound = false;
         int labelLocation = -1;
         for (int i=0; i < SymbolTable::entries.size(); i++) {
            if (SymbolTable::entries[i].name.compare(op) == 0) {
               if (SymbolTable::entries[i].flags == "ABS") {
                  cout << "Error: Addressing using $ can't use absolute symbol!" << endl;
                  throw exception();
               }
               labelLocation = i;
               labelFound = true;
            }
         }
         
         if (!labelFound) {
            SymbolTableEntry ste = SymbolTableEntry();
            ste.addr = -1;
            ste.flags = "?";
            ste.name = op;
            ste.numID = (int)SymbolTable::entries.size();
            ste.sectionID = -1;
            ste.type = "SYM";
            SymbolTable::pushBack(ste);
            labelLocation = (int)SymbolTable::entries.size() - 1;
         }
         
         RelocationTableEntry rte = RelocationTableEntry();
         rte.numID = SymbolTable::entries[labelLocation].numID;
         rte.address = instr->locationCounter;
         rte.type = "R";
         Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
         
         displacement = 0;
         break;
      }
      default:
         cout << "Error: Unknown addressing type!" << endl;
         throw exception();
   }
   unsigned char adrAndReg0Fields = (adrField << 3) | reg0Field;
   instr->instruction.push_back(adrAndReg0Fields);
   return displacement;
}


int Addressing::getMemoryDirectAddress(string word, int locationCounter) {
   string label = InputLine::getFirstWord(word);
   
   if (label == "") {
      cout << "Error: Trying to address with empty value!" << endl;
      throw exception();
   }
   
   if (isValidString(label)) {
      bool labelFound = false;
      int labelLocation = -1;
      for (int i=0; i < SymbolTable::entries.size(); i++) {
         if (SymbolTable::entries[i].name.compare(label) == 0) {
            if (SymbolTable::entries[i].flags == "ABS") {
               // If first word in expression is abs symbol than it must be a constant expression
               return getExpressionValue(word);
            }
            labelLocation = i;
            labelFound = true;
            break;
         }
      }
      
      if (!labelFound) {
         SymbolTableEntry ste = SymbolTableEntry();
         ste.addr = -1;
         ste.flags = "?";
         ste.name = label;
         ste.numID = (int)SymbolTable::entries.size();
         ste.sectionID = -1;
         ste.type = "SYM";
         SymbolTable::pushBack(ste);
         labelLocation = (int)SymbolTable::entries.size() - 1;
      }
      
      RelocationTableEntry rte = RelocationTableEntry();
      rte.numID = SymbolTable::entries[labelLocation].numID;
      rte.address = locationCounter;
      rte.type = "A";
      Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
      word = word.substr(label.length());
   }
   
   if (word == "") {
      return 0;
   }
   
   return getExpressionValue(word);
}

