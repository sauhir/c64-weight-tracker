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
#include <string.h>
#include "input.h"
#include "defs.h"

/*
 * Read an integer from STDIN.
 */
unsigned int Input_get_integer(void) {
    char input;
    unsigned char i, x, y;
    unsigned int num;
    unsigned char buffer[BUF_LEN];
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
    unsigned char buffer[BUF_LEN];
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
    while (letter = input[i]) {
        if (letter >= '0' && letter <= '9') {
            integer[j] = letter;
            ++j;
        } else if (letter == ',' || letter == '.') {
            break;
        }
        ++i;
    }

    j = 0;
    /* Parse decimal part */
    while (letter = input[i]) {
        if (letter >= '0' && letter <= '9') {
            decimal[j] = letter;
            ++j;
        }
        ++i;
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

    while (letter = input[i]) {
        if (letter >= '0' && letter <= '9') {
            continue;
        } else if (letter == ',' || letter == '.') {
            continue;
        } else {
            return 1;
        }
        ++i;
    }
    return 0;
}
