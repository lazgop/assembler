//
//  directives.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef directives_h
#define directives_h

#include <string>
#include <vector>

using namespace std;

class Directive{
   static string dataDefiningDirs[];
   static int dataDefiningDirsLength;
   static string regularDirs[];
   static int regularDirsLength;
   static string otherDirs[];
   static int otherDirsLength;
   
   string type;
   int constSize;
   int size;
   
public:
   // First pass constructor
   Directive(string keyWord, string afterKeyword);
   // Second pass constructor
   Directive(string keyWord, string afterKeyword, int locationCounter);
   
   int getSize();
   string getType();
   int getType(string directiveFW, string directiveSW);
   int getDDDType(string directive);
   int getRDType(string directive);
   
   vector<string> convertRemainderFromPos(vector<string> remV, int pos);
   // No commas
   vector<string> convertRemainderWithDUP(string remainder);
};

#endif /* directives_h */
