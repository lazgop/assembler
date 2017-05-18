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
#include "keywordsutil.h"

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
                  if (isRegister(text[1].substr(1, text[1].length() - 1)) && text[1][0] == '[' && text[1][text[1].length()] == ']') {
                     size = 4;
                  } else {
                     size = 8;
                  }
                  break;
                case 3: // "RET"
                  size = 4;
                  break;
               case 4: case 5: case 6: case 7: case 8: case 9: // "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"
                  if (isRegister(text[2].substr(1, text[2].length() - 1)) && text[2][0] == '[' && text[2][text[2].length()] == ']') {
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
            if (isRegister(text[2])) {
               size = 4;
            } else if (isRegister(text[2].substr(1, text[2].length() - 1)) && text[2][0] == '[' && text[2][text[2].length()] == ']') {
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
};

#endif /* instructions_hpp */
