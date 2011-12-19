/* errors.c - error handling in the passim assembler.
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
#include "errors.h"
#include "symtab.h"

#define MAX_ERR 8

unsigned int line_no;


void report_err(char* description)
{
    static short int errnum = 0;

    fprintf(stderr, "\n\tError #%2d Line %4d: %s\n", errnum, line_no, description);
    if (MAX_ERR <= errnum)
        finish("Too many errors", FAIL);
    else
        errnum++;
}    

void finish(char* description, EXITTYPE result)
{
    if (NULL != description) puts(description);
    
    // close the open files
    if (NULL != source)      fclose(source);
    if (NULL != dest)        fclose(dest);
    if (NULL != listing)     fclose(listing);
    
    // empty the tables built during the run
    if (NULL != labels)      empty_table(labels);
    if (NULL != opcodes)     empty_table(opcodes);
    if (NULL != direct_args) empty_table(direct_args);
    if (NULL != opcodes)     empty_table(address_args);
    if (NULL != opcodes)     empty_table(indexed_args);
    exit(result);
}
