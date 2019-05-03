#include <conio.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "diary.h"

static const char st_title_welcome[] = "Weight Tracker 64 (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day";
static const char st_prompt_month[] = "Month";
static const char st_prompt_year[] = "Year";

static const char st_prompt_weight[] = "Weight: ";

unsigned char day;
unsigned char month;
unsigned int year;
char buffer[BUF_LEN];

FILE *fp;
DIR *dp;
struct dirent *ep;

char filename[17];
unsigned int weight;

char *tmp_ptr;

char *files[NUM_FILES];
char files_length;

char *tokens[20];

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

struct Entry entries[31];
unsigned char entries_length;

int main(void) {
    clrscr();

    (void)Config_load();

    printf("Free mem: %zu\n", _heapmemavail());

    switch (main_menu()) {
        case 1:
            list_directory();
            break;
        case 2:
            new_entry();
            break;
    }

    status = Config_save();

    if (status == true) {
        printf("\nConfig file saved.\n");
    }

    cleanup();
    return EXIT_SUCCESS;
}

/*
 * Display main menu choice
 */
unsigned char main_menu(void) {
    unsigned char input;
    printf("%s\n", st_title_welcome);

    while (1) {
        printf("\nDo you want to:\n");
        printf("1) List existing entries\n");
        printf("2) Add a new entry\n");
        printf("Choose: ");
        input = cgetc();
        if (input == '1') {
            return 1;
        } else if (input == '2') {
            return 2;
        }
    }
}

void list_directory(void) {
    unsigned char i;
    unsigned char input;
    read_directory(dp, files);

    printf("\n\nAvailable files:\n");

    i = 0;
    while (tmp_ptr = files[i]) {
        printf("%d) %s\n", i+1, tmp_ptr);
        i++;
    }
    input = 0;
    while (input < 1 || input > files_length) {
        printf("\nSelect file: ");
        input = (char)read_number();
    }
    open_file(files[input-1]);
}

/*
 * Make a new entry from user input.
 */
