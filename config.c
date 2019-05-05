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

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "config.h"
#include "tokens.h"
#include "wtrack64.h"

/*
 * Load configuration from disk.
 */
unsigned char Config_load(struct Config *config) {
    FILE *fp;
    unsigned char line=0;
    struct Tokens tokens;
    unsigned char buffer[BUF_LEN];

    fp = fopen("config.cfg", "r");
    if (fp == NULL) {
        return false;
    } else {
        if (fgets(buffer, BUF_LEN, fp) != NULL) {
            if (line == 0) {
                Tokens_parse(buffer, &tokens);
                config->last_entry.date.year = atoi(tokens.list[0]);
                config->last_entry.date.month = (unsigned char)atoi(tokens.list[1]);
                config->last_entry.date.day = (unsigned char)atoi(tokens.list[2]);
                config->last_entry.weight10x = (unsigned char)atoi(tokens.list[3]);
                Tokens_cleanup(&tokens);
            } else if (line == 1) {
                Tokens_parse(buffer, &tokens);
                config->max_weight.date.year = atoi(tokens.list[0]);
                config->max_weight.date.month = (unsigned char)atoi(tokens.list[1]);
                config->max_weight.date.day = (unsigned char)atoi(tokens.list[2]);
                config->max_weight.weight10x = (unsigned char)atoi(tokens.list[3]);
                Tokens_cleanup(&tokens);
            } else if (line == 2) {
                Tokens_parse(buffer, &tokens);
                config->min_weight.date.year = atoi(tokens.list[0]);
                config->min_weight.date.month = (unsigned char)atoi(tokens.list[1]);
                config->min_weight.date.day = (unsigned char)atoi(tokens.list[2]);
                config->min_weight.weight10x = (unsigned char)atoi(tokens.list[3]);
                Tokens_cleanup(&tokens);
            }
        } else {
            fclose(fp); fp = NULL;
            return false;
        }
    }
    fclose(fp); fp = NULL;
    return true;
}

/*
 * Save configuration to disk.
 */
unsigned char Config_save(struct Config *config) {
    unsigned char *csv;
    FILE *fp;
    unsigned char buffer[BUF_LEN];

    if (Entry_validate(&config->last_entry) == false) {
        return false;
    }

    fp = fopen("config.cfg", "w");
    if (fp == NULL) {
        return false;
    } else {
        csv = Entry_to_csv(&config->last_entry);
        fputs(buffer, fp);
        fclose(fp); fp = NULL;
        free(csv);
        return true;
    }
}

