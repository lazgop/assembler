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
   Directive(vector<string> text){
      switch(getType(text[0], text[1])){
         case 0: {// data defining direcive
            switch (getDDDType(text[0])) {
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
            vector<string> rem = convertRemainder(text);
            size = constSize * (int)rem.size();
            break;
         }
         case 1: // regular Directive
            switch (getRDType(text[0])) {
               case 0: // ".global"
                  type = ".global";
                  size = 0;
                  break;
               case 1: {// "ORG"
                  type = "ORG";
                  string rem = getRemainderFromVectorPosition(text, 1);
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
            string rem = getRemainderFromVectorPosition(text, 2);
            if (isConstantExpression(rem)) {
               if (isCalculatableExpression(rem)) {
                  int val = getExpressionValue(rem);
                  SymbolTableEntry entry = SymbolTableEntry();
                  entry.type = "SYM";
                  entry.numID = (int)SymbolTable::entries.size();
                  entry.name = text[0];
                  entry.flags = "ABS";
                  entry.sectionID = -1;
                  entry.addr = val;
                  SymbolTable::entries.push_back(entry);
                  
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
                           SymbolTable::entries.push_back(entry);
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
                  uste.name = text[0];
                  uste.expression = rem;
                  USymbolTable::entries.push_back(uste);
               }
            } else {
               throw exception();
            }
            size = 0;
            //TODO
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
   
   vector<string> convertRemainder(vector<string> remV) {
      string rem = getRemainderFromVectorPosition(remV, 1);
      
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
};

#endif /* directives_h */
