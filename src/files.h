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
#include <stdbool.h>

#ifndef __files_h_
#define __files_h_
void Files_add_file(unsigned char *, unsigned int, unsigned char **);
void Files_read_dir(struct Files *);
bool Files_load_entries(unsigned char *, struct Entries *);
void Files_list_entries(unsigned char *, struct Entries *);
void Files_swap(unsigned char *, unsigned char *);
void Files_sort(struct Files *);
void Files_delete(unsigned char *);
void Files_cleanup(struct Files *);
#endif
