#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char st_title_welcome[] = "Life Tracker (c) Sauli Hirvi 2019";
static const char st_title_date[] = "What date is it today?";
static const char st_prompt_day[] = "Day: ";
static const char st_prompt_month[] = "Month: ";
static const char st_prompt_year[] = "Year: ";

static const char st_prompt_lunch[] = "Food? ";
static const char st_prompt_dinner[] = "Dinner? ";
static const char st_prompt_alcohol[] = "Alcohol? ";
static const char st_prompt_stomach[] = "Stomach? ";
static const char st_prompt_activity[] = "Activity? ";
static const char st_prompt_rating[] = "Rating? ";
static const char st_prompt_weight[] = "Weight? ";

#define BUF_LEN 10

static const char KEY_NEWLINE = 13;
static const char KEY_BACKSPACE = 20;

struct Decimal {
    int integer;
    int fraction;
};

struct Entry {
    int year;
    int month;
    int day;
    struct Decimal weight;
};

char day, month;
int year;
char buffer[BUF_LEN];

FILE *fp;
char temp[1024];
char filename[16];
struct Decimal *weight;

int read_number(void);
void read_decimal(struct Decimal *);
void parse_decimal(char *, struct Decimal *);

int main(void) {
    clrscr();

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

    while ((letter = input[i++]) != NULL) {
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
int read_number(void) {
    char input;
    char i;
    int num;

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
        // Ignore everything except numeric input
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
    char i;

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
        // Ignore everything except numeric input
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
    int i, j;
    char integer[5];
    char decimal[5];

    memset(integer, 0, sizeof integer);
    memset(decimal, 0, sizeof decimal);

    i = 0;
    j = 0;
    /* Parse integer part */
    while ((letter = input[i++]) != NULL) {
        if (letter >= '0' && letter <= '9') {
            integer[j++] = letter;
        } else if (letter == ',' || letter == '.') {
            break;
        } 
    }

    j = 0;
    /* Parse decimal part */
    while ((letter = input[i++]) != NULL) {
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

}
