//
//  inputline.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef inputline_h
#define inputline_h

#include <sstream>
#include <vector>
#include <string>

using namespace std;

class InputLine {
public:
   vector<string> words;
   
   InputLine(string line) {
      words = vector<string>();
      istringstream iss(line);
      do {
         string sub;
         iss >> sub;

         if (!(sub.compare("") == 0)) {
            words.push_back(sub);
//            cout << "Substring: " << sub << endl;
         }
      } while (iss);
//      cout << endl;
   }
};

#endif /* inputline_h */
