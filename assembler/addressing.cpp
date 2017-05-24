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
         displacement = instr->getMemoryDirectAddress(op);
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
         displacement = instr->getMemoryDirectAddress(op);
         instr->locationCounter += 4;
         break;
      }
      case 3: {// RegInd
         instr->size = 4;
         adrField = 0b010;
         reg0Field = getRegNumFromOp(op) & 0b00011111;
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
         //  TODO: - Finish this case
         displacement = instr->getMemoryDirectAddress(op);
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
