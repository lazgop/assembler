//
//  elf_header.cpp
//  assembler
//
//  Created by Lazar Gopcevic on 17/5/17.
//  Copyright © 2017 Lazar Gopcevic. All rights reserved.
//

#include "elf_header.hpp"

ostream& operator <<(ostream& osObject, const Elf32_Ehdr& hdr) {
   osObject << hdr.e_ident;
   osObject << endl;
   
   osObject << char(hdr.e_type & 0x00ff);
   osObject << char(hdr.e_type >> 8);
   osObject << char(hdr.e_machine & 0x00ff);
   osObject << char(hdr.e_machine >> 8);
   osObject << char(hdr.e_version & 0x000000ff);
   osObject << char((hdr.e_version >> 8) & 0x000000ff);
   osObject << char((hdr.e_version >> 16) & 0x000000ff);
   osObject << char((hdr.e_version >> 24) & 0x000000ff);
   osObject << char(hdr.e_entry & 0x000000ff);
   osObject << char((hdr.e_entry >> 8) & 0x000000ff);
   osObject << char((hdr.e_entry >> 16) & 0x000000ff);
   osObject << char((hdr.e_entry >> 24) & 0x000000ff);
   osObject << char(hdr.e_phoff & 0x000000ff);
   osObject << char((hdr.e_phoff >> 8) & 0x000000ff);
   osObject << char((hdr.e_phoff >> 16) & 0x000000ff);
   osObject << char((hdr.e_phoff >> 24) & 0x000000ff);
   osObject << endl;
   
   osObject << char(hdr.e_shoff & 0x000000ff);
   osObject << char((hdr.e_shoff >> 8) & 0x000000ff);
   osObject << char((hdr.e_shoff >> 16) & 0x000000ff);
   osObject << char((hdr.e_shoff >> 24) & 0x000000ff);
   osObject << char(hdr.e_flags & 0x000000ff);
   osObject << char((hdr.e_flags >> 8) & 0x000000ff);
   osObject << char((hdr.e_flags >> 16) & 0x000000ff);
   osObject << char((hdr.e_flags >> 24) & 0x000000ff);
   osObject << char(hdr.e_ehsize & 0x00ff);
   osObject << char(hdr.e_ehsize >> 8);
   osObject << char(hdr.e_phentsize & 0x00ff);
   osObject << char(hdr.e_phentsize >> 8);
   osObject << char(hdr.e_phnum & 0x00ff);
   osObject << char(hdr.e_phnum >> 8);
   osObject << char(hdr.e_shentsize & 0x00ff);
   osObject << char(hdr.e_shentsize >> 8);
   osObject << endl;
   
   osObject << char(hdr.e_shnum & 0x00ff);
   osObject << char(hdr.e_shnum >> 8);
   osObject << char(hdr.e_shstrndx & 0x00ff);
   osObject << char(hdr.e_shstrndx >> 8);
   
   return osObject;
}
