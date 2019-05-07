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

#include "entry.h"

/*
 * Parse entry string into tokens delimited by semicolons.
 */
void Entry_parse(unsigned char *input, struct Entry *output) {
    char *token = NULL;
    unsigned char i;

    for (token = strtok(input, ";"), i = 0; token; token = strtok(NULL, ";"), ++i) {
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
 * Return formatted entry.
 */
unsigned char *Entry_format(struct Entry *entry) {
    unsigned char *weight_str, *entry_str;
    weight_str = Entry_format_weight(entry->weight10x);

    entry_str = (unsigned char*)calloc(24, sizeof(unsigned char));

    sprintf(entry_str, "%4d-%02d-%02d: %s kg",
        entry->date.year, entry->date.month, entry->date.day, weight_str);
    free(weight_str);
    return entry_str;
}

/*
 * Save entries for one month to disk.
 */
void Entry_save_month(struct Entries *entries, unsigned int year, unsigned char month) {
    unsigned char i;
    unsigned char *filename;
    FILE *fp;
    unsigned char buffer[BUF_LEN];

    filename = (unsigned char*)calloc(FILENAME_LEN, sizeof(unsigned char));

    /* Construct data file name */
    sprintf(filename, "%04d%02d.dat", year, month);

    fp = fopen(filename, "w");

    if (fp) {
        for (i=0; i<entries->count; ++i) {
            sprintf(buffer, "%04d;%02d;%02d;%4d\n", entries->list[i].date.year, entries->list[i].date.month, entries->list[i].date.day, entries->list[i].weight10x);
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
struct Entry *Entry_find(struct Entries *entries, struct Entry *entry) {
    unsigned char i;

    for (i=0; i<entries->count; ++i) {
        if (entries->list[i].date.day == entry->date.day &&
            entries->list[i].date.month == entry->date.month &&
            entries->list[i].date.year == entry->date.year) {
            return &entries->list[i];
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
    for (i=0; i<entries->count-1; ++i) {
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

    str = (unsigned char *)calloc(6, sizeof(char));
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
 * Remove duplicate entries from a sorted array.
 */
void Entry_remove_duplicates(struct Entries *entries) {
    unsigned char i,j;
    unsigned char prev_day;
    struct Entries *clean_entries;
    clean_entries = (struct Entries *)calloc(1, sizeof(struct Entries));

    prev_day = 0;
    for (i=0, j=0; i<entries->count; ++i) {
        if (entries->list[i].date.day != prev_day) {
            clean_entries->list[j] = entries->list[i];
            ++j;
        }
        prev_day = entries->list[i].date.day;
    }
    clean_entries->count = j;

    for (i=0; i<clean_entries->count; ++i) {
        entries->list[i] = clean_entries->list[i];
    }
    entries->count = clean_entries->count;
    free(clean_entries);
}

