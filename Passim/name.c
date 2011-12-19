/* name.c - string allocation function for the passim assembler.
 * Used to consistently allocate strings for the symbol table.
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "name.h"

int min(int n, int m);

/* string creation function */

char* create_name(char* name)
{
    char* name_str;
    size_t length;
    
    /* find the size of the array needed and allocate it */
    length = min(strlen(name) + 1, MAX_NAME_SIZE); 
    name_str = calloc(length, sizeof(char)); /* calloc() also zeros the array */
 
    /* copy the string, leaving one extra char for a delimiter */
    strncpy(name_str, name, length - 1);  

    return name_str;   
}


int min(int n, int m)
{
    return (n < m) ? n : m;
}    
