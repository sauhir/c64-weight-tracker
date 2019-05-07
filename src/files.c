
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "files.h"
#include "main.h"
#include "date.h"
#include "entry.h"

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
void Files_read_dir(struct Files *files) {
    unsigned int i;
    struct dirent *ep;
    DIR *dp;

    dp = opendir (".");
    files->count = 0;
    /* Read the directory and add .dat files to array */
    i = 0;
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (strstr(ep->d_name, ".dat")) {
                Files_add_file(ep->d_name, i, files->list);
                ++i;
                files->count = i;
            }
        }
        closedir(dp);
    } else {
        cprintf("Error opening directory\r\n");
    }
    free(ep);
}

/*
 * Read entries from file and print them.
 */
bool Files_load_entries(unsigned char *filename, struct Entries *entries) {
    unsigned char i;
    FILE *fp;
    unsigned char buffer[BUF_LEN];

    i = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        entries->count = 0;
        return false;
    } else {
        while (fgets(buffer, BUF_LEN, fp) != NULL) {
            Entry_parse(buffer, &entries->list[i]);
            ++i;
        }
    }
    fclose(fp); fp = NULL;
    entries->count = i;
    return true;
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
    for (i=0; i<files->count-1; ++i) {
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
 * Free file list allocations
 */
void Files_cleanup(struct Files *files) {
    unsigned char i;
    for (i=0; i<files->count; ++i) {
        free(files->list[i]);
    }
}

