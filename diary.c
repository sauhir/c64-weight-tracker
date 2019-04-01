#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char st_welcome[] = "Life Tracker (c) Sauli Hirvi 2019";
static const char st_d_prompt[] = "What date is it today?";
static const char st_d_day[] = "Day: ";
static const char st_d_month[] = "Month: ";
static const char st_d_year[] = "Year: ";

#define BUF_LEN 10

static const char KEY_NEWLINE = 13;
static const char KEY_BACKSPACE = 20;

char day, month;
int year;
char buffer[BUF_LEN];

int get_number(void);

int main(void) {
    clrscr();
    printf("%s\n", st_welcome);
    printf("%s\n", st_d_prompt);

    printf("\n%s", st_d_day);
    day = 0;
    while (0 == day) {
        day = (char)get_number();
    }
    printf("\n%s", st_d_month);
    month = (char)get_number();
    printf("\n%s", st_d_year);
    year = get_number();

    printf("\n%d.%d.%d\n", day, month, year);

    return EXIT_SUCCESS;
}

int get_number(void) {
    char input;
    char i;
    int num;

    memset(buffer, 0, BUF_LEN);
    i = 0;
    while (1) {
        input = cgetc();
        if (KEY_BACKSPACE == input) {
            if (i > 0) --i;
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
