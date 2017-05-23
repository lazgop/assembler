//
//  directives.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef directives_h
#define directives_h

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

using namespace std;

class Directive{
   static string dataDefiningDirs[];
   static int dataDefiningDirsLength;
   static string regularDirs[];
   static int regularDirsLength;
   static string otherDirs[];
   static int otherDirsLength;
   
   string type;
   int constSize;
   int size;
   
public:
   Directive(string keyWord, string afterKeyword){
      
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
                  if (isConstantExpression(afterKeyword)) {
                     if (isCalculatableExpression(afterKeyword)) {
                        size = getExpressionValue(afterKeyword);
                     } else {
                        cout << "Error: Const expression in ORG directive is not calculatable!" << endl;
                        throw exception();
                     }
                  } else {
                     cout << "Error: Const expression in ORG directive is not valid expression!" << endl;
                     throw exception();
                  }
                  break;
               }
               default:
                  break;
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
            if (isConstantExpression(rem)) {
               if (isCalculatableExpression(rem)) {
                  int val = getExpressionValue(rem);
                  SymbolTableEntry entry = SymbolTableEntry();
                  entry.type = "SYM";
                  entry.numID = (int)SymbolTable::entries.size();
                  entry.name = keyWord;
                  entry.flags = "ABS";
                  entry.sectionID = -1;
                  entry.addr = val;
                  SymbolTable::pushBack(entry);
                  
                  bool calculatedSymbolInThisPass = false;
                  do {
                     calculatedSymbolInThisPass = false;
                     for (int i=0; i < USymbolTable::entries.size(); i++) {
                        if(isCalculatableExpression(USymbolTable::entries[i].expression) && USymbolTable::entries[i].name != "-1") {
                           int val = getExpressionValue(USymbolTable::entries[i].expression);
                           SymbolTableEntry entry = SymbolTableEntry();
                           entry.type = "SYM";
                           entry.numID = (int)SymbolTable::entries.size();
                           entry.name = USymbolTable::entries[i].name;
                           entry.flags = "ABS";
                           entry.sectionID = -1;
                           entry.addr = val;
                           SymbolTable::pushBack(entry);
                           calculatedSymbolInThisPass = true;
                           USymbolTable::entries[i].name = "-1";
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
               } else {
                  USymbolTableEntry uste = USymbolTableEntry();
                  uste.name = keyWord;
                  uste.expression = rem;
                  USymbolTable::pushBack(uste);
               }
            } else {
               cout << "Error: Right of DEF is not a constant expression!" << endl;
               throw exception();
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
   Directive(string keyWord, string afterKeyword, int locationCounter) {
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
               int val = 0;
               bool foundLabel = false;
               for (int j = 0; j < SymbolTable::entries.size(); j++) {
                  if (SymbolTable::entries[j].name.compare(rem[i]) == 0) {
                     if (SymbolTable::entries[j].sectionID == -1) {
                        val = SymbolTable::entries[j].addr;
                     } else {
                        RelocationTableEntry rte = RelocationTableEntry();
                        rte.numID = SymbolTable::entries[j].numID;
                        rte.address = locationCounter + i * constSize;
                        rte.type = "R";
                        Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
                        val = 0;
                     }
                     foundLabel = true;
                     break;
                  }
               }
               
               if (!foundLabel && isValidString(rem[i])) {
                  SymbolTableEntry entry = SymbolTableEntry();
                  entry.type = "SYM";
                  entry.numID = (int)SymbolTable::entries.size();
                  entry.name = rem[i];
                  entry.flags = "?";
                  entry.sectionID = -1;
                  entry.addr = -1;
                  SymbolTable::pushBack(entry);
                  
                  RelocationTableEntry rte = RelocationTableEntry();
                  rte.numID = entry.numID;
                  rte.address = locationCounter + i * constSize;
                  rte.type = "A";
                  Sections::entries[Sections::entries.size() - 1].relTable.entries.push_back(rte);
                  val = 0;
               } else {
                  val = getExpressionValue(rem[i]);
               }
               
               for (int k=0; k < constSize; k++) {
                  Sections::entries[Sections::entries.size() - 1].content.push_back(char(val));
                  val = val >> 8;
               }
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
                  if (isConstantExpression(rem)) {
                     if (isCalculatableExpression(rem)) {
                        size = getExpressionValue(rem);
                     } else {
                        cout << "Error: Const expression in ORG directive is not calculatable!" << endl;
                        throw exception();
                     }
                  } else {
                     cout << "Error: Const expression in ORG directive is not valid expression!" << endl;
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
   
   
   int getSize() {
      return size;
   }
   
   string getType() {
      return type;
   }
   
   int getType(string directiveFW, string directiveSW) {
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
   
   
   int getDDDType(string directive) {
      for (int i=0; i < dataDefiningDirsLength; i++) {
         if (directive.compare(dataDefiningDirs[i]) == 0) {
            return i;
         }
      }
      return -1;
   }

    int getRDType(string directive) {
      for (int i=0; i < regularDirsLength; i++) {
         if (directive.compare(regularDirs[i]) == 0) {
            return i;
         }
      }
      return -1;
   }
   
   vector<string> convertRemainderFromPos(vector<string> remV, int pos) {
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
   vector<string> convertRemainderWithDUP(string remainder) {

      vector<string> sepByCommas = InputLine::splitOperands(remainder);
      vector<string> convertedVector = vector<string>();
      for (int i=0; i < sepByCommas.size(); i++) {
         if (sepByCommas[i].find("DUP") != string::npos) {
            string leftOfDup = sepByCommas[i].substr(0, sepByCommas[i].find("DUP"));
            string rightOfDup = sepByCommas[i].substr(sepByCommas[i].find("DUP") + 3, sepByCommas[i].length() - (sepByCommas[i].find("DUP") + 3));
            
            leftOfDup = trimSpacesFromStr(leftOfDup);
            rightOfDup = trimSpacesFromStr(rightOfDup);
            if (!isConstantExpression(leftOfDup)) {
               cout << "Error: Left of DUP is not a constant expression!" << endl;
               throw exception();
            }
            
            int leftOfDupVal = getExpressionValue(leftOfDup);
            for (int j=0; j < leftOfDupVal; j++) {
               convertedVector.push_back(rightOfDup);
            }
         } else {
            convertedVector.push_back(sepByCommas[i]);
         }
      }
      return convertedVector;
   }

};

#endif /* directives_h */
