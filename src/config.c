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
#include "main.h"
#include "entry.h"

/*
 * Load configuration from disk.
 */
unsigned char Config_load(struct Config *config) {
    FILE *fp;
    struct Tokens *tokens;
    unsigned char *buffer;
    unsigned char line;
    struct Entry *entry;

    line = 0;

    tokens = (struct Tokens*)calloc(1, sizeof(struct Tokens));
    buffer = (unsigned char *)calloc(BUF_LEN, sizeof(unsigned char));

    fp = fopen("config.cfg", "r");
    if (fp == NULL) {
        return false;
    } else {
        if (fgets(buffer, BUF_LEN, fp) != NULL) {
            entry = NULL;
            if (line == 0) {
                entry = &config->last_entry;
            } else if (line == 1) {
                entry = &config->max_weight;
            } else if (line == 2) {
                entry = &config->min_weight;
            }

            if (entry != NULL) {
                Tokens_parse(buffer, tokens);
                entry->date.year = atoi(tokens->list[0]);
                entry->date.month = (unsigned char)atoi(tokens->list[1]);
                entry->date.day = (unsigned char)atoi(tokens->list[2]);
                entry->weight10x = (unsigned char)atoi(tokens->list[3]);
                Tokens_cleanup(tokens);
            }
            ++line;
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
        fputs(csv, fp);
        fclose(fp); fp = NULL;
        free(csv);
        return true;
    }
}

