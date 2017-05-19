//
//  directives.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 18/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include "directives.h"

string Directive::dataDefiningDirs[] = {"DB", "DW", "DD"};
int Directive::dataDefiningDirsLength = 3;
string Directive::regularDirs[] = {".global", "ORG"};
int Directive::regularDirsLength = 2;
string Directive::otherDirs[] = {"DEF"};
int Directive::otherDirsLength = 1;


