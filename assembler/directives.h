//
//  directives.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef directives_h
#define directives_h

#include <stdio.h>
#include <string>
#include <vector>
#include "keywordsutil.h"

using namespace std;

class Directive{
   static string dataDefiningDirs[];
   static int dataDefiningDirsLength;
   static string regularDirs[];
   static int regularDirsLength;
   static string otherDirs[];
   static int otherDirsLength;
   
   string type;
   int size;
   
public:
   Directive(vector<string> text){
      switch(getType(text[0], text[1])){
         case 0: // data defining direcive
            switch (getDDDType(text[0])) {
               case 0: // "DB"
                  size = 4;
                  break;
               case 1: // "DW"
                  size = 8;
                  break;
               case 3: // "DD"
                  size = 16;
                  break;
               default:
                  break;
            }
            break;
         case 1: // regular Directive
            switch (getRDType(text[0])) {
               case 0: // ".global"
                  size = 0;
                  break;
               case 1: // "ORG"
                  size = 0;
                  break;
               default:
                  break;
            }
            break;
         case 2: // other Directive
            // "DEF" - ?
            size = 0;
            //TODO
            break;
         default:
            type = "NONE";
            break;
      }
   }
   
   int getSize() {
      return size;
   }
   
   string getType() {
      return type;
   }
   
   int getType(string directiveFW, string directiveSW) {
      for(int i=0; i<dataDefiningDirsLength; i++){
         if(directiveFW.compare(dataDefiningDirs[i]) == 0) {
            return 0;
         }
      }
      for(int i=0; i<regularDirsLength; i++){
         if(directiveFW.compare(regularDirs[i]) == 0) {
            return 1;
         }
      }
      for(int i=0; i<otherDirsLength; i++){
         if(directiveSW.compare(otherDirs[i]) == 0) {
            return 2;
         }
      }
      
      return -1;
   }
   
   
   int getDDDType(string directive) {
      for (int i=0; i < dataDefiningDirsLength; i++) {
         if (directive.compare(dataDefiningDirs[i]) == 0) {
            return i;
         }
      }
      return -1;
   }

    int getRDType(string directive) {
      for (int i=0; i < regularDirsLength; i++) {
         if (directive.compare(regularDirs[i]) == 0) {
            return i;
         }
      }
      return -1;
   }
};

#endif /* directives_h */
