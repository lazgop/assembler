//
//  instructions.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef instructions_hpp
#define instructions_hpp

#include <string>
#include <vector>

using namespace std;

class Instruction{
   static string jumpIns[];
   static int jumpInsLength;
   static string lsIns[];
   static int lsInsLength;
   static string stackIns[];
   static int stackInsLength;
   static string alIns[];
   static int alInsLength;
public:
   string type;
   int size;
   int locationCounter;
   vector<char> instruction;
   
   Instruction(string keyWord, string afterKeyword);
   Instruction(string keyWord, string afterKeyword, int lc);
   
   int getSize();
   string getType();
   
   int getType(string instruction);
   
   
   int getJumpType(string instruction);
   
   int getLSType(string instruction);
   int getALType(string instruction);

   int getLSTypeCode(int lsInsType);
};

#endif /* instructions_hpp */
