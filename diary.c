#include <conio.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "diary.h"

static const char st_title_welcome[] = "Life Tracker (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day";
static const char st_prompt_month[] = "Month";
static const char st_prompt_year[] = "Year";

static const char st_prompt_weight[] = "Weight: ";
static const char st_prompt_lunch[] = "Food: ";
static const char st_prompt_alcohol[] = "Alcohol: ";
static const char st_prompt_activity[] = "Activity: ";
static const char st_prompt_rating[] = "Rating: ";

unsigned char day;
unsigned char month;
unsigned int year;
char buffer[BUF_LEN];

FILE *fp;
DIR *dp;
struct dirent *ep;

char filename[17];
struct Decimal *weight;

char *tmp_ptr;

char **files;
char files_length;

char **tokens;

struct Config config;

unsigned char status;

unsigned char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int main(void) {
    unsigned int i;
    clrscr();

    files = (char**)calloc(NUM_FILES, sizeof(char *));
    tokens = (char**)calloc(20, sizeof(char *));

    read_directory(dp, files);

    i = 0;
    while (tmp_ptr = files[i]) {
        printf("%d: %s\n", i, tmp_ptr);
        i++;
    }

    /*
    TODO:
    - Read the contents of a file
    - Parse contents of the file into an array of Entries
    - Print out the Entries
    */

    status = load_config();

    if (status == false) {
        printf("\nConfig file not found.\n");
    } else if (status == true) {
        printf("\nConfig file found.\n");
        printf("%d, %d, %d.\n", config.day, config.month, config.year);
    }

    printf("%s\n", st_title_welcome);
    printf("%s\n", st_title_date);

    year = 0;

    while (year == 0) {
        printf("\n%s [%d]:", st_prompt_year, config.year);
        year = read_number();
        if (year == 0 && config.year > 0) {
            year = config.year;
            break;
        }
    }

    month = 0;
    while (month < 1 || month > 12) {
        printf("\n%s [%d]:", st_prompt_month, config.month);
        month = (char)read_number();
        if (month == 0 && config.month > 0) {
            month = config.month;
            break;
        }
    }

    day = 0;
    while (day < 1 || day > 31) {
        printf("\n%s [%d]:", st_prompt_day, config.day);
        day = (char)read_number();
        if (day == 0 && config.day > 0) {
            day = config.day;
            break;
        }
    }

    printf("\n%s", st_prompt_weight);

    weight = (struct Decimal *)calloc(1, sizeof(struct Decimal));
    read_decimal(weight);

    /* Construct data file name */
    sprintf(filename, "%04d%02d.dat", year, month);

    /*
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
    } else {
        if (fgets(temp, 1024, fp) != NULL) {
            puts(temp);
        }
    }

    fclose(fp);
    */

    sprintf(buffer, "%04d;%02d;%02d;%d;%d\n", year, month, day, weight->integer, weight->fraction);

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

    status = save_config();

    if (status == true) {
        printf("\nConfig file saved.\n");
    } else {
        printf("\nError writing config file.\n");
    }

    cleanup();
    return EXIT_SUCCESS;
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
    free(weight);

    for (i = 0; i < files_length ; i++) {
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
void read_decimal(struct Decimal *decimal) {
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
            parse_decimal(buffer, decimal);
            return;
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
void parse_decimal(char *input, struct Decimal *output) {
    char letter;
    unsigned int i, j;
    char integer[5];
    char decimal[5];

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

    output->integer = atoi(integer);
    output->fraction = atoi(decimal);
}

/*
 * Parse entry string into tokens delimited by semicolons.
 */
void parse_entry(char *input, struct Entry *output) {
    char *token = NULL;

    while (token = strtok(input, ";")) {
        printf("token: %s\n", token);
    }
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
    fn = (char*)calloc(strlen(input), sizeof(char));
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
                printf("%s\n", ep->d_name);
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
unsigned char load_config(void) {
    fp = fopen("config.cfg", "r");
    if (fp == NULL) {
        return false;
    } else {
        if (fgets(buffer, BUF_LEN, fp) != NULL) {
            parse_tokens(buffer, tokens);
            close(fp);
            config.year = atoi(tokens[0]);
            config.month = (unsigned char)atoi(tokens[1]);
            config.day = (unsigned char)atoi(tokens[2]);
            return true;
        } else {
            close(fp);
            return false;
        }
    }
}

/*
 * Save configuration to disk.
 */
unsigned char save_config(void) {
    fp = fopen("config.cfg", "w");
    if (fp == NULL) {
        return false;
    } else {
        sprintf(buffer, "%04d;%02d;%02d", year, month, day);
        fputs(buffer, fp);
        close(fp);
        return true;
    }
}
