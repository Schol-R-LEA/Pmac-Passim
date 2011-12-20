/* analysis.c - First pass of the passim assembler.
 * Goes through the source code, counts the number and size of the
 * instructions, and calculates the addresses of the labels.
 *
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
#include "passim.h"
#include "symtab.h"
#include "token.h"
#include "errors.h"

WORD count_text();

void analyze()
{
    WORD word_count = 0;

    printf("   0: ");

    while(!(feof(source) || ferror(source)))
    {
        get_token();

        // first, see if there is a label at the start of the line
        if (T_LABEL == token_type)
        {
            insert(&labels, token_buffer, word_count);
            get_token();    // get the next token for processing
        }

        // after any possible labels, get either an opcode or a directive
        switch(token_type)
        {
        case T_NAME:
            if (match(opcodes, token_buffer))
            {
                word_count++;
            }
            else if (NULL != match(address_args, token_buffer)
                     || NULL != match(direct_args, token_buffer))
            {
                word_count += 2;
            }
            else if (NULL !=   match(indexed_args, token_buffer))
            {
                word_count += 3;
            }
            else
            {
                // printf("current token : %s", token_buffer);
                report_err("Instruction expected.");
            }

            eat_line();
            break;

        case T_AT:
            word_count += count_text();
            break;
        case T_SHARP:
            word_count++;
            eat_line();
            break;
        case T_NEWLINE:
        case T_END:
            break;    // do nothing
        default:
            // anything else is an error
            report_err("Opcode or directive expected.");
            eat_line();
            break;
        }
    }
}




WORD count_text()
{
    char ch;
    int char_count;

    ch = fgetc(source);

    for (char_count = 0; ('\n' != ch); char_count++)
    {
        ch = fgetc(source);
    }

    return char_count;
}

