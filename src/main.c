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

#include "main.h"
#include "date.h"
#include "input.h"
#include "entry.h"

static const char st_title_welcome[] = "Weight Tracker 64 (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day";
static const char st_prompt_month[] = "Month";
static const char st_prompt_year[] = "Year";
static const char st_prompt_weight[] = "Weight: ";

struct Files files;

struct Entries *entries;

struct Config *config;

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

    config = (struct Config *)calloc(1, sizeof(struct Config));
    entries = (struct Entries *)calloc(1, sizeof(struct Entries));

    (void)Config_load(config);

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
            Config_save(config);
        } else if (choice == 3) {
            cprintf("\r\nShutting down...\r\n");
            break;
        }
    }

    sleep(2);
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

    Files_list_entries(files.list[selection], entries);
    Files_cleanup(&files);
    textcolor(COLOR_GREEN);
    cprintf("Press any key\n\r");
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
            ++i;
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
    unsigned char filename[17];
    struct Entry *new_entry;
    struct Entry *old_entry;
    bool status, is_new;

    new_entry = (struct Entry *)calloc(1, sizeof(struct Entry));

    cprintf("%s\r\n", st_title_date);

    old_entry = NULL;
    Date_increment(&config->last_entry.date);
    new_entry->date = config->last_entry.date;

    new_entry->date.year = 0;

    while (new_entry->date.year == 0) {
        cprintf("\r\n%s [%d]:", st_prompt_year, config->last_entry.date.year);
        new_entry->date.year = Input_get_integer();
        if (new_entry->date.year == 0 && config->last_entry.date.year > 0) {
            new_entry->date.year = config->last_entry.date.year;
            break;
        }
    }

    new_entry->date.month = 0;
    while (new_entry->date.month < 1 || new_entry->date.month > 12) {
        cprintf("\r\n%s [%d]:", st_prompt_month, config->last_entry.date.month);
        new_entry->date.month = (char)Input_get_integer();
        if (new_entry->date.month == 0 && config->last_entry.date.month > 0) {
            new_entry->date.month = config->last_entry.date.month;
            break;
        }
    }

    new_entry->date.day = 0;
    while (new_entry->date.day < 1 || new_entry->date.day > 31) {
        cprintf("\r\n%s [%d]:", st_prompt_day, config->last_entry.date.day);
        new_entry->date.day = (char)Input_get_integer();
        if (new_entry->date.day == 0 && config->last_entry.date.day > 0) {
            new_entry->date.day = config->last_entry.date.day;
            break;
        }
    }

    cprintf("\r\n%s", st_prompt_weight);
    new_entry->weight10x = Input_get_decimal();

    /* Load the file determined by new_date */
    sprintf(filename, "%04d%02d.dat",
        new_entry->date.year, new_entry->date.month);

    memset(entries->list, 0, MAX_ENTRIES);
    entries->count = 0;

    status = Files_load_entries(filename, entries);

    Entry_sort(entries);
    Entry_remove_duplicates(entries);

    /* Check if the current entry already exists */
    if (status == true) {
        old_entry = Entry_find(entries, new_entry);
    }

    if (old_entry != NULL) {
        old_entry->weight10x = new_entry->weight10x;
        is_new = false;
    } else {
        entries->list[entries->count] = *new_entry;
        entries->count += 1;
        is_new = true;
    }

    /* Rewrite the existing entry if exists */
    Entry_save_month(entries, new_entry->date.year, new_entry->date.month);

    cprintf("\r\nEntry saved.\r\n");

    if (is_new == true) {
        config->last_entry.date = new_entry->date;
        config->last_entry.weight10x = new_entry->weight10x;
    } else {
        config->last_entry.date = old_entry->date;
        config->last_entry.weight10x = old_entry->weight10x;
    }
    Config_save(config);
}

