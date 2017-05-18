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
//string type;  //SEG/SYM
//int numID;
//string name;
//int sectionID; // -1 for ABS
//int addr; //0x...
//int size; //0x...
//string flags;
void SymbolTable::outputSymbolTable() {
   cout << " TYPE "
   << " numID "
   << " name "
   << " sectionID "
   << " addr "
   << " size "
   << " flags "
   << endl;
   
   for (int i=0; i < SymbolTable::entries.size(); i++) {
      cout << " " << SymbolTable::entries[i].type
      << " " << SymbolTable::entries[i].numID
      << " " << SymbolTable::entries[i].name
      << " " << SymbolTable::entries[i].sectionID
      << " " << SymbolTable::entries[i].addr
      << " " << SymbolTable::entries[i].size
      << " " << SymbolTable::entries[i].flags
      << endl;
   }
}

#endif /* symboltable_h */
