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
#include "usymboltable.h"
#include "instructions.h"
#include "directives.h"
#include "section.hpp"
#include "sections.hpp"

using namespace std;

void assemblerFistPass(list<InputLine> *inputFile) {
   
   // UNDEF section
   SymbolTableEntry entry = SymbolTableEntry();
   entry.type = "SEG";
   entry.numID = 0;
   entry.name = "UNDEF";
   entry.sectionID = 0;
   entry.addr = 0;
   SymbolTable::pushBack(entry);
   // end UNDEF section
   
   int locationCounter = 0;
   int addressCounter = 0;
   int lastSectionIndex = 0;
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
         if (isSection(firstWord)) {
            addressCounter = lastDirectiveORGAdress;
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
         if (lastDirectiveORG) {
            entry.addr = addressCounter;
            lastDirectiveORG = false;
         } else {
            entry.addr = 0;
         }
         
         if (firstWord.find(".bss") != string::npos) {
            entry.flags = "ARW";
         } else if (firstWord.find(".text") != string::npos) {
            entry.flags = "ARX";
         } else if (firstWord.find(".data") != string::npos) {
            entry.flags = "ARW";
         } else if (firstWord.find(".rodata") != string::npos) {
            entry.flags = "AR";
         }
         
         SymbolTable::entries[lastSectionIndex].size = locationCounter;
         
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
   
   
   bool calculatedSymbolInThisPass = false;
   do {
      calculatedSymbolInThisPass = false;
      for (int i=0; i < USymbolTable::entries.size(); i++) {
         if(USymbolTable::entries[i].name != "-1") {
            try {
               int val = getExpressionValue(USymbolTable::entries[i].expression);
               SymbolTableEntry entry = SymbolTableEntry();
               entry.type = "SYM";
               entry.numID = (int)SymbolTable::entries.size();
               entry.name = USymbolTable::entries[i].name;
               entry.flags = "L";
               entry.sectionID = -1;
               entry.addr = val;
               SymbolTable::pushBack(entry);
               calculatedSymbolInThisPass = true;
               USymbolTable::entries[i].name = "-1";
            } catch (...) {
               
            }
         }
      }
   }while(calculatedSymbolInThisPass);
   
   vector<USymbolTableEntry> tmpUSymbTbl = vector<USymbolTableEntry>();
   for (int i=0; i < USymbolTable::entries.size(); i++) {
      if (USymbolTable::entries[i].name != "-1") {
         tmpUSymbTbl.push_back(USymbolTable::entries[i]);
      }
   }
   USymbolTable::entries = tmpUSymbTbl;
   
   
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
   
   for (int i=0; i < USymbolTable::entries.size(); i++) {
      // calculate expression value
      // TODO: - maybe check if label is first operand
   }
   
   if (USymbolTable::entries.size() > 0) {
      cout << "Error: Uncalculatable symbol/s found in Uncalculatable Symbols Table!" << endl;
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
   
   
//   SymbolTable::outputSymbolTable();
//   Sections::outputSections();
   
   FileManager::outputToFile(argv[2]);
   
   return 0;
}
