// Ecrit par Thomas MICHEL

#include "counter.h"

int main(int argc, char *argv[])
{
    // Prise en charge des arguments
    if (argc == 2)
    {
        printf("%ld %s\n", count_words(argv[1]), argv[1]);
    }
    else if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        // On affiche simplement la taille du fichier renvoyée par l'appel systeme stat
        // (On suppose que le texte en en ascii donc que chaque caractere est codé sur 1 octet)
        struct stat v;
        if (stat(argv[2], &v) == -1)
        {
            perror("Error while reading file data");
            return -1;
        }
        printf("%ld %s\n", v.st_size, argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "-l") == 0)
    {
        printf("%ld %s\n", count_lines(argv[2]), argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "-w") == 0)
    {
        printf("%ld %s\n", count_words(argv[2]), argv[2]);
    }
    else
    {
        perror("Invalid arguments: Invalid number of arguments or invalid option");
        exit(-1);
    }
}