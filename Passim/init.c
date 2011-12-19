/* init.c - shell argument processing for the passim assembler.
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
#include <string.h>
#include "init.h"
#include "passim.h"
#include "symtab.h"
#include "errors.h"

void init(int count, char *list[])
{

    /* parse the arguments */
    parse_args(count, list);
    init_tables();
}

void init_tables()
{
    /* initialize the table pointers */
    labels = opcodes = direct_args = address_args = indexed_args = NULL;

    printf("loading tables:\n");
    /* initialize the opcode table */
    insert(&opcodes, "HALT",  0x0000);
    insert(&direct_args,  "PUSH", 0x0001);
    insert(&indexed_args, "PUSHI", 0x0002);
    insert(&opcodes, "PUSHR", 0x0003);
    insert(&address_args, "PUSHA", 0x0004);
    insert(&opcodes, "PUSHO", 0x0005);
    insert(&opcodes, "PUSHF", 0x0006);
    insert(&opcodes, "PUSHS", 0x0007);
    insert(&opcodes, "PUSHP", 0x0008);
    insert(&opcodes, "PUSHZ", 0x0009);
    insert(&opcodes, "DUP",   0x000A);
    insert(&address_args, "POP", 0x0100);
    insert(&indexed_args, "POPI", 0x0101);
    insert(&opcodes, "POPR",  0x0102);
    insert(&opcodes, "POPO",  0x0103);
    insert(&opcodes, "POPF",  0x0104);
    insert(&opcodes, "POPS",  0x0105);
    insert(&opcodes, "DROP",  0x0106);
    insert(&opcodes, "SWAP",  0x0107);
    insert(&address_args, "BRA", 0x0200);
    insert(&indexed_args, "BRI", 0x0201);
    insert(&address_args, "BRZ", 0x0300);
    insert(&address_args, "BNZ", 0x0301);
    insert(&address_args, "BSR", 0x0400);
    insert(&opcodes, "RTS",   0x0401);
    insert(&opcodes, "EQL",   0x0500);
    insert(&opcodes, "NEQ",   0x0501);
    insert(&opcodes, "LES",   0x0502);
    insert(&opcodes, "LEQ",   0x0503);
    insert(&opcodes, "GRE",   0x0504);
    insert(&opcodes, "GEQ",   0x0505);
    insert(&opcodes, "ADD",   0x0600);
    insert(&opcodes, "INC",   0x06F0);
    insert(&opcodes, "SUB",   0x0700);
    insert(&opcodes, "DEC",   0x07F0);
    insert(&opcodes, "MUL",   0x0800);
    insert(&opcodes, "DIV",   0x0900);
    insert(&opcodes, "MOD",   0x09F0);
    insert(&opcodes, "SHL",   0x0A00);
    insert(&opcodes, "SHR",   0x0B00);
    insert(&opcodes, "IOR",   0x0C00);
    insert(&opcodes, "XOR",   0x0D00);
    insert(&opcodes, "AND",   0x0E00);
    insert(&opcodes, "NOT",   0x0F00);
    insert(&opcodes, "IN",    0x1000);
    insert(&opcodes, "OUT",   0x2000);
}

void parse_args(int count, char *list[])
{
    if (3 > count)
    {
        if (1 == count && (0 == strcmp("--symbols", list[1])))
	{
	    init_tables();
            printf("%10s %16s\n", "Mnemonic", "Opcode");
            puts("----------------------------");
            printf("Unary ops:\n");
            print_table(opcodes);
            printf("Direct ops:\n");
            print_table(direct_args);
            printf("Indexed ops:\n");
            print_table(indexed_args);
            printf("Addressed ops:\n");
            print_table(address_args);
            finish("Table complete.", SUCCEED);
        }
        else
        {
            finish("Usage: <src> <dest> [-l <listing>]", FAIL);
        }
    }

    /* open the two working files */

    if (NULL == (source = fopen(list[1], "r")))
    {
        finish("Source file not found", FAIL);
    }
    if (NULL == (dest = fopen(list[2], "w")))
    {
        finish("Could not open object file", FAIL);
    }

    if (5 == count && 0 == strcmp(list[3], "-l"))
    {
        LIST = true;
        if (NULL == (listing = fopen(list[4], "w")))
        {
            finish("Could not open object file", FAIL);
        }
    }
    else
    {
        LIST = false;
    }
}

