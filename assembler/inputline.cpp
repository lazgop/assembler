//
//  inputline.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 22/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include "inputline.h"

string InputLine::getFirstWord(string line) {
   string word = "";
   for (int i=0; i < line.length(); i++) {
      string c = line.substr(i, 1);
      if (c == " " && word == "") {
         continue;
      }
      if (c == ";") {
         break;
      }
      if (c == " ") {
         break;
      }
      word += c;
   }
   return word;
}

string InputLine::getFirstOperand(string line) {
   string word = "";
   for (int i=0; i < line.length(); i++) {
      string c = line.substr(i, 1);
      if (c == " " && word == "") {
         continue;
      }
      if (c == ";") {
         break;
      }
      if (c == ",") {
         break;
      }
      word += c;
   }
   int spaces = 0;
   for (int i = (int)(word.length() - 1); i>=0; i--) {
      if (word.substr(i, 1) == " ") {
         spaces++;
         continue;
      }
      break;
   }
   word = word.substr(0, word.length() - spaces);
   return word;
}

string InputLine::getNextOperandAfter(string op, string line) {
   int opLastPosInLine = (int)(line.find(op) + op.length());
   string remline = line.substr(opLastPosInLine, line.length() - opLastPosInLine);
   if (remline[0] == ',') {
      remline = remline.substr(1, remline.length() - 1);
   }
   string word = "";
   for (int i=0; i < remline.length(); i++) {
      string c = remline.substr(i, 1);
      if (c == " " && word == "") {
         continue;
      }
      if (c == ";") {
         break;
      }
      if (c == ",") {
         break;
      }
      word += c;
   }
   int spaces = 0;
   for (int i = (int)(word.length() - 1); i>=0; i--) {
      if (word.substr(i, 1) == " ") {
         spaces++;
         continue;
      }
      break;
   }
   word = word.substr(0, word.length() - spaces);
   return word;
}

vector<string> InputLine::splitOperands(string line) {
   vector<string> operands = vector<string>();
   
   string curOperand = InputLine::getFirstOperand(line);
   
   while (curOperand != "") {
      operands.push_back(curOperand);
      curOperand = InputLine::getNextOperandAfter(curOperand, line);
   }
   
   return operands;
}

string InputLine::getNextWordAfter(string word, string line) {
   string nextWord = "";
   int wordEndPosInLine = (int)(line.find(word) + word.length());
   string lineSubstring = line.substr(wordEndPosInLine, line.length() - wordEndPosInLine);
   for (int i=0; i < lineSubstring.length(); i++) {
      string c = lineSubstring.substr(i, 1);
      if (c == " " && nextWord == "") {
         continue;
      }
      if (c == ";") {
         break;
      }
      if (c == " ") {
         break;
      }
      if (c == ",") {
         break;
      }
      nextWord += c;
   }
   return nextWord;
}

string InputLine::getRemainingStringAfter(string word, string line) {
   int wordEndPosInLine = (int)(line.find(word) + word.length());
   string remString = line.substr(wordEndPosInLine, line.length() - wordEndPosInLine);
   for (int i=0; i < remString.length(); i++) {
      if (remString.substr(i, 1) == " ") {
         continue;
      }
      remString = remString.substr(i, remString.length() - i);
      break;
   }
   
   int spaces = 0;
   for (int i = (int)(remString.length() - 1); i>=0; i--) {
      if (remString.substr(i, 1) == " ") {
         spaces++;
         continue;
      }
      break;
   }
   word = word.substr(0, word.length() - spaces);
   return remString;
}

bool InputLine::checkIfSomethingBehindWord(string word, string line) {
   int wordEndPosInLine = (int)(line.find(word) + word.length());
   string substring = line.substr(wordEndPosInLine, line.length() - wordEndPosInLine);
   for (int i=0; i < substring.length(); i++) {
      string c = substring.substr(i, 1);
      if (c == " ") {
         continue;
      }
      if (c == ";") {
         break;
      }
      return true;
   }
   return false;
}
