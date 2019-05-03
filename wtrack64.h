/* Weight Tracker 64
 *
 * Copyright (C) 2019 Tubular Softworks
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

#ifndef __diary_h_
#define __diary_h_

#define BUF_LEN 512
#define FILENAME_LEN 16
#define NUM_FILES 24

static const unsigned char MAX_DAYS_IN_MONTH = 31;

static const char KEY_NEWLINE = 13;
static const char KEY_BACKSPACE = 20;

struct Date {
    unsigned char day;
    unsigned char month;
    unsigned int year;
};

struct Entry {
    struct Date date;
    unsigned int weight10x;
};

struct Entries {
    struct Entry list[31];
    unsigned char count;
};

struct Config {
    struct Date last_date;
    struct Entry max_weight;
    struct Entry min_weight;
};

struct Files {
    unsigned char *list[NUM_FILES];
    unsigned char count;
};


/* Function prototypes */
unsigned char View_main_menu(void);
void View_directory_list(void);
void View_new_entry(void);
void cleanup(void);
unsigned int Input_get_integer(void);
unsigned int Input_get_decimal(void);
unsigned int Input_parse_decimal(unsigned char *);
unsigned int Input_validate_decimal(unsigned char *);
void Tokens_parse(unsigned char *, unsigned char **);
void Files_add_file(unsigned char *, unsigned int, unsigned char **);
void Files_read_dir(DIR *, struct Files *);
bool Files_load_entries(unsigned char *);
void Files_list_entries(unsigned char *);
unsigned char Config_load(void);
unsigned char Config_save(struct Date *);
void Entry_parse(unsigned char *, struct Entry *);
void Entry_print(struct Entry *);
void Entry_save_month(unsigned int, unsigned char);
void Entry_save(struct Entry *);
struct Entry *Entry_find(struct Date *);
void Entry_swap(struct Entry *, struct Entry *);
void Entry_sort(struct Entries *);
char *Entry_format_weight(unsigned int);
void Date_increment(struct Date *);
struct Date *Date_parse_filename(unsigned char *);
#endif
