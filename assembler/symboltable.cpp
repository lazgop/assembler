//
//  symboltable.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 19/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iomanip>
#include <vector>
#include <iostream>
#include "symboltable.h"

using namespace std;

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

void SymbolTable::pushBack(SymbolTableEntry ste) {
   if (SymbolTable::contains(ste.name)) {
      cout << "Error: Symbol redeclaration error for symbol: " << ste.name << endl;
      throw exception();
   }
   SymbolTable::entries.push_back(ste);
}

