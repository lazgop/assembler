//
//  symboltableentry.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef symboltableentry_h
#define symboltableentry_h

using namespace std;

class SymbolTableEntry{
public:
   string type;  //SEG/SYM
   int numID;
   string name;
   int sectionID; // -1 for ABS
   int addr; //0x...
   int size; //0x...
   string flags; //for SYM 'G' or 'L'
};

#endif /* symboltableentry_h */
