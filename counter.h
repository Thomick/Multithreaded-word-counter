// Ecrit par Thomas MICHEL

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUF_SIZE 1000000

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// Structure contenant les informations sur la partie du fichier à traiter
typedef struct file_bit_struct
{
    char *file_name;
    int start_index;
    int end_index;
} file_bit;

// Structure contenant toutes les informations relatives à l'appel de line_counter
typedef struct lc_arg_struct
{
    file_bit fb;
    long nb_newlines;
} lc_arg;

// Structure contenant toutes les informations relatives à l'appel de word_counter
typedef struct wc_arg_struct
{
    file_bit fb;
    long nb_words;
    short begin_with_space;
    short end_with_space;
} wc_arg;

int open_file(char *arg);
long count_lines(char *file_name);
void *line_counter(void *arg);
long count_words(char *file_name);
void *word_counter(void *arg);
