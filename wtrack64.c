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

FILE *fp;
DIR *dp;
struct dirent *ep;

unsigned char filename[17];
unsigned int weight;

unsigned char *tmp_ptr;

struct Files files;
struct Entries entries;

unsigned char *tokens[20];

struct Date prev_date;

unsigned char status;

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

    (void)Config_load();

    revers(0);
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_GRAY1);
    textcolor(COLOR_LIGHTGREEN);

    while(1) {
        clrscr();
        choice = View_main_menu();

        if (choice == 1) {
            View_directory_list();
            cgetc();
        } else if (choice == 2) {
            View_new_entry();
            cgetc();
            Config_save(&prev_date);
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
    textcolor(COLOR_GREEN);
    cprintf("\r\nDo you want to:\r\n");
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
        if (input == ' ' || input == 13) {
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
void View_directory_list(void) {
    unsigned char i, selection;
    unsigned char input;
    struct Date *fileDate;

    memset(files.list, 0, NUM_FILES*sizeof(unsigned char *));
    Files_read_dir(dp, &files);

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
    cursor(0);
    input = 0;
    while (1) {
        gotoxy(0,2);
        i = 0;
        while (tmp_ptr = files.list[i]) {
            revers(selection == i);
            fileDate = Date_parse_filename(tmp_ptr);
            cprintf("-> %s %d\r\n", month_names[fileDate->month-1], fileDate->year);
            i++;
        }
        revers(0);

        input = cgetc();

        if (input == ' ' || input == 13) {
            break;
        } else if (input == 'j') {
            selection = (selection+1)%files.count;
        } else if (input == 'k') {
            selection = (selection-1)%files.count;
        } else if (input == 'd') {
            Files_delete(files.list[selection]);
            cprintf("File: %s deleted.\r\n", files.list[selection]);
            return;
        }
    }
    Files_list_entries(files.list[selection]);
    cprintf("\r\nPress any key\n\r");
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
    cprintf("%s\r\n", st_title_date);

    old_entry = NULL;
    Date_increment(&prev_date);
    new_date = prev_date;

    new_date.year = 0;

    while (new_date.year == 0) {
        cprintf("\r\n%s [%d]:", st_prompt_year, prev_date.year);
        new_date.year = Input_get_integer();
        if (new_date.year == 0 && prev_date.year > 0) {
            new_date.year = prev_date.year;
            break;
        }
    }

    new_date.month = 0;
    while (new_date.month < 1 || new_date.month > 12) {
        cprintf("\r\n%s [%d]:", st_prompt_month, prev_date.month);
        new_date.month = (char)Input_get_integer();
        if (new_date.month == 0 && prev_date.month > 0) {
            new_date.month = prev_date.month;
            break;
        }
    }

    new_date.day = 0;
    while (new_date.day < 1 || new_date.day > 31) {
        cprintf("\r\n%s [%d]:", st_prompt_day, prev_date.day);
        new_date.day = (char)Input_get_integer();
        if (new_date.day == 0 && prev_date.day > 0) {
            new_date.day = prev_date.day;
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
    status = Files_load_entries(filename);

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

    prev_date = entry.date;
}

/*
 * Free allocations.
 */
void cleanup(void) {
    int i;
    free(fp);
    free(dp);
    free(ep);
    free(tmp_ptr);

    for (i = 0; i < NUM_FILES ; i++) {
        free(files.list[i]);
    }
    free(files.list);
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
 * Parse tokens from a string delimited by semicolons.
 */
void Tokens_parse(unsigned char *input, unsigned char **output) {
    char *in_token = NULL;
    char *out_token;
    unsigned char i;

    for (in_token = strtok(input, ";"), i = 0; in_token; in_token = strtok(NULL, ";"), i++) {
        out_token = (char*)calloc(strlen(in_token)+1, sizeof(char));
        strcpy(out_token, in_token);
        output[i] = out_token;
    }
}

/*
 * Allocates and adds filename to array.
 */
void Files_add_file(unsigned char *input, unsigned int idx, unsigned char **arr_ptr) {
    char *fn;
    fn = (char*)calloc(strlen(input)+1, sizeof(char));
    strcpy(fn, input);
    arr_ptr[idx] = fn;
}

/*
 * Read directory listing of .dat files into files.
 */
void Files_read_dir(DIR *dp, struct Files *files) {
    unsigned int i;

    dp = opendir (".");

    files->count = 0;

    /* Read the directory and add .dat files to array */
    i = 0;
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (strstr(ep->d_name, ".dat")) {
                Files_add_file(ep->d_name, i, files->list);
                i++;
                files->count = i;
            }
        }
        closedir(dp);
    } else {
        cprintf("Error opening directory\r\n");
    }
}

/*
 * Read entries from file and print them.
 */
bool Files_load_entries(unsigned char *filename) {
    unsigned char i;

    i = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        entries.count = 0;
        return false;
    } else {
        while (fgets(buffer, BUF_LEN, fp) != NULL) {
            Entry_parse(buffer, &entries.list[i++]);
        }
    }
    fclose(fp);
    entries.count = i;
    return true;
}

/*
 * Read entries from file and print them.
 */
void Files_list_entries(unsigned char *filename) {
    unsigned char i, line;
    struct Date *date;
    Files_load_entries(filename);

    date = Date_parse_filename(filename);
    clrscr();
    gotoxy(0, 0);
    textcolor(COLOR_GREEN);
    cprintf("Entries for %s %d:\r\n", month_names[date->month-1], date->year);
    line = 0;
    textcolor(COLOR_LIGHTGREEN);
    free(date);
    Entry_sort(&entries);

    for (i=0; i<entries.count; i++) {
        Entry_print(&entries.list[i]);
        if (++line == LINES_PER_PAGE) {
            cgetc();
            clrscr();
            line = 0;
        }
    }
}

/*
 * Swap file pointers.
 */
void Files_swap(unsigned char *a, unsigned char *b) {
    unsigned char tmp[17];
    strcpy(tmp, a);
    strcpy(a, b);
    strcpy(b, tmp);
}

/*
 * Sort files based on name.
 */
void Files_sort(struct Files *files) {
    unsigned char i;
    unsigned char changed = 0;
    if (files->count == 0) {
        return;
    }
    for (i=0; i<files->count-1; i++) {
        if (strcmp(files->list[i], files->list[i+1]) > 0) {
            Files_swap(files->list[i], files->list[i+1]);
            changed = 1;
        }
    }
    if (changed) {
        Files_sort(files);
    }
}

/*
 * Delete file
 */
void Files_delete(unsigned char *filename) {
    remove(filename);
}

/*
 * Load configuration from disk.
 */
unsigned char Config_load(void) {
    fp = fopen("config.cfg", "r");
    if (fp == NULL) {
        return false;
    } else {
        if (fgets(buffer, BUF_LEN, fp) != NULL) {
            Tokens_parse(buffer, tokens);
            fclose(fp);
            prev_date.year = atoi(tokens[0]);
            prev_date.month = (unsigned char)atoi(tokens[1]);
            prev_date.day = (unsigned char)atoi(tokens[2]);
            return true;
        } else {
            fclose(fp);
            return false;
        }
    }
}

/*
 * Save configuration to disk.
 */
unsigned char Config_save(struct Date *date) {
    if (date == NULL) return false;

    if (date->year ==0 || date->month == 0 || date->day == 0) {
        return false;
    }

    fp = fopen("config.cfg", "w");
    if (fp == NULL) {
        return false;
    } else {
        sprintf(buffer, "%04d;%02d;%02d", date->year, date->month, date->day);
        fputs(buffer, fp);
        fclose(fp);
        return true;
    }
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
    fclose(fp);
}

/*
 * Save entry to disk.
 */
void Entry_save(struct Entry *entry) {
    /* Construct data file name */
    sprintf(filename, "%04d%02d.dat", entry->date.year, entry->date.month);

    sprintf(buffer, "%04d;%02d;%02d;%4d\n", entry->date.year, entry->date.month, entry->date.day, entry->weight10x);

    fp = fopen(filename, "a");
    if (!fp) {
        fp = fopen(filename, "w");
    }

    if (fp) {
        fputs(buffer, fp);
    } else {
        printf("Error opening file\n");
    }
    fclose(fp);
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
