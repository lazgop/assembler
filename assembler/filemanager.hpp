//
//  filemanager.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef filemanager_hpp
#define filemanager_hpp

#include <iostream>
#include <fstream>
#include <list>

#include "inputline.h"
using namespace std;

class FileManager {
   FileManager(){}
public:
   static list<InputLine> *readInputFile(string fname) {
      ifstream inputFile;
      inputFile.open(fname);
      if (inputFile.is_open()) {
         list<InputLine> *linelist = new list<InputLine>();
         string line;
         while(getline(inputFile, line)) {
            if (line.compare(".end") == 0) {
               break;
            }
            if (line.compare("") == 0) {
               continue;
            }
            linelist->push_back(InputLine(line));
         }
         inputFile.close();
         return linelist;
      } else {
         // Throw error
      }
      return nullptr;
   }
};

#endif /* filemanager_hpp */
