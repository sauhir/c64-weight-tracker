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
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <c64.h>

#include "wtrack64.h"

static const char st_title_welcome[] = "Weight Tracker 64 (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day";
static const char st_prompt_month[] = "Month";
static const char st_prompt_year[] = "Year";
static const char st_prompt_weight[] = "Weight: ";

unsigned char buffer[BUF_LEN];

struct Files files;

struct Entries entries;

struct Config config;

unsigned char days_in_month[] = {
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31 };

unsigned char *month_names[] = {
    "Jan", "Feb", "Mar",
    "Apr", "May", "Jun",
    "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec" };

int main(void) {
    unsigned char choice;
    clrscr();

    (void)Config_load(&config);

    revers(0);
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_GRAY1);
    textcolor(COLOR_LIGHTGREEN);

    while(1) {
        clrscr();
        choice = View_main_menu();

        if (choice == 1) {
            View_dir_list();
            cgetc();
        } else if (choice == 2) {
            View_new_entry();
            cgetc();
            Config_save(&config);
        } else if (choice == 3) {
            cprintf("\r\nShutting down...\r\n");
            break;
        }
    }

    sleep(2);
    cleanup();
    clrscr();
    return EXIT_SUCCESS;
}

/*
 * Display main menu choice
 */
unsigned char View_main_menu(void) {
    unsigned char input, selection;

    selection = 0;
    clrscr();
    textcolor(COLOR_GRAY2);
    cprintf("%s\r\n", st_title_welcome);
    cprintf("Free mem: %zu\r\n", _heapmemavail());
    textcolor(COLOR_GREEN);
    cprintf("Do you want to:\r\n");
    textcolor(COLOR_LIGHTGREEN);
    cursor(0);

    while (1) {
        gotoxy(0, 3);
        revers(selection == 0);
        cprintf("-> List existing entries\r\n");
        revers(selection == 1);
        cprintf("-> Add a new entry\r\n");
        revers(selection == 2);
        cprintf("-> Quit program\r\n");
        textcolor(COLOR_GREEN);
        revers(0);
        cprintf("Choose: ");
        textcolor(COLOR_LIGHTGREEN);
        input = cgetc();
        if (input == ' ' || input == KEY_NEWLINE) {
            return selection+1;
        } else if (input == 'j') {
            selection = (selection+1)%3;
        } else if (input == 'k') {
            selection = (selection-1)%3;
        }
    }
}

/*
 * Display directory list menu
 */
void View_dir_list(void) {
    unsigned char selection;

    Files_read_dir(&files);

    selection = 0;
    clrscr();
    gotoxy(0,0);
    textcolor(COLOR_GREEN);
    cprintf("Available records:\r\n");
    textcolor(COLOR_LIGHTGREEN);

    if (files.count == 0) {
        cprintf("No files found.\r\n");
        return;
    } else {
        Files_sort(&files);
    }

    selection = View_dir_list_menu(&files);

    Files_list_entries(files.list[selection], &entries);
    Files_cleanup(&files);
    cprintf("\r\nPress any key\n\r");
}

unsigned char View_dir_list_menu(struct Files *files) {
    unsigned char input;
    unsigned char i;
    unsigned char selection;
    struct Date *file_date;
    unsigned char *file_ptr;

    cursor(0);
    input = 0;
    while (1) {
        gotoxy(0,2);
        i = 0;
        while (file_ptr = files->list[i]) {
            revers(selection == i);
            file_date = Date_parse_filename(file_ptr);
            cprintf("-> %s %d\r\n", month_names[file_date->month-1], file_date->year);
            free(file_date);
            i++;
        }
        revers(0);

        input = cgetc();

        if (input == ' ' || input == 13) {
            return selection;
        } else if (input == 'j') {
            selection = (selection+1)%files->count;
        } else if (input == 'k') {
            selection = (selection-1)%files->count;
        } else if (input == 'd') {
            Files_delete(files->list[selection]);
            cprintf("File: %s deleted.\r\n", files->list[selection]);
            return 255;
        }
    }
}

/*
 * Make a new entry from user input.
 */
