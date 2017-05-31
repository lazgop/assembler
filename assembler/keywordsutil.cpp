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
#include <cmath>
#include "symboltable.h"
#include "keywordsutil.h"
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


int getIntFromHex(string hex) {
   int value = 0;
   for (int i=0; i < hex.length(); i++) {
      switch (hex[hex.length() - i - 1]) {
         case '0': value += pow(16,i) * 0; break;
         case '1': value += pow(16,i) * 1; break;
         case '2': value += pow(16,i) * 2; break;
         case '3': value += pow(16,i) * 3; break;
         case '4': value += pow(16,i) * 4; break;
         case '5': value += pow(16,i) * 5; break;
         case '6': value += pow(16,i) * 6; break;
         case '7': value += pow(16,i) * 7; break;
         case '8': value += pow(16,i) * 8; break;
         case '9': value += pow(16,i) * 9; break;
         case 'a':
         case 'A': value += pow(16,i) * 10; break;
         case 'b':
         case 'B': value += pow(16,i) * 11; break;
         case 'c':
         case 'C': value += pow(16,i) * 12; break;
         case 'd':
         case 'D': value += pow(16,i) * 13; break;
         case 'e':
         case 'E': value += pow(16,i) * 14; break;
         case 'f':
         case 'F': value += pow(16,i) * 15; break;
         default:
            cout << "Error: Unknown hex digit " << hex[hex.length() - i - 1] << endl;
            throw exception();
      }
   }
   return value;
}

int getIntFromBin(string bin) {
   int value = 0;
   for (int i=0; i < bin.length(); i++) {
      switch (bin[bin.length() - i - 1]) {
         case '0': value += pow(2,i) * 0; break;
         case '1': value += pow(2,i) * 1; break;
         default:
            cout << "Error: Unknown bin digit " << bin[bin.length() - i - 1] << endl;
            throw exception();
      }
   }
   return value;
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

string getHexFromChar(char c) {
   
   string hex = "";
   
   unsigned int maskedVal = (c >> 4) & 0x0F;
   if (maskedVal <=9) {
      hex += char(maskedVal + 48);
   } else {
      hex += char(maskedVal + 55);
   }
   
   maskedVal = 0x0F & c;
   if (maskedVal <=9) {
      hex += char(maskedVal + 48);
   } else {
      hex += char(maskedVal + 55);
   }
   
   return hex;
}


bool isDirective(string word){
   return false;
}

bool isRegister(string word){
   string registerNames[] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "PC", "SP"};
   int registerNamesLength = 18;
   string test = word;
//   transform(test.begin(), test.end(), test.begin(), ::toupper);
   for (int i=0; i < registerNamesLength; i++) {
      if (test.compare(registerNames[i]) == 0) {
         return true;
      }
   }
   return false;
}

vector<string> splitInfixExpression(string infix) {
   vector<string> splitExpression = vector<string>();
   
   string operand = "";
   for (int i=0; i<infix.length(); i++) {
      string c = infix.substr(i,1);
      
      if (c == " " && operand == "") {
         continue;
      }
      
      if (c == " " && operand != "") {
         splitExpression.push_back(operand);
         operand = "";
         continue;
      }
      
      if (c == "(" || c == ")" || c == "+" || c == "-" || c == "/" || c == "*") {
         if (operand != "") {
            splitExpression.push_back(operand);
            operand = "";
         }
         splitExpression.push_back(c);
         continue;
      }
   
      operand += c;
   }
   
   if (operand != "") {
      splitExpression.push_back(operand);
      operand = "";
   }
   
   return splitExpression;
}

