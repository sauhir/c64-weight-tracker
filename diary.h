
#ifndef __diary_h_
#define __diary_h_

#define BUF_LEN 10

#define FILENAME_LEN 16

static const unsigned char NUM_FILES = 24;
static const unsigned char MAX_DAYS_IN_MONTH = 31;

static const char KEY_NEWLINE = 13;
static const char KEY_BACKSPACE = 20;

struct Config {
    unsigned char day;
    unsigned char month;
    unsigned int year;
};

struct Decimal {
    unsigned int integer;
    unsigned int fraction;
};

struct Entry {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    struct Decimal weight;
};

/* Function prototypes */
unsigned int read_number(void);
void read_decimal(struct Decimal *);
void parse_decimal(char *, struct Decimal *);
void add_dat_file(char *, int, char **);
void open_dat_file(char *);
void read_directory(DIR *, char **);
void load_config(struct Config *);
void save_config(struct Config *);

#endif
