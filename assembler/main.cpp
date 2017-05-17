//
//  main.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include "error.hpp"
#include "elf_header.hpp"

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
      Elf32_Ehdr hdr = Elf32_Ehdr();
      outputFile << hdr;
      
//      string line;
//      while(getline(inputFile, line)) {
//         line += char(0b1000001);
//         char *chars = new char[line.length()];
//         copy(line.begin(), line.end(), chars);
//         
//         for (int i = 0; i < line.length(); i++) {
//            outputFile << chars[i];
//         }
//         cout << line;
//      }
      
      inputFile.close();
      outputFile.close();
   } else {
      Error::handleFileNotOpenError();
      return 2;
   }
   
   return 0;
}
