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
         if (word.substr(4, word.length() - 5).length() > 0) {
            return true;
         }
      } else if (isRegister(word.substr(1, 2))) {
         if (word.substr(3, word.length() - 4).length() > 0) {
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
   if (word[0] == '$') {
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
         // in case of immed if label is found relocation must be absolute
         displacement = Addressing::getMemoryDirectAddress(op, instr->locationCounter, "A", false);
         instr->locationCounter +=4;
         break;
      }
      case 1: { // RegDir
         instr->size = 4;
         adrField = 0b000;
         // no relocation
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
         // in case of memdir if label is found relocation must be absolute
         displacement = Addressing::getMemoryDirectAddress(op, instr->locationCounter, "A", false);
         instr->locationCounter += 4;
         break;
      }
      case 3: {// RegInd
         instr->size = 4;
         adrField = 0b010;
         // no relocation
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
         // TODO: - in case of regindpom if label is found relocation is relative???
         // maybe should use getMemoryDirectAddress function with isPCRel set to true
         displacement = getExpressionValue(offsetString);
         instr->locationCounter += 4;
         break;
      }
      case 5: { // PCRELPOM $
         instr->size = 8;
         adrField = 0b111;
         reg0Field = 0b10001;
         op = op.substr(1);
         string label = "";
         
         // First operand must be a label
         for (int i=0; i < op.length(); i++) {
            if (op.substr(i,1) == " " && label == "") {
               continue;
            }
            
            if (op.substr(i,1) == " ") {
               break;
            }
            
            if (op.substr(i,1) == ";") {
               break;
            }
            
            if (op.substr(i,1) == "+" || op.substr(i,1) == "-") {
               break;
            }
            
            label+=op.substr(i,1);
         }
         
         
         if (!isValidString(label)) {
            cout << "Error: Addressing using $ must use a valid label!" << endl;
            throw exception();
         }
         
         instr->locationCounter += 4;
         displacement = Addressing::getMemoryDirectAddress(op, instr->locationCounter, "R", true);
         break;
      }
      default:
         cout << "Error: Unknown addressing type!" << endl;
         throw exception();
   }
   unsigned char adrAndReg0Fields = (adrField << 5) | reg0Field;
   instr->instruction.push_back(adrAndReg0Fields);
   return displacement;
}


int Addressing::getMemoryDirectAddress(string word, int locationCounter, string relocationType, bool isPCRel) {
   string label = InputLine::getFirstWord(word);
   
   if (label == "") {
      cout << "Error: Trying to address with empty value!" << endl;
      throw exception();
   }
   int value = 0;
   
   if (isValidString(label)) {
      bool labelFound = false;
      int labelLocation = -1;
      for (int i=0; i < SymbolTable::entries.size(); i++) {
         if (SymbolTable::entries[i].name.compare(label) == 0) {
            if (SymbolTable::entries[i].sectionID == -1) {
               // If first word in expression is abs symbol than it must be a constant expression
               value = getExpressionValue(word);
               labelLocation = 0;
               labelFound = true;
               break;
            }
            labelLocation = i;
            labelFound = true;
            break;
         }
      }
      
      if (!labelFound) {
         SymbolTableEntry ste = SymbolTableEntry();
         ste.addr = 0;
         ste.flags = "?";
         ste.name = label;
         ste.numID = (int)SymbolTable::entries.size();
         ste.sectionID = 0;
         ste.type = "SYM";
         SymbolTable::pushBack(ste);
         labelLocation = (int)SymbolTable::entries.size() - 1;
      }
      
      if (labelLocation != 0) {
         SymbolTableEntry ste = SymbolTable::entries[labelLocation];
         Section currentSection = Sections::entries[Sections::entries.size() - 1];
         
         if (ste.sectionID == currentSection.numID && relocationType == "R") {
            // If label is in the same section as instruction that is using the label and relative addressing is being used
            // then the value of displacement is difference between the offset of the label from start of the current section (ste.addr)
            // and current location in this section (actually current location + 4 is the starting address of next instruction and because of PC it must be
            // incremented by 4)
            value = ste.addr;
            
            if (isPCRel) {
               value -= locationCounter + 4;
            } else {
               value -= locationCounter;
            }
         } else {
            RelocationTableEntry rte = RelocationTableEntry();
            if (ste.flags == "L") {
               value = ste.addr;
               if (isPCRel) {
                  value -=  4;
               }
               rte.numID = ste.sectionID;
            } else {
               rte.numID = ste.numID;
            }
            rte.address = locationCounter;
            rte.type = relocationType;
            Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
         }
         word = word.substr(label.length());
      }
   }
   
   if (word != "") {
      value += getExpressionValue(word);
   }
   
   return value;
}

