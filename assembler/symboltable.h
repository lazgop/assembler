//
//  symboltable.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef symboltable_h
#define symboltable_h

#include <vector>
#include <fstream>
#include "symboltableentry.h"
using namespace std;

class SymbolTable{
public:
   static vector<SymbolTableEntry> entries;
   static bool contains(string name);
   static void pushBack(SymbolTableEntry);
   
   static void outputSymbolTable(ofstream &out);
   static void outputSymbolTable();
};
#endif /* symboltable_h */
