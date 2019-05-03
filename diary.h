
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

struct Entry {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int weight;
};

struct Config {
    struct Date last_date;
    struct Entry max_weight;
    struct Entry min_weight;
};

struct Files {
    unsigned char *list[NUM_FILES];
    unsigned char count;
};

/* Function prototypes */
unsigned int read_number(void);
unsigned char main_menu(void);
void view_directory_list(void);
void view_new_entry(void);
void cleanup(void);
int is_valid_decimal(char *);
unsigned int read_number(void);
unsigned int read_decimal(void);
unsigned int parse_decimal(char *);
void parse_tokens(char *, char **);
void process_file_name(char *, int, char **);
void read_directory(DIR *, struct Files *);
unsigned char Config_load(void);
unsigned char Config_save(void);
void Entry_parse(char *, struct Entry *);
void Entry_print(struct Entry *entry);
void Entry_swap(struct Entry *, struct Entry *);
void Entry_sort(struct Entry *, unsigned char);
void open_file(char *);
void Date_increment(struct Date *);
char *format_weight_str(unsigned int);
struct Date *date_from_filename(unsigned char *);
#endif
