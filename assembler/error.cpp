//
//  error.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include "error.hpp"
#include <iostream>

using namespace std;

void Error::handleArgvError() {
   cout << "Invalid arguments!" << endl;
}

void Error::handleFileNotOpenError() {
   cout << "Can't open files!" << endl;
}
