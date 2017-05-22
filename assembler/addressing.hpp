//
//  addressing.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 19/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef addressing_hpp
#define addressing_hpp

#include <string>
#include "keywordsutil.h"

using namespace std;

class Addressing {
public:
   
   static bool isRegisterDirect(string word) {
      return isRegister(word);
   }
   
   static bool isRegisterIndirect(string word) {
      return isRegister(word.substr(1, word.length() - 2)) && word[0] == '[' && word[word.length() - 1] == ']';
   }
   
   static bool isRegisterIndirectWithOffset(string word) {
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

   static bool isMemoryDirect(string word) {
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
   
   static bool isImmediate(string word) {
      if (word[0] == '#' && isConstantExpression(word.substr(1, word.length() - 1))) {
         return true;
      } else if (word[0] == '#' && isValidString(word.substr(1, word.length() - 1))) {
         return true;
      }
      
      return false;
   }
   
   static bool isPCRelPom(string word) {
      if (word[0] == '$' && isValidString(word.substr(1, word.length() - 1))) {
         return true;
      }
      return false;
   }
   
   static int getType(string word) {
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
};

#endif /* addressing_hpp */
