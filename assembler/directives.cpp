//
//  directives.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//


#include "directives.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "keywordsutil.h"
#include "symboltable.h"
#include "symboltableentry.h"
#include "usymboltable.h"
#include "usymboltableentry.h"
#include "sections.hpp"
#include "inputline.h"

string Directive::dataDefiningDirs[] = {"DB", "DW", "DD"};
int Directive::dataDefiningDirsLength = 3;
string Directive::regularDirs[] = {".global", "ORG"};
int Directive::regularDirsLength = 2;
string Directive::otherDirs[] = {"DEF"};
int Directive::otherDirsLength = 1;

Directive::Directive(string keyWord, string afterKeyword){
   
   // Needed for DUP command
   string secondWord = InputLine::getFirstWord(afterKeyword);
   switch(getType(keyWord, secondWord)){
      case 0: {// data defining direcive
         switch (getDDDType(keyWord)) {
            case 0: // "DB"
               type = "DB";
               constSize = 1;
               break;
            case 1: // "DW"
               type = "DW";
               constSize = 2;
               break;
            case 2: // "DD"
               type = "DD";
               constSize = 4;
               break;
         }
         vector<string> rem = convertRemainderWithDUP(afterKeyword);
         size = constSize * (int)rem.size();
         break;
      }
      case 1: // regular Directive
         switch (getRDType(keyWord)) {
            case 0: // ".global"
               type = ".global";
               size = 0;
               break;
            case 1: {// "ORG"
               type = "ORG";
               try {
                  size = getExpressionValue(afterKeyword);
               }catch(...) {
                  cout << "Error: Could not calculate expression in ORG directive!" << endl;
                  throw exception();
               }
               break;
            }
         }
         break;
      case 2:{ // other Directives
         // "DEF"
         if (!isValidString(keyWord)) {
            cout << "Error: Trying to define non string value!" << endl;
            throw exception();
         }
         type = "DEF";
         string rem = InputLine::getRemainingStringAfter("DEF", afterKeyword);
         
         int exprVal = 0;
         try {
            exprVal = getExpressionValue(rem);
            SymbolTableEntry entry = SymbolTableEntry();
            entry.type = "SYM";
            entry.numID = (int)SymbolTable::entries.size();
            entry.name = keyWord;
            entry.flags = "ABS";
            entry.sectionID = 0;
            entry.addr = exprVal;
            SymbolTable::pushBack(entry);
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
                        entry.flags = "ABS";
                        entry.sectionID = 0;
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
            
         }catch(...) {
            USymbolTableEntry uste = USymbolTableEntry();
            uste.name = keyWord;
            uste.expression = rem;
            USymbolTable::pushBack(uste);
         }
         size = 0;
         break;
      }
      default:
         type = "NONE";
         break;
   }
}
// *******************************
// *******************************
// *******************************
// *******************************
// *******************************
// *******************************
// *******************************

// Second pass constructor
Directive::Directive(string keyWord, string afterKeyword, int locationCounter) {
   // Needed for DUP command
   string secondWord = InputLine::getFirstWord(afterKeyword);
   switch(getType(keyWord, secondWord)){
      case 0: {// data defining direcive
         switch (getDDDType(keyWord)) {
            case 0: // "DB"
               type = "DB";
               constSize = 1;
               break;
            case 1: // "DW"
               type = "DW";
               constSize = 2;
               break;
            case 2: // "DD"
               type = "DD";
               constSize = 4;
               break;
         }
         vector<string> rem = convertRemainderWithDUP(afterKeyword);
         size = constSize * (int)rem.size();
         for (int i = 0; i < rem.size(); i++) {
            string curOp = rem[i];
            string label = InputLine::getFirstWord(curOp);
            if (label == "?") {
               for (int k=0; k < constSize; k++) {
                  Sections::entries[Sections::entries.size() - 1].content.push_back('?');
               }
               locationCounter += constSize;
               continue;
            }
            
            Section currentSection = Sections::entries[Sections::entries.size() - 1];
            if (label != "?" && currentSection.name.find(".bss") != string::npos) {
               cout << "Error: All data in .bss section must be '?'!" << endl;
               throw exception();
            }
            
            int value = 0;
            int labelLocation = -1;
            if (isValidString(label)) {
               bool labelFound = false;
               for (int i=0; i < SymbolTable::entries.size(); i++) {
                  if (SymbolTable::entries[i].name.compare(label) == 0) {
                     if (SymbolTable::entries[i].flags == "ABS") {
                        // If first word in expression is abs symbol than it must be a constant expression
                        value = getExpressionValue(curOp);
                        labelLocation = 0;
                        labelFound = true;
                        break;
                     }
                     labelLocation = i;
                     labelFound = true;
                     break;
                  }
               }
               
               if (!labelFound) {
                  SymbolTableEntry ste = SymbolTableEntry();
                  ste.addr = 0;
                  ste.flags = "?";
                  ste.name = label;
                  ste.numID = (int)SymbolTable::entries.size();
                  ste.sectionID = 0;
                  ste.type = "SYM";
                  SymbolTable::pushBack(ste);
                  labelLocation = (int)SymbolTable::entries.size() - 1;
               }
               
               if (labelLocation != 0) {
                  SymbolTableEntry ste = SymbolTable::entries[labelLocation];
                  
                  RelocationTableEntry rte = RelocationTableEntry();
                  if (ste.flags == "L") { // If local label - insert its address into value and set its section index in rel.tab.
                     value = ste.addr;
                     rte.numID = ste.sectionID;
                  } else { // If not local label - set its index into rel.tab.
                     rte.numID = SymbolTable::entries[labelLocation].numID;
                  }
                  rte.address = locationCounter;
                  rte.type = "A";
                  Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
                  curOp = curOp.substr(label.length());
               }
            }
            
            if (labelLocation != 0) {
               value += getExpressionValue(curOp);
            }
            
            for (int k=0; k < constSize; k++) {
               Sections::entries[Sections::entries.size() - 1].content.push_back(char(value));
               value = value >> 8;
            }
            locationCounter += constSize;
         }
         break;
      }
      case 1: // regular Directive
         switch (getRDType(keyWord)) {
            case 0: {// ".global"
               type = ".global";
               vector<string> rem = InputLine::splitOperands(afterKeyword);
               bool foundLabel = false;
               for (int i=0; i < rem.size(); i++) {
                  for (int j = 0; j < SymbolTable::entries.size(); j++) {
                     if (SymbolTable::entries[j].name.compare(rem[i]) == 0) {
                        SymbolTable::entries[j].flags = "G";
                        foundLabel = true;
                        break;
                     }
                  }
                  
                  if (!foundLabel) {
                     SymbolTableEntry entry = SymbolTableEntry();
                     entry.type = "SYM";
                     entry.numID = (int)SymbolTable::entries.size();
                     entry.name = rem[i];
                     entry.flags = "G";
                     entry.sectionID = -1;
                     entry.addr = -1;
                     SymbolTable::pushBack(entry);
                  }
               }
               size = 0;
               break;
            }
            case 1: {// "ORG"
               type = "ORG";
               string rem = afterKeyword;
               try {
                  size = getExpressionValue(afterKeyword);
               }catch(...) {
                  cout << "Error: Could not calculate expression in ORG directive!" << endl;
                  throw exception();
               }
               break;
            }
            default:
               break;
         }
         break;
      case 2:{ // other Directive
         // "DEF"
         type = "DEF";
         size = 0;
         break;
      }
      default:
         type = "NONE";
         break;
   }
}


