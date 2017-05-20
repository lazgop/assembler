//
//  relocationtable.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 20/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef relocationtable_hpp
#define relocationtable_hpp

#include <vector>
#include "relocationtableentry.hpp"

using namespace std;

class RelocationTable {
public:
   vector<RelocationTableEntry> entries;
   RelocationTable() {
      entries = vector<RelocationTableEntry>();
   }
};

#endif /* relocationtable_hpp */
