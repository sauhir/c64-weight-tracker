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

#include "date.h"
#include <stdlib.h>

/*
 * Increment the date based on number of days per month.
 */
void Date_increment(struct Date *date) {
    if (date->year ==0 || date->month == 0 || date->day == 0) {
        return;
    }

    date->day;

    if (date->day > days_in_month[date->month-1]) {
        date->day = 1;
        date->month += 1;
    }
    if (date->month > 12) {
        date->month = 1;
        date->year += 1;
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

    for (i=0; i<4; ++i) {
        year[i] = filename[i];
    }
    for (i=0; i<2; ++i) {
        month[i] = filename[i+4];
    }

    date->year = atoi(year);
    date->month = atoi(month);
    date->day = 0;

    free(year);
    free(month);
    return date;
}
