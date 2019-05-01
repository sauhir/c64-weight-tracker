#include <conio.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diary.h"

static const char st_title_welcome[] = "Life Tracker (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day: ";
static const char st_prompt_month[] = "Month: ";
static const char st_prompt_year[] = "Year: ";

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

char temp[1024];
char filename[16];
struct Decimal *weight;

char *tempstr;

char **files;

struct Config config;


int main(void) {
    unsigned int i;
    clrscr();

    files = calloc(NUM_FILES, sizeof(char *));

    read_directory(dp, files);

    i = 0;
    while (tempstr = files[i]) {
        printf("%d: %s\n", i, tempstr);
        i++;
    }

    /*
    TODO:
    - Read the contents of a file
    - Parse contenst of the file into an array of Entries
    - Print out the Entries
    */

    load_config(&config);

    printf("%s\n", st_title_welcome);
    printf("%s\n", st_title_date);

    printf("\n%s", st_prompt_day);
    day = 0;
    while (0 == day) {
        day = (char)read_number();
    }
    printf("\n%s", st_prompt_month);
    month = (char)read_number();
    printf("\n%s", st_prompt_year);
    year = read_number();

    printf("\n%s", st_prompt_weight);

    weight = (struct Decimal *)calloc(1, sizeof(struct Decimal));
    read_decimal(weight);

    sprintf(filename, "%04d%02d.dat", year, month);


/*
    fp = fopen(filename, "r");
    if (fp == NULL) {
//        printf("Error opening file\n");
    } else {
        if (fgets(temp, 1024, fp) != NULL) {
            puts(temp);
        }
    }

    fclose(fp);
*/
    sprintf(temp, "%04d;%02d;%02d;%d;%d\n", year, month, day, weight->integer, weight->fraction);

    fp = fopen(filename, "a");
    if (!fp) {
        fp = fopen(filename, "w");
    }

    if (fp) {
        fputs(temp, fp);
    } else {
        printf("Error opening file\n");
    }
    fclose(fp);

    return EXIT_SUCCESS;
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
        } else if (input == '.') {
            printf("%c", input);
            buffer[i] = input;
            ++i;
        }
    }
}

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
        } else if (letter == ',' || letter == '.') {
            break;
        }
    }

    output->integer = atoi(integer);
    output->fraction = atoi(decimal);
}

void parse_entry(char *input, struct Entry *output) {
    char *token = NULL;
    while (token = strtok(input, ";")) {
        printf("token: %s\n", token);
    }
}

void add_dat_file(char *input, int idx, char **arr_ptr) {
    char *fn;
    fn = calloc(strlen(input), sizeof(char));
    strcpy(fn, input);
    arr_ptr[idx] = fn;
}


void read_directory(DIR *dp, char **files) {
    unsigned int i;

    dp = opendir (".");

    /* Read the directory and add .dat files to array */
    i = 0;
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (strstr(ep->d_name, ".dat")) {
                printf("%s\n", ep->d_name);
                add_dat_file(ep->d_name, i, files);
                i++;
            }
        }
        closedir(dp);
    } else {
        printf("Error opening directory\n");
    }
}

void load_config(struct Config *config) {

}

void save_config(struct Config *config) {

}