void View_new_entry(void) {
    struct Date new_date;
    struct Entry entry;
    struct Entry *old_entry;
    bool status;
    unsigned char filename[17];
    unsigned int weight;

    cprintf("%s\r\n", st_title_date);

    old_entry = NULL;
    Date_increment(&config.last_entry.date);
    new_date = config.last_entry.date;

    new_date.year = 0;

    while (new_date.year == 0) {
        cprintf("\r\n%s [%d]:", st_prompt_year, config.last_entry.date.year);
        new_date.year = Input_get_integer();
        if (new_date.year == 0 && config.last_entry.date.year > 0) {
            new_date.year = config.last_entry.date.year;
            break;
        }
    }

    new_date.month = 0;
    while (new_date.month < 1 || new_date.month > 12) {
        cprintf("\r\n%s [%d]:", st_prompt_month, config.last_entry.date.month);
        new_date.month = (char)Input_get_integer();
        if (new_date.month == 0 && config.last_entry.date.month > 0) {
            new_date.month = config.last_entry.date.month;
            break;
        }
    }

    new_date.day = 0;
    while (new_date.day < 1 || new_date.day > 31) {
        cprintf("\r\n%s [%d]:", st_prompt_day, config.last_entry.date.day);
        new_date.day = (char)Input_get_integer();
        if (new_date.day == 0 && config.last_entry.date.day > 0) {
            new_date.day = config.last_entry.date.day;
            break;
        }
    }

    cprintf("\r\n%s", st_prompt_weight);
    weight = Input_get_decimal();

    entry.weight10x = weight;
    entry.date = new_date;

    /* Load the file determined by new_date */
    sprintf(filename, "%04d%02d.dat",
        new_date.year, new_date.month);
    status = Files_load_entries(filename, &entries);

    /* Check if the current entry already exists */
    if (status == true) {
        old_entry = Entry_find(&new_date);
    }

    if (old_entry != NULL) {
        old_entry->weight10x = entry.weight10x;
    } else {
        entries.list[(entries.count)++] = entry;
    }

    Entry_sort(&entries);
    /* Rewrite the existing entry if exists */
    Entry_save_month(new_date.year, new_date.month);

    cprintf("\r\nEntry saved.\r\n");

    config.last_entry.date = entry.date;
}

/*
 * Free allocations.
 */
void cleanup(void) {

}

/*
 * Read an integer from STDIN.
 */
unsigned int Input_get_integer(void) {
    char input;
    unsigned char i, x, y;
    unsigned int num;

    memset(buffer, 0, BUF_LEN);
    i = 0;
    cursor(1);
    while (1) {
        input = cgetc();
        if (KEY_BACKSPACE == input) {
            if (i > 0) {
                --i;
                x = wherex();
                y = wherey();
                --x;
                cputcxy(x, y, ' ');
                gotoxy(x, y);
            }
            continue;
        }

        if (KEY_NEWLINE == input) {
            num = atoi(buffer);
            return num;
        }
        /* Ignore everything except numeric input */
        if (input >= '0' && input <= '9') {
            cprintf("%c", input);
            buffer[i] = input;
            ++i;
        }
    }
    return 0;
}

/*
 * Read a decimal number from STDIN.
 */
unsigned int Input_get_decimal(void) {
    char input;
    unsigned char i, x, y;

    memset(buffer, 0, BUF_LEN);
    i = 0;
    cursor(1);
    while (1) {
        input = cgetc();
        if (KEY_BACKSPACE == input) {
            if (i > 0) {
                --i;
                x = wherex();
                y = wherey();
                --x;
                cputcxy(x, y, ' ');
                gotoxy(x, y);
            }
            continue;
        }

        if (KEY_NEWLINE == input) {
            return Input_parse_decimal(buffer);
        }
        /* Ignore everything except numeric input */
        if (input >= '0' && input <= '9') {
            cprintf("%c", input);
            buffer[i] = input;
            ++i;
        } else if (input == '.' || input == ',') {
            cprintf("%c", input);
            buffer[i] = input;
            ++i;
        }
    }
}

/*
 * Parse a decimal number string into a Decimal struct.
 */
unsigned int Input_parse_decimal(unsigned char *input) {
    char letter;
    unsigned int i, j;
    char integer[5];
    char decimal[5];
    unsigned int retval = 0;

    memset(integer, 0, sizeof integer);
    memset(decimal, 0, sizeof decimal);

    i = 0;
    j = 0;
    /* Parse integer part */
    while (letter = input[i++]) {
        if (letter >= '0' && letter <= '9') {
            integer[j++] = letter;
        } else if (letter == ',' || letter == '.') {
            break;
        }
    }

    j = 0;
    /* Parse decimal part */
    while (letter = input[i++]) {
        if (letter >= '0' && letter <= '9') {
            decimal[j++] = letter;
        }
    }

    retval = atoi(integer) * 10;
    retval += atoi(decimal);
    return retval;
}

/*
 * Validate that the passed string contains only numbers
 * or decimal separators.
 */
unsigned int Input_validate_decimal(unsigned char *input) {
    char letter;
    int i = 0;

    while (letter = input[i++]) {
        if (letter >= '0' && letter <= '9') {
            continue;
        } else if (letter == ',' || letter == '.') {
            continue;
        } else {
            return 1;
        }
    }
    return 0;
}



/*
 * Parse entry string into tokens delimited by semicolons.
 */
void Entry_parse(unsigned char *input, struct Entry *output) {
    char *token = NULL;
    unsigned char i;

    for (token = strtok(input, ";"), i = 0; token; token = strtok(NULL, ";"), i++) {
        if (i == 0) {
            output->date.year = atoi(token);
        } else if (i == 1) {
            output->date.month = atoi(token);
        } else if (i == 2) {
            output->date.day = atoi(token);
        } else if (i == 3) {
            output->weight10x = atoi(token);
        }
    }
}

/*
 * Print formatted entry.
 */
