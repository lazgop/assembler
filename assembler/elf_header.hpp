//
//  elf_header.hpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#ifndef elf_header_hpp
#define elf_header_hpp

#include <stdio.h>
#include <stdint.h>
#include <iostream>

using namespace std;

#define EI_NIDENT 16

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

class Elf32_Ehdr {
public:
   unsigned char e_ident[EI_NIDENT] = {char(0x7f), char(0x45), char(0x4c), char(0x46),
                                       char(0x01), char(0x01), char(0x01), char(0x00),
                                       char(0x00), char(0x00), char(0x00), char(0x00),
                                       char(0x00), char(0x00), char(0x00), char(0x00)};
   Elf32_Half e_type = 1;
   Elf32_Half e_machine = 3;
   Elf32_Word e_version = 1;
   Elf32_Addr e_entry = 0; // TBD
   Elf32_Off e_phoff = 0; // TBD
   Elf32_Off e_shoff = 0; // TBD
   Elf32_Word e_flags = 0; // TBD
   Elf32_Half e_ehsize = 34;
   Elf32_Half e_phentsize = 0; // TBD
   Elf32_Half e_phnum = 0; // TBD
   Elf32_Half e_shentsize = 28;
   Elf32_Half e_shnum = 0; // TBD
   Elf32_Half e_shstrndx = 0; // TBD
   
   friend ostream& operator <<(ostream& osObject, const Elf32_Ehdr& hdr);
};

#endif /* elf_header_hpp */
