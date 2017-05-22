//
//  keywordsutil.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "symboltable.h"

using namespace std;

string sections[] = {".text", ".data", ".rodata", ".bss"};
int sectionsLength = 4;

bool isInteger(const std::string & s) {
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
   
   char * p ;
   strtol(s.c_str(), &p, 10) ;
   
   return (*p == 0) ;
}

char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F'};
int hexDigitsLength = 21;

bool isHexDigit(char c) {
   for (int i = 0; i < hexDigitsLength; i++) {
      if (hexDigits[i] == c) {
         return true;
      }
   }
   
   return false;
}

bool isSection(string word) {
   for (int i = 0; i < sectionsLength; i++) {
      if (word.find(sections[i]) == 0) { // word starts with sections[i]
         string modifier = word.substr(sections[i].length(), word.length());
         if (modifier.length() > 0) {
            if (modifier[0] != '.') {
               cout << "Error: Section keyword invalid name " << word << endl;
               throw exception();
            }
            
            modifier = modifier.substr(1, modifier.length());
            if (!isInteger(modifier)) {
               cout << "Error: Section keyword modifier not a number " << word << endl;
               throw exception();
            }
         }
         return true;
      }
   }
   return false;
}

bool isLabel(string word) {
   if ((word[0] < 'a' || word[0] > 'z') && (word[0] < 'A' || word[0] > 'Z')){
      return false;
   }
   for (int i=1; i < word.length() - 1; i++) {
      if ((word[i] < 'a' || word[i] > 'z') && (word[i] < 'A' || word[i] > 'Z') && (word[i] < '0' || word[i] > '9')){
         return false;
      }
   }
   if (word[word.length() - 1] != ':'){
      return false;
   }
   return true;
}

bool isValidString(string word) {
   if ((word[0] < 'a' || word[0] > 'z') && (word[0] < 'A' || word[0] > 'Z')){
      return false;
   }
   for (int i=1; i < word.length() - 1; i++) {
      if ((word[i] < 'a' || word[i] > 'z') && (word[i] < 'A' || word[i] > 'Z') && (word[i] < '0' || word[i] > '9')){
         return false;
      }
   }
   return true;
}

void printHexaFromChar(char c) {
   
   unsigned int maskedVal = (c >> 4) & 0x0F;
   if (maskedVal <=9) {
      cout << maskedVal;
   } else {
      cout << char(maskedVal + 55);
   }
   
   maskedVal = 0x0F & c;
   if (maskedVal <=9) {
       cout << maskedVal;
   } else {
       cout << char(maskedVal + 55);
   }  
}

bool isDirective(string word){
   return false;
}

bool isRegister(string word){
   string registerNames[] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "PC", "SP"};
   int registerNamesLength = 18;
   string test = word;
   transform(test.begin(), test.end(), test.begin(), ::toupper);
   for (int i=0; i < registerNamesLength; i++) {
      if (test.compare(registerNames[i]) == 0) {
         return true;
      }
   }
   return false;
}

bool isConstantExpression(string expression) {
   // TODO: - FINISH CONSTANT EXPRESSION - BUG WITH [R1]
   vector<string> expressionParts = vector<string>();
   string currentPart = "";
   for (int i = 0; i < expression.length(); i++) {
      if (expression[i] == '(' || expression[i] == ')') {
         if (currentPart != "") {
            expressionParts.push_back(currentPart);
            currentPart = "";
         }
         char c = expression[i];
         expressionParts.push_back(string(1,c));
      } else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
         if (currentPart != "") {
            expressionParts.push_back(currentPart);
            currentPart = "";
         }
         char c = expression[i];
         expressionParts.push_back(string(1,c));
      } else if ((expression[i] >= '0' && expression[i] <= '9') || (expression[i] >= 'a' && expression[i] <= 'z') || (expression[i] >= 'A' && expression[i] <= 'Z')) {
         char c = expression[i];
         currentPart += string(1,c);
      } else if (expression[i] == '\'') {
         expressionParts.push_back(expression.substr(i, 3));
         i += 2;
      }
   }
   
   if (currentPart != "") {
      expressionParts.push_back(currentPart);
      currentPart = "";
   }
   
//   for (int i=0; i<expressionParts.size(); i++) {
//      cout << expressionParts[i] << endl;
//   }
   
   int openBracketsCounter = 0;
   for (int i=0; i<expressionParts.size(); i++) {
      if (expressionParts[i].substr(0,2) == "0x") {
         for (int j=2; j < expressionParts[i].length(); j++) {
            if (!isHexDigit(expressionParts[i][j])) {
               return false;
            }
         }
         
         
      } else if (expressionParts[i].substr(0,2) == "0b") {
         for (int j=2; j<expressionParts[i].length(); j++) {
            if (expressionParts[i][j] != '0' && expressionParts[i][j] != '1') {
               return false;
            }
         }
         
         
      } else if (isInteger(expressionParts[i])) {
         
      } else if (expressionParts[i] == "(") {
         openBracketsCounter++;
      } else if (expressionParts[i] == ")") {
         if (openBracketsCounter <= 0) {
            return false;
         }
         openBracketsCounter--;
      } else if (expressionParts[i][0] == '\'' && expressionParts[i][2] == '\'') {
         
      }
   }
   
   // TODO: Finish method
   return true;
}

bool isCalculatableExpression(string expr) {
   if (isInteger(expr)) {
      return true;
   }
   
   for(int i=0; i < SymbolTable::entries.size(); i++) {
      if (SymbolTable::entries[i].name == expr) {
         return true;
      }
   }
   return false;
}

int getExpressionValue(string expr) {
   // TODO: Finish method
   if (isInteger(expr)) {
      return stoi(expr);
   }
   
   for(int i=0; i < SymbolTable::entries.size(); i++) {
      if (SymbolTable::entries[i].name == expr) {
         return SymbolTable::entries[i].addr;
      }
   }
   
   return 0;
}

string getRemainderFromVectorPosition(vector<string> vec, int pos) {
   string rem = "";
   for (int i=pos; i<vec.size(); i++) {
      rem+=vec[i];
   }
   return rem;
}

int getRegNum(string word) {
   string registerNames[] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "SP", "PC"};
   int registerNamesLength = 18;
   for (int i = 0; i < registerNamesLength; i++) {
      if (word.compare(registerNames[i]) == 0) {
         return i;
      }
   }
   return -1;
}

string trimSpaces(string word) {
   string trimmedString = "";
   int spaces = 0;

   for (int i=0; i < (int)word.length(); i++) {
      if(word.substr(i,1) == " ") {
         spaces++;
         continue;
      }
      break;
   }
   trimmedString = word.substr(spaces, word.length() - spaces);
   spaces = 0;
   
   for (int i= (int)(trimmedString.length() - 1); i>=0; i--) {
      if (trimmedString.substr(i, 1) == " ") {
         spaces++;
         continue;
      }
      break;
   }
   trimmedString = trimmedString.substr(0, trimmedString.length() - spaces);
   
   return trimmedString;
}
