// Ecrit par Thomas MICHEL

#include "counter.h"

// Ouvre un fichier en lecture à partir du nom du fichier
// Renvoit un descripteur du fichier ouvert
int open_file(char *file_name)
{
    int fh;
    fh = open(file_name, O_RDONLY);
    if (fh == -1)
    {
        perror("Can't open the file");
        exit(-1);
    }
    return fh;
}

// Compte le nombre de lignes d'un fichier dont le nom est donné en entrée
// Renvoit le nombre de lignes de ce fichier
long count_lines(char *file_name)
{
    // Récupération de la taille du fichier
    struct stat v;
    if (stat(file_name, &v) == -1)
    {
        perror("Error while reading file data");
        return -1;
    }
    long size = v.st_size;
    // Création des arguments pour les 4 threads (Répartition du fichier en 4 parties)
    lc_arg arg1 = {{file_name, 0, size / 4}, 0};
    lc_arg arg2 = {{file_name, size / 4, 2 * (size / 4)}, 0};
    lc_arg arg3 = {{file_name, 2 * (size / 4), 3 * (size / 4)}, 0};
    lc_arg arg4 = {{file_name, 3 * (size / 4), size}, 0};
    // Création des 4 threads
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, line_counter, &arg1);
    pthread_create(&t2, NULL, line_counter, &arg2);
    pthread_create(&t3, NULL, line_counter, &arg3);
    pthread_create(&t4, NULL, line_counter, &arg4);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    // Recombinaison des résultats
    return arg1.nb_newlines + arg2.nb_newlines + arg3.nb_newlines + arg4.nb_newlines;
}

// Compte les lignes de la partie du fichier indiquée dans a
// Stocke le résultat dans a->nb_newlines
void *line_counter(void *a)
{
    lc_arg *arg = (lc_arg *)a;
    // Ouverture du fichier et déplacement au début de la partie assignée
    int fh = open_file(arg->fb.file_name);
    if (lseek(fh, arg->fb.start_index, SEEK_SET) == -1)
    {
        perror("Error while moving the cursor in the file");
        exit(-1);
    }

    //Initialisation
    long remaining_char = arg->fb.end_index - arg->fb.start_index;
    char *buf = NULL;
    arg->nb_newlines = 0;

    // Boucle tant qu'il reste des caractères à traiter
    while (remaining_char > 0)
    {
        // Allocation de la mémoire du buffer (limitée pour les gros fichiers)
        int buf_size = MIN(remaining_char, MAX_BUF_SIZE);
        buf = malloc(buf_size * sizeof(char));
        if (read(fh, buf, buf_size) == -1)
        {
            perror("Error while reading the file");
            exit(-1);
        }

        for (long i = 0; i < buf_size; i++)
        {
            if (buf[i] == '\n')
                arg->nb_newlines++;
        }

        remaining_char -= buf_size;
        free(buf);
        buf = NULL;
    }
}

// Compte le nombre de mots d'un fichier dont le nom est donné en entrée
// Renvoit le nombre de mots de ce fichier
long count_words(char *file_name)
{
    // Récupération de la taille du fichier
    struct stat v;
    stat(file_name, &v);
    long size = v.st_size;
    // Création des arguments pour les 4 threads (Répartition du fichier en 4 parties)
    wc_arg arg1 = {{file_name, 0, size / 4}, 0, 0, 0};
    wc_arg arg2 = {{file_name, size / 4, 2 * (size / 4)}, 0, 0, 0};
    wc_arg arg3 = {{file_name, 2 * (size / 4), 3 * (size / 4)}, 0, 0, 0};
    wc_arg arg4 = {{file_name, 3 * (size / 4), size}, 0, 0, 0};
    // Création des 4 threads
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, word_counter, &arg1);
    pthread_create(&t2, NULL, word_counter, &arg2);
    pthread_create(&t3, NULL, word_counter, &arg3);
    pthread_create(&t4, NULL, word_counter, &arg4);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    // Recombinaison des résultats
    long nb_words = arg1.nb_words + arg2.nb_words + arg3.nb_words + arg4.nb_words;
    // Si une partie finit sur un caractère qui n'est pas un espace le dernier mot n'est pas compté
    // Soit la partie suivante commence par un espace : dans ce cas on compte le mot final de la partie
    // Soit la partie suivante ne commence pas par un espace : le mot final à été compté avec le premier mot de la partie suivante
    // Si une partie finit par un espace, tous ses mots on bien été comptés
    if (arg2.begin_with_space && (!arg1.end_with_space))
        nb_words++;
    if (arg3.begin_with_space && (!arg2.end_with_space))
        nb_words++;
    if (arg4.begin_with_space && (!arg3.end_with_space))
        nb_words++;
    if (!arg4.end_with_space)
        nb_words++;
    return nb_words;
}

// Compte les mots de la partie du fichier indiquée dans a
// Stocke le résultat dans a->nb_words
// Indique dans a->begin_with_space (resp a->end_with_space) si la partie commence (resp termine) par un espace
void *word_counter(void *a)
{
    wc_arg *arg = (wc_arg *)a;
    // Ouverture du fichier et déplacement au début de la partie assignée
    int fh = open_file(arg->fb.file_name);
    if (lseek(fh, arg->fb.start_index, SEEK_SET) == -1)
    {
        perror("Error while moving the cursor in the file");
        exit(-1);
    }

    //Initialisation
    long remaining_char = arg->fb.end_index - arg->fb.start_index;
    long index_last = remaining_char - 1;
    char *buf = NULL;
    short last_was_space = 1;
    long cur_index = 0;
    arg->nb_words = 0;
    arg->begin_with_space = 0;
    arg->end_with_space = 0;

    // Boucle tant qu'il reste des caractères à traiter
    while (remaining_char > 0)
    {
        // Allocation de la mémoire du buffer (limitée pour les gros fichiers)
        int buf_size = MIN(remaining_char, MAX_BUF_SIZE);
        buf = malloc(buf_size * sizeof(char));

        if (read(fh, buf, buf_size) == -1)
        {
            perror("Error while reading the file");
            exit(-1);
        }
        for (long i = 0; i < buf_size; i++)
        {
            // Nouveau mot lorsque le caractère courant est un espace et le précédent n'en était pas un
            if (buf[i] == '\n' || buf[i] == '\t' || buf[i] == ' ')
            {
                if (cur_index == 0)
                    arg->begin_with_space = 1;
                if (cur_index == index_last)
                    arg->end_with_space = 1;
                if (!last_was_space)
                    arg->nb_words++;
                last_was_space = 1;
            }
            else
            {
                last_was_space = 0;
            }
            cur_index++;
        }

        remaining_char -= buf_size;
        free(buf);
        buf = NULL;
    }
}