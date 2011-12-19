/* token.h
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

#ifndef TOKEN_H
#define  TOKEN_H

#include "name.h"

typedef enum {
    T_NONE, 
    T_LABEL, T_NAME, T_NUMBER,
    T_SHARP, T_AT,
    T_LBRACKET, T_RBRACKET, 
    T_NEWLINE,
    T_END
} T_TYPE;

extern char token_buffer[MAX_NAME_SIZE];
extern T_TYPE token_type;

/* tokenizing function */
void get_token();
void eat_line();


void print_toktype();
#endif
