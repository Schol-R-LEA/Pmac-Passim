/* token.c - token processing for the passim assembler.
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
#include <ctype.h>

#include "passim.h"
#include "name.h"
#include "token.h"
#include "errors.h"

char next_char = ' ';
char token_buffer[MAX_NAME_SIZE];
T_TYPE token_type;
int pos;

/* local functions */
void get_name();
void get_number();
void get_sharp();
void get_at();
void get_rbracket();
void get_lbracket();
void get_newline();
void skip_comment();

/*
   tokenize input
   This is the entry point to the tokenizers state machine.
   Based on the first character, it calls a collector function
   which implements a given state behavior. This is a very simple
   FSM with the following states: start, newline, label, name,
   numeric, sharp, lbracket, rbracket, comment and finish.
*/
void get_token()
{
    token_type = T_NONE;  /* no current token */
    token_buffer[0] = DELIM;

    /* eat any whitespace except newline */

    do
    {
        next_char = toupper(fgetc(source));
        if (feof(source) || ferror(source))
        {
            token_type = T_END;
            return;
        }
        putchar(next_char);
        if (LIST)
        {
            fputc(next_char, listing);
        }
    }
    while ('\n' != next_char && isspace(next_char));

    if (isalpha(next_char))
    {
        get_name();
    }
    else if (isxdigit(next_char))
    {
        get_number();
    }
    else
    {
        switch(next_char)
        {
        case ';' :
            skip_comment();
            break;
        case '#' :
            get_sharp();
            break;
        case '@' :
            get_at();
            break;
        case '[' :
            get_rbracket();
            break;
        case ']' :
            get_lbracket();
            break;
        case '\n':
            get_newline();
            break;
        default:
            if (feof(source) || ferror(source))
            {
                token_type = T_END;
            }
            else
            {
                report_err("Invalid character");
                printf("%x\n", (int) next_char);
            }
            break;
        }
    }
}


/* get_name() - read in an alphanumeric lexical element
 * and determine if it is a new label declaration
 * or an opcode or an identifier which is declared elsewhere.
*/
void get_name()
{
    int pos;  /* position in the buffer */

    for(pos = 0; (isalnum(next_char)) && ((MAX_NAME_SIZE - 1) > pos); pos++)
    {
        token_buffer[pos] = next_char;
        next_char = toupper(fgetc(source));
        putchar(next_char);
        if (LIST)
        {
            fputc(next_char, listing);
        }
    }

    if (MAX_NAME_SIZE == pos)
    {
        report_err("Name or label too long.");
        token_type = T_NONE;
    }
    else if (':' == next_char)
    {
        token_type = T_LABEL;
    }
    else
    {
        token_type = T_NAME;
    }
    token_buffer[pos] = DELIM;
}


void get_number()
{
    int pos;

    for(pos = 0; (isxdigit(next_char)) && ((MAX_NAME_SIZE - 1) > pos); pos++)
    {
        token_buffer[pos] = next_char;
        putchar(next_char);
        if (LIST)
        {
            fputc(next_char, listing);
        }
        next_char = toupper(fgetc(source));
    }

    if (MAX_NAME_SIZE == pos)
    {
        report_err("Number too large.");
        token_type = T_NONE;
    }
    else
    {
        token_type = T_NUMBER;
    }
    token_buffer[pos] = '\00';
}


void get_sharp()
{
    token_type = T_SHARP;
}


// At this time, @ syntax isn't supported
void get_at()
{
    token_type = T_AT;
}


// At this time, '[]' syntax isn't supported
void get_lbracket()
{
    token_type = T_LBRACKET;
}


void get_rbracket()
{
    token_type = T_RBRACKET;
}


void get_newline()
{
    line_no++;
    printf("%4d: ", line_no);
    token_type = T_NEWLINE;
}


void skip_comment()
{
    while ('\n' != next_char && !feof(source))
    {
        next_char = fgetc(source);
        putchar(next_char);
        if (LIST)
        {
            fputc(next_char, listing);
        }
    }
    get_newline();
}


void eat_line()
{
    while ('\n' != next_char && !feof(source))
    {
        next_char = fgetc(source);
        putchar(next_char);
        if (LIST)
        {
            fputc(next_char, listing);
        }
    }
    get_newline();
}


/* print_toktype() - debugging utility function
 * which prints out the token types.
 * Not currently used in the program itself.
 */
void print_toktype()
{
    printf("\ttoken type: ");
    switch (token_type)
    {
    case T_NONE:
        printf("T_NONE");
        break;
    case T_LABEL:
        puts("T_LABEL");
        break;
    case T_NAME:
        puts("T_NAME");
        break;
    case T_NUMBER:
        puts("T_NUMBER");
        break;
    case T_SHARP:
        puts("T_SHARP");
        break;
    case T_AT:
        puts("T_AT");
        break;
    case T_LBRACKET:
        puts("T_LBRACKET");
        break;
    case T_RBRACKET:
        puts("T_RBRACKET");
        break;
    case T_NEWLINE:
        puts("T_NEWLINE");
        break;
    default:
        puts("invalid token type");
    }
}

