//
//  error.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef error_hpp
#define error_hpp

class Error {
public:
   static void handleArgvError();
   static void handleFileNotOpenError();
};

#endif /* error_hpp */
