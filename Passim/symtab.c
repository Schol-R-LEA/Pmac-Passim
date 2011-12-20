/* symtab.c - symbol table handling for the passim assembler.
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
#include "passim.h"
#include "symtab.h"
#include "name.h"
#include "errors.h"

/* symbol creation and destruction */

symbol* create_symbol(char* name, WORD value, symbol* next)
{
    symbol* newsym;


    newsym = malloc(sizeof(symbol));

    newsym ->name = create_name(name);
    newsym ->value = value;
    newsym ->next = next;

    return newsym;
}


void destroy_symbol(symbol* sym)
{
    free(sym ->name);
    free(sym);
}

void print_symbol(symbol* sym)
{
    printf("%8s: %16.4x", sym ->name, sym ->value);
    if (LIST)
    {
        fprintf(listing, "%8s: %16.4x", sym ->name, sym ->value);
    }
}

/* list functions for tables */

void insert(symtable* table, char* name, WORD value)
{
    symbol *newsym, *curr, *rest;

    newsym = curr = rest = NULL;

    if (NULL == *table)
    {
        *table = (symtable) create_symbol(name, value, NULL);
    }
    else
    {
        curr = (symbol *) *table;
        rest = curr ->next;


        /* seek until you find a symbol that is equal or greater than head */
        while (NULL != rest && (0 < strncmp(name, curr ->name, (strlen(name) + 1))))
        {
            curr = rest;
            rest = rest ->next;
        }

        if (0 == strncmp(name, curr ->name, (strlen(name) + 1)))
        {
            report_err("Duplicate symbol");
        }
        else
        {
            curr ->next = create_symbol(name, value, rest);
        }
    }
}


symbol* match(symtable table, char* name)
{
    symbol *curr;

    curr = (symbol *) table;

    while (NULL != curr && 0 != strcmp(curr ->name, name))
        curr = curr ->next;

    return curr;
}



void print_table(symtable table)
{
    symbol *curr = (symbol *) table;

    while (NULL != curr)
    {
        print_symbol(curr);
        printf("\n");
        if (LIST)
        {
            fprintf(listing, "\n");
        }
        curr = curr ->next;
    }
}


void empty_table(symtable table)
{
    symbol *curr, *next;

    curr = (symbol *) table;

    while (NULL != curr)
    {
        next = curr ->next;
        destroy_symbol(curr);
        curr = next;
    }
}


