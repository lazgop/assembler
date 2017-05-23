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
#include "section.hpp"
#include "sections.hpp"

using namespace std;

void assemblerFistPass(list<InputLine> *inputFile) {
   
   int locationCounter = 0;
   int addressCounter = 0;
   int lastSectionIndex = -1;
   bool lastDirectiveORG = false;
   int lastDirectiveORGAdress = -1;
   
   for (list<InputLine>::iterator iterator = inputFile->begin(); iterator != inputFile->end(); iterator++) {
      //find symbol or section and increment locationCounter
      
      string firstWord = iterator->getFirstWord();
      if (firstWord == ".end") {
         break;
      }
      if (firstWord == "") {
         continue;
      }
      if (lastDirectiveORG) {
         if (isSection(firstWord) && (lastDirectiveORGAdress >= addressCounter)) {
            addressCounter = lastDirectiveORGAdress;
            lastDirectiveORG = false;
         } else {
            cout << "Error: No section after ORG directive or ORG address less than current address!" << endl;
            throw exception();
         }
      }
      
      if (isSection(firstWord)) {
         if (iterator->checkIfSomethingBehindWord(firstWord)) {
            cout << "Error: Multiple commands in one line!" << endl;
            throw exception();
         }
         
         SymbolTableEntry entry = SymbolTableEntry();
         entry.type = "SEG";
         entry.numID = (int)SymbolTable::entries.size();
         entry.name = firstWord;
         entry.sectionID = entry.numID;
         entry.addr = addressCounter;
         
         if (lastSectionIndex != -1) {
            SymbolTable::entries[lastSectionIndex].size = locationCounter;
         }
         
         locationCounter = 0;
         
         SymbolTable::pushBack(entry);
         lastSectionIndex = (int)SymbolTable::entries.size() - 1;
         continue;
      }
      
      if (isLabel(firstWord)) {
         string labelName = firstWord.substr(0, firstWord.length() - 1);
         SymbolTableEntry entry = SymbolTableEntry();
         entry.type = "SYM";
         entry.numID = (int)SymbolTable::entries.size();
         entry.name = labelName;
         entry.flags = "L";
         entry.sectionID = lastSectionIndex;
         entry.addr = locationCounter;
         
         SymbolTable::pushBack(entry);
         
         if (!(iterator->checkIfSomethingBehindWord(firstWord))) {
            continue;
         }
         firstWord = iterator->getNextWordAfter(firstWord);
      }
      
      string afterFirstWord = iterator->getRemainingStringAfter(firstWord);
      
      Instruction ins = Instruction(firstWord, afterFirstWord);
      if (!(ins.getType().compare("NONE") == 0)) {
         locationCounter+=ins.getSize();
         addressCounter += ins.getSize();
         continue;
      }
      
      // TODO: Directive
      Directive dir = Directive(firstWord, afterFirstWord);
      if (!(dir.getType().compare("NONE") == 0)) {
         if (dir.getType().compare("ORG") == 0) {
            lastDirectiveORG = true;
            lastDirectiveORGAdress = dir.getSize();
         } else {
            locationCounter += dir.getSize();
            addressCounter += dir.getSize();
         }
         continue;
      }
      
      cout << "Error: Unknown line of code: " << endl;
      throw exception();
   }
   
   if (USymbolTable::entries.size() > 0) {
      cout << "Error: Uncalculatable symbols in USTable found!" << endl;
      throw exception();
   }
   
   if (lastSectionIndex != -1) {
      SymbolTable::entries[lastSectionIndex].size = locationCounter;
   }
   locationCounter = 0;
}

void assemblerSecondPass(list<InputLine> *inputFile) {
   int locationCounter = 0;
   int addressCounter = 0;
   int lastSectionIndex = -1;
   bool lastDirectiveORG = false;
   int lastDirectiveORGAdress = -1;
   
   for (list<InputLine>::iterator iterator = inputFile->begin(); iterator != inputFile->end(); iterator++) {
      
      //find symbol or section and increment locationCounter
      string firstWord = iterator->getFirstWord();
      
      if (isSection(firstWord)) {
         Section section = Section(firstWord);
         Sections::entries.push_back(section);
         locationCounter = 0;
         continue;
      }
      
      vector<string> curWords = iterator->words;
      if (isLabel(firstWord)) {
         
         if (!(iterator->checkIfSomethingBehindWord(firstWord))) {
            continue;
         }
         firstWord = iterator->getNextWordAfter(firstWord);
      }
      
      string afterFirstWord = iterator->getRemainingStringAfter(firstWord);
      
      Instruction ins = Instruction(firstWord, afterFirstWord, locationCounter);
      if (!(ins.getType().compare("NONE") == 0)) {
         locationCounter+=ins.getSize();
         addressCounter += ins.getSize();
         for (int i = 0; i < ins.instruction.size(); i++) {
            Sections::entries[Sections::entries.size() - 1].content.push_back(ins.instruction[i]);
         }
         continue;
      }
      
      Directive dir = Directive(firstWord, afterFirstWord, locationCounter);
      if (!(dir.getType().compare("NONE") == 0)) {
         if (dir.getType().compare("ORG") == 0) {
            lastDirectiveORG = true;
            lastDirectiveORGAdress = dir.getSize();
         } else {
            locationCounter += dir.getSize();
            addressCounter += dir.getSize();
         }
         continue;
      }
      
   }
   
   if (USymbolTable::entries.size() > 0) {
      cout << "Error: Uncalculatable symbols in USTable found!" << endl;
      throw exception();
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
   
   list<InputLine> *inputFile = FileManager::readInputFile(argv[1]);
   
   try {
      assemblerFistPass(inputFile);
   } catch (exception e) {
      return 1;
   }
   try {
      assemblerSecondPass(inputFile);
   }catch(exception e) {
      return 2;
   }
   
   for (int i=0; i < SymbolTable::entries.size(); i++) {
      if (SymbolTable::entries[i].flags == "?") {
         cout << "Error: Undefined symbol " << SymbolTable::entries[i].name << endl;
         return 2;
      }
   }
   
   SymbolTable::outputSymbolTable();
   Sections::outputSections();
   
   return 0;
}
