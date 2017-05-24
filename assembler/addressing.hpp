//
//  addressing.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 19/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef addressing_hpp
#define addressing_hpp

#include <string>
#include "instructions.h"

using namespace std;

class Addressing {
public:
   
   static int resolveAddressingAndReturnDisplacement(string op, Instruction *instr, vector<int>);
   static bool isRegisterDirect(string word);
   static bool isRegisterIndirect(string word);
   static bool isRegisterIndirectWithOffset(string word);
   static bool isMemoryDirect(string word);
   static bool isImmediate(string word);
   static bool isPCRelPom(string word);
   static int getType(string word);
};

#endif /* addressing_hpp */