bool isConstantExpression(string expression) {
   // TODO: - FINISH CONSTANT EXPRESSION - BUG WITH [R1]
//   vector<string> expressionParts = vector<string>();
//   string currentPart = "";
//   for (int i = 0; i < expression.length(); i++) {
//      if (expression[i] == '(' || expression[i] == ')') {
//         if (currentPart != "") {
//            expressionParts.push_back(currentPart);
//            currentPart = "";
//         }
//         char c = expression[i];
//         expressionParts.push_back(string(1,c));
//      } else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
//         if (currentPart != "") {
//            expressionParts.push_back(currentPart);
//            currentPart = "";
//         }
//         char c = expression[i];
//         expressionParts.push_back(string(1,c));
//      } else if ((expression[i] >= '0' && expression[i] <= '9') || (expression[i] >= 'a' && expression[i] <= 'z') || (expression[i] >= 'A' && expression[i] <= 'Z')) {
//         char c = expression[i];
//         currentPart += string(1,c);
//      } else if (expression[i] == '\'') {
//         expressionParts.push_back(expression.substr(i, 3));
//         i += 2;
//      }
//   }
//   
//   if (currentPart != "") {
//      expressionParts.push_back(currentPart);
//      currentPart = "";
//   }
//   
//   int openBracketsCounter = 0;
//   for (int i=0; i<expressionParts.size(); i++) {
//      if (expressionParts[i].substr(0,2) == "0x") {
//         for (int j=2; j < expressionParts[i].length(); j++) {
//            if (!isHexDigit(expressionParts[i][j])) {
//               return false;
//            }
//         }
//         
//         
//      } else if (expressionParts[i].substr(0,2) == "0b") {
//         for (int j=2; j<expressionParts[i].length(); j++) {
//            if (expressionParts[i][j] != '0' && expressionParts[i][j] != '1') {
//               return false;
//            }
//         }
//         
//      } else if (isInteger(expressionParts[i])) {
//         
//      } else if (expressionParts[i] == "(") {
//         openBracketsCounter++;
//      } else if (expressionParts[i] == ")") {
//         if (openBracketsCounter <= 0) {
//            return false;
//         }
//         openBracketsCounter--;
//      } else if (expressionParts[i][0] == '\'' && expressionParts[i][2] == '\'') {
//         
//      }
//   }
   
   if (expression.length() > 0) {
      return true;
   }
   // TODO: Finish method
   return false;
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

int priority(string operat) {
   if (operat == "*" || operat == "/")
      return 1;
   
   if (operat == "+" || operat == "-")
      return 2;

   if (operat == "(")
      return 3;

   cout << "Error: Unknown operator in expression!" << " " << operat << endl;
   throw exception();
}


vector<string> infixToPostfix (vector<string> expression) {
   vector<string> operator_stack;
   
   vector<string> output;
   
   for (unsigned i = 0; i < expression.size(); i++) {
      string curPart = expression[i];
      if (curPart == "+" || curPart == "-" || curPart == "*" || curPart == "/") {
         while (!operator_stack.empty()) {
            if (!(priority(operator_stack[operator_stack.size() - 1]) <= priority(curPart))) {
               break;
            }
            output.push_back(operator_stack[operator_stack.size() - 1]);
            operator_stack.pop_back();
         }
         operator_stack.push_back(curPart);
      } else if (curPart == "(") {
         operator_stack.push_back(curPart);
      } else if (curPart == ")") {
         while (operator_stack[operator_stack.size() - 1] != "(") {
            output.push_back(operator_stack[operator_stack.size() - 1]);
            operator_stack.pop_back();
         }
         operator_stack.pop_back();
      } else {
         output.push_back(curPart);
      }
   }
   
   while (!operator_stack.empty()) {
      output.push_back(operator_stack[operator_stack.size() - 1]);
      operator_stack.pop_back();
   }
   
   return output;
}

struct Value {
   int value;
   bool calculated;
   string label;
};

int evalPostfix(vector<string> postfix) {
   vector<Value> s = vector<Value>();
   int i = 0;
   string ch;
   Value val = Value();
   val.value = 0;
   while (i < postfix.size()) {
      ch = postfix[i];
      if (ch != "+" && ch != "-" && ch != "*" && ch != "/") {
         // we saw an operand
         // push the digit onto stack
         Value operand = Value();
         if (ch.length() > 2 && ch.substr(0,2) == "0x") {
            operand.calculated = true;
            operand.value = getIntFromHex(ch.substr(2, ch.size() - 2));
         } else if (ch.length() > 2 && ch.substr(0,2) == "0b") {
            operand.calculated = true;
            operand.value = getIntFromBin(ch.substr(2, ch.size() - 2));
         } else if (isInteger(ch)){
            operand.calculated = true;
            operand.value = stoi(ch);
         } else if (ch[0] == '\'' && ch[2] == '\'') {
            char c = ch[1];
            operand.calculated = true;
            operand.value = (int)c;
         } else {
            operand.calculated = false;
            operand.label = ch;
         }
         s.push_back(operand);
         
      } else {
         // we saw an operator
         // pop off the top two operands from the
         // stack and evalute them using the current
         // operator
         Value op1 = s[s.size() - 1];
         s.pop_back();
         
         Value op2;
         if (s.size() != 0) {
            op2 = s[s.size() - 1];
            s.pop_back();
         } else {
            op2 = Value();
            op2.calculated = true;
            op2.value = 0;
         }
         
         int op1Section = -2;
         // TODO: - check if both values are labels and if they are from same section
         if (op1.calculated == false) {
            for (int j=0; j < SymbolTable::entries.size(); j++){
               if (SymbolTable::entries[j].name.compare(op1.label) == 0) {
                  if (SymbolTable::entries[j].sectionID == -1) {
                     op1Section = -1;
                     op1.calculated = true;
                     op1.value = SymbolTable::entries[j].addr;
                     break;
                  }
                  op1Section = SymbolTable::entries[j].sectionID;
                  op1.value = SymbolTable::entries[j].addr;
                  break;
               }
            }
            if (op1Section == -2) {
               cout << "Error: Label " << op1.label << " not found in symbol table in expression" << endl;
               throw exception();
            }
         }
         
         int op2Section = -2;
         if (op2.calculated == false) {
            for (int j=0; j < SymbolTable::entries.size(); j++){
               if (SymbolTable::entries[j].name.compare(op2.label) == 0) {
                  if (SymbolTable::entries[j].sectionID == -1) {
                     op2Section = -1;
                     op2.calculated = true;
                     op2.value = SymbolTable::entries[j].addr;
                     break;
                  }
                  op2Section = SymbolTable::entries[j].sectionID;
                  op2.value = SymbolTable::entries[j].addr;
                  break;
               }
            }
            if (op2Section == -2) {
               cout << "Error: Label " << op2.label << " not found in symbol table in expression" << endl;
               throw exception();
            }
         }
         
         if (op1Section == op2Section && !op1.calculated && !op2.calculated) {
            op1.calculated = true;
            op2.calculated = true;
         }
         
         if (!op1.calculated || !op2.calculated) {
            cout << "Error: Cannot calculate expression : " << (op2.calculated ? to_string(op2.value) : op2.label) << " " << ch << " " << (op1.calculated ? to_string(op1.value) : op1.label) << endl;
            throw exception();
         }
         
         val = Value();
         val.calculated = true;
         
         if (ch == "+") {
            val.value = op2.value + op1.value;
         } else if (ch == "-"){
            val.value = op2.value - op1.value;
         } else if (ch == "*") {
            val.value = op2.value * op1.value;
         } else if (ch == "/") {
            val.value = op2.value / op1.value;
         }
         // push the value obtained after evaluating
         // onto the stack
         s.push_back(val);
      }
      i++;
   }
   
   if (s.size() != 1) {
      cout << "Error: Invalid Postfix expression!" << endl;
      throw exception();
   }
   
   Value op1 = s[s.size() - 1];
   s.pop_back();

   int op1Section = -2;
   if (!op1.calculated) {
      for (i=0; i < SymbolTable::entries.size(); i++){
         if (SymbolTable::entries[i].name.compare(op1.label) == 0) {
            if (SymbolTable::entries[i].sectionID == -1) {
               op1Section = -1;
               op1.calculated = true;
               op1.value = SymbolTable::entries[i].addr;
               break;
            }
            op1Section = SymbolTable::entries[i].sectionID;
            op1.value = SymbolTable::entries[i].addr;
            break;
         }
      }
      if (op1Section == -2) {
         cout << "Error: Label " << op1.label << " not found in symbol table in expression" << endl;
         throw exception();
      }
   }
   
   return op1.value;
}


int getExpressionValue(string expr) {
   expr = trimSpacesFromStr(expr);
   vector<string> expression = splitInfixExpression(expr);
   vector<string> exprPostfix = infixToPostfix(expression);
   int value = evalPostfix(exprPostfix);
   return value;
}

string getRemainderFromVectorPosition(vector<string> vec, int pos) {
   string rem = "";
   for (int i=pos; i<vec.size(); i++) {
      rem+=vec[i];
   }
   return rem;
}

int getRegNumFromOp(string word) {
   string regname = "";
   if (isRegister(word)) {
      regname = word;
   } else if (word[0] == '[' && word[word.length() - 1] == ']') {
      word = word.substr(1, word.length() - 2);
      if (isRegister(word)) {
         regname = word;
      } else {
         if (isRegister(word.substr(1, 3))) {
            regname = word.substr(1,3);
         } else {
            regname = word.substr(1,2);
         }
      }
   }
   int regnum = getRegNum(regname);
   if (regnum == -1) {
      cout << "Error: " << regname << " is not a register" << endl;
      throw exception();
   }
   return regnum;
}


int getRegNumFromOp(string word, string *offset) {
   string regname = "";
   if (isRegister(word)) {
      regname = word;
   } else if (word[0] == '[' && word[word.length() - 1] == ']') {
      word = word.substr(1, word.length() - 2);
      if (isRegister(word)) {
         regname = word;
      } else {
         if (isRegister(word.substr(0, 3))) {
            regname = word.substr(0,3);
            *offset = word.substr(3, word.length() - 3);
            
         } else {
            regname = word.substr(0,2);
            *offset = word.substr(2, word.length() - 2);
         }
      }
   }
   int regnum = getRegNum(regname);
   if (regnum == -1) {
      cout << "Error: " << regname << " is not a register" << endl;
      throw exception();
   }
   return regnum;
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


string trimSpacesFromStr(string word) {
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

string getHexStringFromInt(int num) {
   string hex = "";
   for (int i=0; i < 8; i++) {
      unsigned int c = (num >> (4*(7-i))) & (0x0F);
      if (c <= 9) {
         hex += char(c + 48);
      } else {
         hex += char(c + 55);
      }
   }
   return hex;
}
