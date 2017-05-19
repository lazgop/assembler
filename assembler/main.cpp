//
//  main.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "error.hpp"
#include "filemanager.hpp"
#include "inputline.h"
#include "keywordsutil.h"
#include "symboltable.h"
#include "symboltableentry.h"
#include "instructions.h"
#include "directives.h"

using namespace std;

void assemblerFistPass(list<InputLine> *inputFile) {
   
   int locationCounter = 0;
   int addressCounter = 0;
   int lastSectionIndex = -1;
   
   for (list<InputLine>::iterator iterator = inputFile->begin(); iterator != inputFile->end(); iterator++) {
      //find symbol or section and increment locationCounter
      string word = iterator->words[0];
      if (isSection(word)) {
         if (SymbolTable::contains(word)){
            cout << "Section redeclaration error for Section: " << word << endl;
            throw exception();
         }
         SymbolTableEntry entry = SymbolTableEntry();
         entry.type = "SEG";
         entry.numID = (int)SymbolTable::entries.size();
         entry.name = word;
         entry.sectionID = entry.numID;
         addressCounter += locationCounter;
         entry.addr = addressCounter;
         // TODO: Other fields
         
         if (lastSectionIndex != -1) {
            SymbolTable::entries[lastSectionIndex].size = locationCounter;
         }
         
         locationCounter = 0;
         
         SymbolTable::entries.push_back(entry);
         lastSectionIndex = (int)SymbolTable::entries.size() - 1;
      } else {
         vector<string> curWords = iterator->words;
         if (isLabel(word)) {
            word = word.substr(0, word.length() - 1);
            if (SymbolTable::contains(word)){
               cout << "Label redeclaration error for label: " << word << endl;
               throw exception();
            }
            SymbolTableEntry entry = SymbolTableEntry();
            entry.type = "SYM";
            entry.numID = (int)SymbolTable::entries.size();
            entry.name = word;
            entry.flags = "L";
            entry.sectionID = lastSectionIndex;
            entry.addr = locationCounter;
            
            // TODO: Other fields
            SymbolTable::entries.push_back(entry);
            if (!(iterator->words.size() > 1)) {
               continue;
            }
            
            curWords = vector<string>();
            for (int i = 1; i < iterator->words.size(); i++) {
               curWords.push_back(iterator->words[i]);
            }
         }
         
         Instruction ins = Instruction(curWords);
         if (!(ins.getType().compare("NONE") == 0)) {
            locationCounter+=ins.getSize();
            continue;
         }
         
         // TODO: Directive
         Directive dir = Directive(curWords);
         if (!(dir.getType().compare("NONE") == 0)) {
            locationCounter += dir.getSize();
            continue;
         }
      }
      //inc lc
   }
   
   if (lastSectionIndex != -1) {
      SymbolTable::entries[lastSectionIndex].size = locationCounter;
   }
   locationCounter = 0;
}

int main(int argc, const char * argv[]) {
   if (argc < 3) {
      Error::handleArgvError();
      return 1;
   }
   
//   isConstantExpression("25+( smilja*11 - (24)) / gs4 + 'c' + 0x123");
//   cout << isInteger("FF2");
   
   list<InputLine> *inputFile = FileManager::readInputFile(argv[1]);
   
   try {
      assemblerFistPass(inputFile);
      SymbolTable::outputSymbolTable();
   }catch(exception e) {
      // TODO: Handle
      return 1;
   }

   
   
   return 0;
}