void Entry_print(struct Entry *entry) {
    char *weight_str;
    weight_str = Entry_format_weight(entry->weight10x);

    cprintf("%d-%02d-%02d: %s kg\r\n",
        entry->date.year, entry->date.month, entry->date.day, weight_str);
    free(weight_str);
}

/*
 * Save entries for one month to disk.
 */
void Entry_save_month(unsigned int year, unsigned char month) {
    unsigned char i;
    unsigned char *filename;
    FILE *fp;

    filename = (unsigned char*)calloc(FILENAME_LEN, sizeof(unsigned char));

    /* Construct data file name */
    sprintf(filename, "%04d%02d.dat", year, month);

    fp = fopen(filename, "w");

    if (fp) {
        for (i=0; i<entries.count; i++) {
            sprintf(buffer, "%04d;%02d;%02d;%4d\n", entries.list[i].date.year, entries.list[i].date.month, entries.list[i].date.day, entries.list[i].weight10x);
            fputs(buffer, fp);
        }
    } else {
        printf("Error opening file\n");
    }
    fclose(fp); fp = NULL;
    free(filename);
}

/*
 * Construct CSV string from entry.
 */
unsigned char *Entry_to_csv(struct Entry *entry) {
    unsigned char *csv;
    csv = (unsigned char*)calloc(32, sizeof(unsigned char));
    sprintf(csv, "%04d;%02d;%02d;%4d\n", entry->date.year, entry->date.month, entry->date.day, entry->weight10x);
    return csv;
}

/*
 * Save entry to disk.
 */
void Entry_save(struct Entry *entry) {
    FILE *fp;
    unsigned char *filename, *csv;

    /* Construct data file name */
    filename = (unsigned char*)calloc(FILENAME_LEN, sizeof(unsigned char));
    sprintf(filename, "%04d%02d.dat", entry->date.year, entry->date.month);

    csv = Entry_to_csv(entry);

    fp = fopen(filename, "a");
    if (!fp) {
        fp = fopen(filename, "w");
    }

    if (fp) {
        fputs(csv, fp);
    } else {
        printf("Error opening file\n");
    }
    fclose(fp); fp = NULL;
    free(csv);
    free(filename);
}

/*
 * Find an entry based on date.
 */
struct Entry *Entry_find(struct Date *date) {
    unsigned char i;
    bool is_equal = true;

    for (i=0; i<entries.count; i++) {
        if (entries.list[i].date.day != date->day) is_equal = false;
        if (entries.list[i].date.month != date->month) is_equal = false;
        if (entries.list[i].date.year != date->year) is_equal = false;
        if (is_equal == true) {
            return &entries.list[i];
        }
    }
    return NULL;
}

/*
 * Swap entry pointers.
 */
void Entry_swap(struct Entry *a, struct Entry *b) {
    struct Entry tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Sort entries based on day.
 */
void Entry_sort(struct Entries *entries) {
    unsigned char i;
    unsigned char changed = 0;
    for (i=0; i<entries->count-1; i++) {
        if (entries->list[i].date.day > entries->list[i+1].date.day) {
            Entry_swap(&entries->list[i], &entries->list[i+1]);
            changed = 1;
        }
    }
    if (changed) {
        Entry_sort(entries);
    }
}

/*
 * Format 10x weight integer into a decimal string.
 */
unsigned char *Entry_format_weight(unsigned int weight) {
    unsigned char integer;
    unsigned char decimal;
    unsigned char *str;

    str = (unsigned char *)calloc(5, sizeof(char));
    integer = weight / 10;
    decimal = weight % integer;
    sprintf(str, "%d.%d", integer, decimal);
    return str;
}

/*
 * Checks whether an entry has all fields set.
 */
bool Entry_validate(struct Entry *entry) {
    if (entry == NULL) return false;

    if (entry->date.day ==0 ||
        entry->date.month == 0 ||
        entry->date.year == 0 ||
        entry->weight10x == 0) {
        return false;
    }
    return true;
}

/*
 * Increment the date based on number of days per month.
 */
void Date_increment(struct Date *date) {
    if (date->year ==0 || date->month == 0 || date->day == 0) {
        return;
    }

    date->day++;

    if (date->day > days_in_month[date->month-1]) {
        date->day = 1;
        date->month++;
    }
    if (date->month > 12) {
        date->month = 1;
        date->year++;
    }
}

/*
 * Parse file name into a Date struct.
 */
struct Date *Date_parse_filename(unsigned char *filename) {
    unsigned char *year, *month;
    unsigned char i;
    struct Date *date;

    year = (unsigned char *)calloc(4+1, sizeof(char*));
    month = (unsigned char *)calloc(2+1, sizeof(char*));
    date = (struct Date *)calloc(1, sizeof(struct Date *));

    for (i=0; i<4; i++) {
        year[i] = filename[i];
    }
    for (i=0; i<2; i++) {
        month[i] = filename[i+4];
    }

    date->year = atoi(year);
    date->month = atoi(month);
    date->day = 0;

    free(year);
    free(month);
    return date;
}
