#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 128

typedef struct Arguments {
    int file_count;
    char **files;

    _Bool long_option;
    _Bool short_option;
    _Bool line_numbers;
    _Bool help;
} Arguments;

int parse_arguments(int argc, char **argv, Arguments *args);
size_t write_files(const char *path, Arguments *args);
size_t write_file_stream(FILE *stream, const char *source_name);
void parse_long_option(int index, char **argv, Arguments *arg);
void parse_short_option(int index, char **argv, Arguments *arg);
void write_help(void);

int main(int argc, char **argv) {
    Arguments args = {0};
    size_t size = 0;
    size_t total_size = 0;

    parse_arguments(argc, argv, &args);

    if (args.help) {
        write_help();
        exit(0);
    }

    if (args.file_count == 0) {
        total_size += write_file_stream(stdin, "stdin");
    } else {
        for (int i = 0; i < args.file_count; i++) {
            putchar('\n');
            size = write_files(args.files[i], &args);

            if (!size) {
                continue;
            }

            printf("\n%zu bytes written from %s\n", size, args.files[i]);
            total_size += size;
        }
    }

    printf("Total bytes written: %zu\n", total_size);

    free(args.files);
    return 0;
}

int parse_arguments(int argc, char **argv, Arguments *args) {
    int track_options = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--", 2) == 0) {
            args->long_option = 1;
            parse_long_option(i, argv, args);
            track_options++;
        } else if (strncmp(argv[i], "-", 1) == 0) {
            args->short_option = 1;
            parse_short_option(i, argv, args);
            track_options++;
        }
    }

    if (args->help)
        return 0;

    args->file_count = argc - track_options - 1;

    if (args->file_count > 0) {
        args->files = malloc(args->file_count * sizeof(char *));
        if (args->files == NULL) {
            perror("Error allocating memory");
            exit(1);
        }

        for (int i = 0; i < argc; i++) {
            args->files[i] = argv[i + 1 + track_options];
        }
    }
    return 1;
}

size_t write_files(const char *path, Arguments *args) {
    size_t read_bytes = 0;
    size_t written = 0;
    size_t size = 0;
    char buffer[MAX_LEN];

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        perror(path);
        return 0;
    }

    while ((read_bytes = fread(buffer, sizeof(char), sizeof(buffer), fp)) !=
           0) {
        written = fwrite(buffer, sizeof(char), read_bytes, stdout);

        size += written;

        if (read_bytes > written) {
            perror("Error writing to standard output");
            fclose(fp);
            return size;
        }
    }

    if (ferror(fp)) {
        perror(path);
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return size;
}

size_t write_file_stream(FILE *stream, const char *source_name) {
    size_t read_bytes = 0;
    size_t written = 0;
    size_t total_written = 0;
    int newline_count = 0;
    char buffer;

    while ((read_bytes = fread(&buffer, 1, sizeof(buffer), stream)) > 0) {
        written = fwrite(&buffer, 1, read_bytes, stdout);
        total_written += written;

        if (buffer == '\n')
            newline_count++;
    }
    if (ferror(stream)) {
        perror(source_name);
    }

    return total_written - newline_count;
}

void parse_long_option(int index, char **argv, Arguments *args) {
    if ((strcmp(argv[index], "--help")) == 0) {
        args->help = 1;
    }
}

void parse_short_option(int index, char **argv, Arguments *args) {
    char *options = argv[index];
    puts(options);
    printf("%zu\n", strlen(options));

    for (int i = 1; i < strlen(options); i++) {
        switch (options[1]) {
        case 'n':
            args->line_numbers = 1;
        }
    }
}

void write_help(void) { printf("Will help you later :)) \n"); }
