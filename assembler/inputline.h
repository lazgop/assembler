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
   string line;
   InputLine(string line) {
      words = vector<string>();
      this->line = line;
//      string word = "";
//      bool labelFound = false;
//      bool spaceFound = false;
//      for (int i=0; i < line.length(); i++) {
//         string c = line.substr(i, 1);
//         
//         // If space was already found skip all contiguos spaces
//         if (c == " " && spaceFound) {
//            continue;
//         }
//         spaceFound = false;
//         
//         // If comment symbol was found skip rest of line
//         if (c == ";") {
//            if (word != "") {
//               words.push_back(word);
//            }
//            break;
//         }
//         
//         // If label symbol was found append it to the label and add the label into words
//         if (c == ":") {
//            if (words.size() == 0 && word == "") {
//               cout << "Error: Using ':' character without label!" << endl;
//               throw exception();
//            }
//            if (word == "") {
//               words[words.size() - 1] += ":";
//            } else {
//               word += c;
//               words.push_back(word);
//               word = "";
//            }
//            labelFound = true;
//         }
//         
//         if (c == " " && !spaceFound) {
//            if (word != "") {
//               words.push_back(word);
//               word = "";
//            }
//            spaceFound = true;
//         }
//         
//         
//      }
//      istringstream iss(line);
//      do {
//         string sub;
//         iss >> sub;
//         
//         bool commentFound = false;
//         if (sub.find(";") != string::npos) {
//            sub = sub.substr(0, sub.find(";"));
//            commentFound = true;
//         }
//         
//         if (!(sub.compare("") == 0)) {
//            words.push_back(sub);
//         }
//         
//         if (commentFound) {
//            break;
//         }
//      } while (iss);
   }
   
   string getFirstWord() {
      return getFirstWord(line);
   }
   static string getFirstWord(string line);
   
   string getNextWordAfter(string word) {
      return getNextWordAfter(word, line);
   }
   static string getNextWordAfter(string word, string line);
   
   string getRemainingStringAfter(string word) {
      return getRemainingStringAfter(word, line);
   }
   static string getRemainingStringAfter(string word, string line);
   
   bool checkIfSomethingBehindWord(string word) {
      return checkIfSomethingBehindWord(word, line);
   }
   static bool checkIfSomethingBehindWord(string word, string line);
   
   static string getFirstOperand(string line);
   static string getNextOperandAfter(string op, string line);
   static vector<string> splitOperands(string line);
};

#endif /* inputline_h */
