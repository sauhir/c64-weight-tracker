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

#include "defs.h"
#include "config.h"
#include "tokens.h"
#include "files.h"

#ifndef __main_h_
#define __main_h_

/* Function prototypes */
unsigned char View_main_menu(void);
void View_dir_list(void);
unsigned char View_dir_list_menu(struct Files *);
void View_entry_list(unsigned char *, struct Entries *);
void View_new_entry(void);

#endif
