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
#include "keywordsutil.h"

using namespace std;

class Addressing {
public:
   
   static bool isRegisterDirect(string word) {
      return isRegister(word);
   }
   
   static bool isRegisterIndirect(string word) {
      return isRegister(word.substr(1, word.length() - 2)) && word[0] == '[' && word[word.length() - 1] == ']';
   }

};

#endif /* addressing_hpp */
