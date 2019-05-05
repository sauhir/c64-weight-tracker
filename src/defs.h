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

#ifndef __defs_h_
#define __defs_h_

#define BUF_LEN 80
#define FILENAME_LEN 17
#define NUM_FILES 24
#define LINES_PER_PAGE 22
#define MAX_ENTRIES 31

#define KEY_NEWLINE 13
#define KEY_BACKSPACE 20

struct Date {
    unsigned char day;
    unsigned char month;
    unsigned int year;
};

struct Entry {
    struct Date date;
    unsigned int weight10x;
};

struct Config {
    struct Entry last_entry;
    struct Entry max_weight;
    struct Entry min_weight;
};

struct Entries {
    struct Entry list[MAX_ENTRIES];
    unsigned char count;
};

struct Files {
    unsigned char *list[NUM_FILES];
    unsigned char count;
};

struct Tokens {
    unsigned char *list[NUM_FILES];
    unsigned char count;
};

extern unsigned char days_in_month[];
extern unsigned char *month_names[];

#endif
