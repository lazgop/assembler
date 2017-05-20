//
//  section.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 20/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef section_hpp
#define section_hpp

#include <string>
#include <vector>
#include "symboltable.h"
#include "relocationtable.hpp"

using namespace std;

class Section {
public:
   string name;
   RelocationTable relTable;
   int size;
   int numID;
   vector<char> content;
   
   Section(string name) {
      for (int i = 0; i < SymbolTable::entries.size(); i++) {
         if (name.compare(SymbolTable::entries[i].name) == 0) {
            size = SymbolTable::entries[i].size;
            numID = SymbolTable::entries[i].numID;
            break;
         }
      }
      this->name = name;
      content = vector<char>();
      relTable = RelocationTable();
   }
};

#endif /* section_hpp */
