//
//  main.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "error.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
   if (argc < 3) {
      Error::handleArgvError();
      return 1;
   }
   
   string inputFileName = argv[1];
   ifstream inputFile;
   inputFile.open(inputFileName);
   
   string outputFileName = argv[2];
   ofstream outputFile;
   outputFile.open(outputFileName);
   
   if (inputFile.is_open() && outputFile.is_open()) {
      string line;
      
      while(getline(inputFile, line)) {
         outputFile << line << endl;
      }
      
      inputFile.close();
      outputFile.close();
   } else {
      Error::handleFileNotOpenError();
      return 2;
   }
   
   return 0;
}
