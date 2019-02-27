//
// Created by sachetto on 18/10/17.
//


#include <stdarg.h>

#include "file_utils.h"
#include "../string/sds.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <errno.h>

#ifdef _WIN32
#include <io.h>
#define read _read
#endif

#ifdef linux

#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>


#include <ftw.h>

#endif

#include "../includes/stb_ds.h"


static FILE *logfile = NULL;

void print_to_stdout_and_file(char const *fmt, ...) {
    va_list ap;

    if (!no_stdout) {
        va_start(ap, fmt);
        vprintf(fmt, ap);
        fflush(stdout);
        va_end(ap);
    }

    va_start(ap, fmt);
    if (logfile) {
        vfprintf(logfile, fmt, ap);
        fflush(logfile);
    }
    va_end(ap);
}

void print_to_stderr_and_file_and_exit(char const *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    fflush(stderr);
    va_end(ap);
    va_start(ap, fmt);
    if (logfile) {
        vfprintf(logfile, fmt, ap);
        fflush(logfile);
    }
    va_end(ap);
    exit(EXIT_FAILURE);
}

void open_logfile(const char *path) {

#ifdef _WIN32
    fopen_s(&logfile, path, "w");
#else
    logfile = fopen(path, "w");
#endif

    if (logfile == NULL) {
        fprintf(stderr, "Error opening %s, printing output only in the sdtout (Terminal)\n", path);
    } else {
        printf("Log will be saved in %s\n", path);
    }
}

void close_logfile() {
    if (logfile) fclose(logfile);
}


int cp_file(const char *to, const char *from) {
    int fd_to, fd_from;
    char buf[4096];
    int nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
        char *out_ptr = buf;
        int nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0) {
                nread -= nwritten;
                out_ptr += nwritten;
            } else if (errno != EINTR) {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0) {
        if (close(fd_to) < 0) {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

    out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

char *read_entire_file(char *filename, long *size) {

    FILE *infile;
    char *buffer;
    long numbytes;

    if (!filename) return NULL;

/* open an existing file for reading */
    infile = fopen(filename, "r");

/* quit if the file does not exist */
    if (infile == NULL)
        return NULL;

/* Get the number of bytes */
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);

/* reset the file position indicator to
the beginning of the file */
    fseek(infile, 0L, SEEK_SET);

/* grab sufficient memory for the
buffer to hold the text */
    buffer = (char *) malloc(numbytes * sizeof(char));

/* memory error */
    if (buffer == NULL)
        return NULL;

/* copy all the text into the buffer */
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);

    *size = numbytes;

    return buffer;
}

#ifdef _WIN32
// if typedef doesn't exist (msvc, blah)
typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
#endif

char **read_lines(const char *filename) {
    char **lines = NULL;


    size_t len = 0;
    ssize_t read;

    FILE *fp;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error reading file %s\n", filename);
        return NULL;
    }

    char * line = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        line[strlen(line) - 1] = '\0';
        arrput(lines, strdup(line));
    }

    free(line);
    fclose(fp);

    return lines;

}


#ifndef _WIN32
char **list_files_from_dir(const char *dir, const char *prefix) {

    DIR *dp;

    char **files = NULL;

    struct dirent *dirp;

    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "Error opening %s\n", dir);
        exit(0);
    }

    while ((dirp = readdir(dp)) != NULL) {

        if (prefix) {

            if (strncmp(prefix, dirp->d_name, strlen(prefix)) == 0) {
                arrput(files, strdup(dirp->d_name));
            }

        } else {
            arrput(files, strdup(dirp->d_name));
        }
    }

    closedir(dp);
    return files;
}
#endif

void free_lines_or_dir_list(char **list) {

    int lines_number = arrlen(list);

    for(int i = 0; i < lines_number; i++) {
        free(list[i]);
    }

    arrfree(list);

}

// remove_ext: removes the "extension" from a file spec.
//   mystr is the string to process.
//   dot is the extension separator.
//   sep is the path separator (0 means to ignore).
// Returns an allocated string identical to the original but
//   with the extension removed. It must be freed when you're
//   finished with it.
// If you pass in NULL or the new string can't be allocated,
//   it returns NULL.
//from https://stackoverflow.com/questions/2736753/how-to-remove-extension-from-file-name

char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    // Error checks and allocate string.

    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;

    // Make a copy and find the relevant characters.
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    // If it has an extension separator.

    if (lastdot != NULL) {
        // and it's before the extension separator.
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.
                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.
            *lastdot = '\0';
        }
    }

    return retstr;
}

static void swap_str_ptrs(char const **arg1, char const **arg2)
{
    const char *tmp = *arg1;
    *arg1 = *arg2;
    *arg2 = tmp;
}


void quicksort_strs(char const *args[], unsigned int len)
{
    unsigned int i, pvt=0;

    if (len <= 1)
        return;

    // swap a randomly selected value to the last node
    swap_str_ptrs(args+((unsigned int)rand() % len), args+len-1);

    // reset the pivot index to zero, then scan
    for (i=0;i<len-1;++i)
    {
        if (strcmp(args[i], args[len-1]) < 0)
            swap_str_ptrs(args+i, args+pvt++);
    }

    // move the pivot value into its place
    swap_str_ptrs(args+pvt, args+len-1);

    // and invoke on the subsequences. does NOT include the pivot-slot
    quicksort_strs(args, pvt++);
    quicksort_strs(args+pvt, len - pvt);
}

bool dir_exists(const char *path) {
    struct stat info;

    if(stat( path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

void create_dir(const char *out_dir) {

    //TODO: check for windows dir separators
    int dirs_count;

    sds *all_dirs = sdssplit(out_dir, "/", &dirs_count);
    sds new_dir = sdsempty();

    for(int d = 0; d < dirs_count; d++) {

        new_dir = sdscat(new_dir, all_dirs[d]);
        new_dir = sdscat(new_dir, "/");

        if (!dir_exists (new_dir)) {

            printf ("%s does not exist! Creating!\n", new_dir);
#if defined _MSC_VER
            if (_mkdir(out_dir) == -1)
#else
            if (mkdir(new_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
#endif
            {

                fprintf (stderr, "Error creating directory %s Exiting!\n", new_dir);
                exit (EXIT_FAILURE);
            }
        }

    }

    sdsfree(new_dir);

    sdsfreesplitres(all_dirs, dirs_count);

}

int remove_directory(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;

        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        r2 = remove_directory(buf);
                    }
                    else
                    {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r)
    {
        r = rmdir(path);
    }

    return r;
}