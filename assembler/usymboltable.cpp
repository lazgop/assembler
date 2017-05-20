//
//  usymboltable.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 19/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iostream>
#include "usymboltable.h"

using namespace std;

vector<USymbolTableEntry> USymbolTable::entries = vector<USymbolTableEntry>();

bool USymbolTable::contains(string name){
   for(int i=0; i<entries.size() ; i++){
      if(name.compare(entries[i].name) == 0){
         return true;
      }
   }
   return false;
}
void USymbolTable::pushBack(USymbolTableEntry uste){
   if (USymbolTable::contains(uste.name)) {
      cout << "Error: Symbol redeclaration error for symbol: " << uste.name << endl;
      throw exception();
   }
   USymbolTable::entries.push_back(uste);
}
