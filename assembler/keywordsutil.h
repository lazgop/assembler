//
//  keywordsutil.h
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef keywordsutil_h
#define keywordsutil_h

using namespace std;

bool isInteger(const std::string & s);
bool isSection(string word);
bool isLabel(string word);
bool isDirective(string word);
bool isRegister(string word);
bool isConstantExpression(string word);
int getExpressionValue(string expr);
bool isCalculatableExpression(string expr);
string getRemainderFromVectorPosition(vector<string>, int);

#endif /* keywordsutil_h */
