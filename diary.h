
#ifndef __diary_h_
#define __diary_h_

#define BUF_LEN 512
#define FILENAME_LEN 16
#define NUM_FILES 24

static const unsigned char MAX_DAYS_IN_MONTH = 31;

static const char KEY_NEWLINE = 13;
static const char KEY_BACKSPACE = 20;

struct Date {
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
unsigned char main_menu(void);
void list_directory(void);
void new_entry(void);
void cleanup(void);
int is_valid_decimal(char *);
unsigned int read_number(void);
void read_decimal(struct Decimal *);
void parse_decimal(char *, struct Decimal *);
void parse_entry(char *, struct Entry *);
void parse_tokens(char *, char **);
void process_file_name(char *, int, char **);
void read_directory(DIR *, char **);
unsigned char load_config(void);
unsigned char save_config(void);
void print_entry(struct Entry *entry);
void open_file(char *);
void swap_entries(struct Entry *, struct Entry *);
void sort_entries(struct Entry *, unsigned char);
void increment_date(struct Date *);

#endif