void new_entry(void) {

    printf("%s\n", st_title_date);

    Date_increment(&prev_date);

    year = 0;

    while (year == 0) {
        printf("\n%s [%d]:", st_prompt_year, prev_date.year);
        year = read_number();
        if (year == 0 && prev_date.year > 0) {
            year = prev_date.year;
            break;
        }
    }

    month = 0;
    while (month < 1 || month > 12) {
        printf("\n%s [%d]:", st_prompt_month, prev_date.month);
        month = (char)read_number();
        if (month == 0 && prev_date.month > 0) {
            month = prev_date.month;
            break;
        }
    }

    day = 0;
    while (day < 1 || day > 31) {
        printf("\n%s [%d]:", st_prompt_day, prev_date.day);
        day = (char)read_number();
        if (day == 0 && prev_date.day > 0) {
            day = prev_date.day;
            break;
        }
    }

    printf("\n%s", st_prompt_weight);

    weight = read_decimal();

    /* Construct data file name */
    sprintf(filename, "%04d%02d.dat", year, month);

    sprintf(buffer, "%04d;%02d;%02d;%4d\n", year, month, day, weight);

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
 * Free allocations.
 */
void cleanup() {
    int i;
    free(fp);
    free(dp);
    free(ep);
    free(tmp_ptr);

    for (i = 0; i < NUM_FILES ; i++) {
        free(files[i]);
    }
    free(files);
}

/*
 * Validate that the passed string contains only numbers
 * or decimal separators.
 */
int is_valid_decimal(char *input) {
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
 * Read an integer from STDIN.
 */
unsigned int read_number(void) {
    char input;
    unsigned char i;
    unsigned int num;

    memset(buffer, 0, BUF_LEN);
    i = 0;
    while (1) {
        input = cgetc();
        if (KEY_BACKSPACE == input) {
            if (i > 0) {
                --i;
                printf("%c", input);
            }
            continue;
        }

        if (KEY_NEWLINE == input) {
            num = atoi(buffer);
            return num;
        }
        /* Ignore everything except numeric input */
        if (input >= '0' && input <= '9') {
            printf("%c", input);
            buffer[i] = input;
            ++i;
        }
    }
    return 0;
}

/*
 * Read a decimal number from STDIN.
 */
unsigned int read_decimal(void) {
    char input;
    unsigned char i;

    memset(buffer, 0, BUF_LEN);
    i = 0;
    while (1) {
        input = cgetc();
        if (KEY_BACKSPACE == input) {
            if (i > 0) {
                --i;
                printf("%c", input);
            }
            continue;
        }

        if (KEY_NEWLINE == input) {
            return parse_decimal(buffer);
        }
        /* Ignore everything except numeric input */
        if (input >= '0' && input <= '9') {
            printf("%c", input);
            buffer[i] = input;
            ++i;
        } else if (input == '.' || input == ',') {
            printf("%c", input);
            buffer[i] = input;
            ++i;
        }
    }
}

/*
 * Parse a decimal number string into a Decimal struct.
 */
unsigned int parse_decimal(char *input) {
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
 * Parse tokens from a string delimited by semicolons.
 */
void parse_tokens(char *input, char **output) {
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
void process_file_name(char *input, int idx, char **arr_ptr) {
    char *fn;
    fn = (char*)calloc(strlen(input)+1, sizeof(char));
    strcpy(fn, input);
    arr_ptr[idx] = fn;
}

/*
 * Read directory listing of .dat files into files.
 */
void read_directory(DIR *dp, char **files) {
    unsigned int i;

    dp = opendir (".");

    /* Read the directory and add .dat files to array */
    i = 0;
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (strstr(ep->d_name, ".dat")) {
                process_file_name(ep->d_name, i, files);
                i++;
                files_length = i;
            }
        }
        closedir(dp);
    } else {
        printf("Error opening directory\n");
    }
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
            parse_tokens(buffer, tokens);
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
unsigned char Config_save(void) {
    if (year ==0 || month == 0 || day == 0) {
        return false;
    }
    fp = fopen("config.cfg", "w");
    if (fp == NULL) {
        return false;
    } else {
        sprintf(buffer, "%04d;%02d;%02d", year, month, day);
        fputs(buffer, fp);
        fclose(fp);
        return true;
    }
}

/*
 * Parse entry string into tokens delimited by semicolons.
 */
void Entry_parse(char *input, struct Entry *output) {
    char *token = NULL;
    unsigned char i;

    for (token = strtok(input, ";"), i = 0; token; token = strtok(NULL, ";"), i++) {
        if (i == 0) {
            output->year = atoi(token);
        } else if (i == 1) {
            output->month = atoi(token);
        } else if (i == 2) {
            output->day = atoi(token);
        } else if (i == 3) {
            output->weight = atoi(token);
        }
    }
}

void Entry_print(struct Entry *entry) {
    char *weight_str;
    weight_str = format_weight_str(entry->weight);

    printf("%d-%02d-%02d: %s\n",
        entry->year, entry->month, entry->day, weight_str);
    free(weight_str);
}


void Entry_swap(struct Entry *a, struct Entry *b) {
    struct Entry tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void Entry_sort(struct Entry *array, unsigned char len) {
    unsigned char i;
    unsigned char changed = 0;
    for (i=0; i<len-1; i++) {
        if (array[i].day > array[i+1].day) {
            Entry_swap(&array[i], &array[i+1]);
            changed = 1;
        }
    }
    if (changed) {
        Entry_sort(array, len);
    }
}

void open_file(char *filename) {
    unsigned char i;
    struct Date *date;
    printf("\nOpening file: '%s'\n", filename);

    i = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file: %d\n", errno);
    } else {
        while (fgets(buffer, BUF_LEN, fp) != NULL) {
            Entry_parse(buffer, &entries[i++]);
        }
    }
    fclose(fp);
    entries_length = i;

    date = date_from_filename(filename);

    printf("Entries for %s %d:\n", month_names[date->month-1], date->year);
    free(date);
    Entry_sort(entries, entries_length);

    for (i=0; i<entries_length; i++) {
        Entry_print(&entries[i]);
    }
}

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

char *format_weight_str(unsigned int weight) {
    unsigned char integer;
    unsigned char decimal;
    unsigned char *str;

    str = (unsigned char *)calloc(5, sizeof(char));
    integer = weight / 10;
    decimal = weight % integer;
    sprintf(str, "%d.%d", integer, decimal);
    return str;
}

struct Date *date_from_filename(unsigned char *filename) {
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

