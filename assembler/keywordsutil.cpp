//
//  keywordsutil.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <string>
#include <stdlib.h>

using namespace std;

string sections[] = {".text", ".data", ".rodata", ".bss"};
int sectionsLength = 4;

bool isInteger(const std::string & s) {
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
   
   char * p ;
   strtol(s.c_str(), &p, 10) ;
   
   return (*p == 0) ;
}

bool isSection(string word) {
   for (int i = 0; i < sectionsLength; i++) {
      if (word.find(sections[i]) == 0) { // word starts with sections[i]
         string modifier = word.substr(sections[i].length(), word.length());
         if (modifier.length() > 0) {
            if (modifier[0] != '.') {
               return false;
            }
            
            modifier = modifier.substr(1, modifier.length());
            if (!isInteger(modifier)) {
               return false;
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
