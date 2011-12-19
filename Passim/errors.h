/* errors.h
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

#include "passim.h"

#ifndef ERRORS_H
#define ERRORS_H

typedef enum {SUCCEED, FAIL} EXITTYPE;

extern unsigned int line_no;

void finish(char* description, EXITTYPE result);
void report_err(char* description);

#endif