int Directive::getSize() {
   return size;
}

string Directive::getType() {
   return type;
}

int Directive::getType(string directiveFW, string directiveSW) {
   for(int i=0; i<dataDefiningDirsLength; i++){
      if(directiveFW.compare(dataDefiningDirs[i]) == 0) {
         return 0;
      }
   }
   for(int i=0; i<regularDirsLength; i++){
      if(directiveFW.compare(regularDirs[i]) == 0) {
         return 1;
      }
   }
   for(int i=0; i<otherDirsLength; i++){
      if(directiveSW.compare(otherDirs[i]) == 0) {
         return 2;
      }
   }
   
   return -1;
}


int Directive::getDDDType(string directive) {
   for (int i=0; i < dataDefiningDirsLength; i++) {
      if (directive.compare(dataDefiningDirs[i]) == 0) {
         return i;
      }
   }
   return -1;
}

int Directive::getRDType(string directive) {
   for (int i=0; i < regularDirsLength; i++) {
      if (directive.compare(regularDirs[i]) == 0) {
         return i;
      }
   }
   return -1;
}

vector<string> Directive::convertRemainderFromPos(vector<string> remV, int pos) {
   string rem = getRemainderFromVectorPosition(remV, pos);
   
   int curi = 0;
   vector<string> sepByCommas = vector<string>();
   for (int i=0; i<rem.length(); i++) {
      if (rem[i] == ',') {
         if (curi == i) {
            throw exception();
         }
         sepByCommas.push_back(rem.substr(curi, i - curi));
         curi = i + 1;
      }
   }
   
   if (curi == rem.length()) {
      cout << "Error: Double comma in expression!" << endl;
      throw exception();
   }
   
   vector<string> convertedVector = vector<string>();
   sepByCommas.push_back(rem.substr(curi, rem.length() - curi));
   for (int i=0; i < sepByCommas.size(); i++) {
      if (sepByCommas[i].find("DUP") != string::npos) {
         string leftOfDup = sepByCommas[i].substr(0, sepByCommas[i].find("DUP"));
         string rightOfDup = sepByCommas[i].substr(sepByCommas[i].find("DUP") + 3, sepByCommas[i].length() - (sepByCommas[i].find("DUP") + 3));
         
         int leftOfDupVal = getExpressionValue(leftOfDup);
         
         for (int i=0; i < leftOfDupVal; i++) {
            convertedVector.push_back(rightOfDup);
         }
      } else {
         convertedVector.push_back(sepByCommas[i]);
      }
   }
   
   return convertedVector;
}


// No commas
vector<string> Directive::convertRemainderWithDUP(string remainder) {
   
   vector<string> sepByCommas = InputLine::splitOperands(remainder);
   vector<string> convertedVector = vector<string>();
   for (int i=0; i < sepByCommas.size(); i++) {
      if (sepByCommas[i].find("DUP") != string::npos) {
         string leftOfDup = sepByCommas[i].substr(0, sepByCommas[i].find("DUP"));
         string rightOfDup = sepByCommas[i].substr(sepByCommas[i].find("DUP") + 3, sepByCommas[i].length() - (sepByCommas[i].find("DUP") + 3));
         
         leftOfDup = trimSpacesFromStr(leftOfDup);
         rightOfDup = trimSpacesFromStr(rightOfDup);
         
         int leftOfDupVal = 0;
         
         try {
            leftOfDupVal = getExpressionValue(leftOfDup);
         } catch (...) {
            cout << "Error: Left of DUP is not a constant expression!" << endl;
            throw exception();
         }
         
         for (int j=0; j < leftOfDupVal; j++) {
            convertedVector.push_back(rightOfDup);
         }
      } else {
         convertedVector.push_back(sepByCommas[i]);
      }
   }
   return convertedVector;
}

