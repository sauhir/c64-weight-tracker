/* Weight Tracker 64
 *
 * Copyright (C) 2019 Sauli Hirvi
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"

#ifndef __entry_h_
#define __entry_h_

#include <stdbool.h>

void Entry_parse(unsigned char *, struct Entry *);
void Entry_print(struct Entry *);
void Entry_save_month(struct Entries *, unsigned int, unsigned char);
void Entry_save(struct Entry *);
struct Entry *Entry_find(struct Entries *, struct Date *);
void Entry_swap(struct Entry *, struct Entry *);
void Entry_sort(struct Entries *);
char *Entry_format_weight(unsigned int);
bool Entry_validate(struct Entry *);
unsigned char *Entry_to_csv(struct Entry *);

#endif
