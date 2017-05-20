//
//  relocationtableentry.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 20/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef relocationtableentry_hpp
#define relocationtableentry_hpp

#include <string>

using namespace std;

class RelocationTableEntry {
public:
   string type; // A or R
   int address;
   int numID;
};

#endif /* relocationtableentry_hpp */
