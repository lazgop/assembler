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

#include "keywordsutil.h"
#include "inputline.h"
#include "symboltable.h"
#include "sections.hpp"
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
            
            line = trimSpacesFromStr(line);
            
            if (line.compare("") == 0) {
               continue;
            }
            linelist->push_back(InputLine(line));
         }
         inputFile.close();
         return linelist;
      } else {
         cout << "ERROR: Could not open input file!" << endl;
         throw exception();
      }
      return nullptr;
   }
   
   static void outputToFile(string name) {
      ofstream outputFile;
      outputFile.open(name);
      if (outputFile.is_open()) {
         SymbolTable::outputSymbolTable(outputFile);
         Sections::outputSections(outputFile);
         outputFile << "#end";
         outputFile.close();
      } else {
         cout << "ERROR: Could not open output file!" << endl;
         throw exception();
      }
   }
};

#endif /* filemanager_hpp */
