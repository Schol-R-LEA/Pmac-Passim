/* symtab.h
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

#ifndef SYMTAB_H
#define SYMTAB_H

#include "passim.h"



typedef struct sym
{
    char* name;
    WORD value;
    struct sym* next;
} symbol;

typedef symbol* symtable;

/* global tables */
extern symtable tokens, labels, opcodes, direct_args, address_args, indexed_args;

/* symbol table functions */
void insert(symtable* table, char* name, WORD value);
symbol* match(symtable table, char* name);
void empty_table(symtable table);
void destroy_symbol(symbol* sym);
void print_table(symtable table);

#endif
