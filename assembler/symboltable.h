//
//  symboltable.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef symboltable_h
#define symboltable_h

#include <list>
#include <iomanip>
#include "symboltableentry.h"

using namespace std;

class SymbolTable{
public:
   static vector<SymbolTableEntry> entries;
   static bool contains(string name);
   
   static void outputSymbolTable();
};

vector<SymbolTableEntry> SymbolTable::entries = vector<SymbolTableEntry>();

bool SymbolTable::contains(string name){
   for(int i=0; i<entries.size() ; i++){
      if(name.compare(entries[i].name) == 0){
         return true;
      }
   }
   return false;
}
void SymbolTable::outputSymbolTable() {
   cout << setw(10) << "TYPE"
   << setw(10) << "numID"
   << setw(10) << "name"
   << setw(10) << "sectionID"
   << setw(10) << "addr"
   << setw(10) << "size"
   << setw(10) << "flags"
   << endl;
   
   for (int i=0; i < SymbolTable::entries.size(); i++) {
      cout << setw(10) << SymbolTable::entries[i].type
      << setw(10) << SymbolTable::entries[i].numID
      << setw(10) << SymbolTable::entries[i].name
      << setw(10) << SymbolTable::entries[i].sectionID
      << setw(10) << SymbolTable::entries[i].addr
      << setw(10) << SymbolTable::entries[i].size
      << setw(10) << SymbolTable::entries[i].flags
      << endl;
   }
}

#endif /* symboltable_h */
