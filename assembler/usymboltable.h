//
//  usymboltable.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 19/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef usymboltable_hpp
#define usymboltable_hpp

#include <vector>
#include <string>

#include "usymboltableentry.h"

using namespace std;

class USymbolTable {
public:
   static vector<USymbolTableEntry> entries;
   static bool contains(string name);
   static void pushBack(USymbolTableEntry);
};

#endif /* usymboltable_hpp */
