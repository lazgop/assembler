//
//  sectons.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 20/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef sectons_hpp
#define sectons_hpp

#include <vector>
#include "section.hpp"

class Sections {
public:
   static vector<Section> entries;
   static void outputSections();
};

#endif /* sectons_hpp */
