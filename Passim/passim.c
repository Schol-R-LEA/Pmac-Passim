/* passim.c - assembler for the pmac stack machine simulator.
 * version 00.01.00
 * Copyright (C) 2009  Joseph Osako
 * This file is part of Pmac-Passim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "passim.h"
#include "init.h"
#include "symtab.h"
#include "errors.h"
#include "analysis.h"
#include "assemble.h"


/* globals */
FILE *source, *dest, *listing;
bool LIST;   /* listing flag */
unsigned long counter;  /* position counter */

/* tables */
symtable labels, opcodes, direct_args, address_args, indexed_args;

/* main() - driver program for the assembler
   - Reads in the shell args, which are passed to init() for parsing.
   - passes the source file to the analyzer, then passes the source
     file and the symbol table to the assembler proper.   
 */

int main(int argc, char *argv[])
{
    printf("Initializing...");
    init(argc, argv);
    puts("finished.\n");
    puts("Pass one...\n");
    analyze();     // pass one - build symbol table and determine positions
    puts("\n\nPass One complete.");
    puts("Symbol Table: ");
    if (LIST)
    {
        fputs("Symbol Table: \n---------------------------------\n", listing);
    }
    print_table(labels);
    LIST = false;  // only print source listing in first pass
    printf("\nPass two...\n");
    assemble();    // pass two - turn instructions to opcodes
    puts("\nfinished.");          
    finish(NULL, SUCCEED);
}


