#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 128

typedef struct Arguments {
    int file_count;
    char **files;
} Arguments;

void parse_arguments(int argc, char **argv, Arguments *args);
size_t write_files(const char *path);
size_t write_file_stream(FILE *stream, const char *source_name);

int main(int argc, char **argv) {
    Arguments args = {0};
    size_t size = 0;
    size_t total_size = 0;

    parse_arguments(argc, argv, &args);
    if (args.file_count == 0) {
        total_size += write_file_stream(stdin, "stdin");
    } else {
        for (int i = 0; i < args.file_count; i++) {
            putchar('\n');
            size = write_files(args.files[i]);

            if (!size) {
                continue;
            }

            printf("%zu bytes read from %s\n", size, args.files[i]);
            total_size += size;
        }
    }

    printf("Total bytes read: %zu\n", total_size);

    free(args.files);
    return 0;
}

void parse_arguments(int argc, char **argv, Arguments *args) {
    int index = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            puts("Options will be added later.");
            exit(0);
        }
    }

    args->file_count = argc - 1;

    if (args->file_count > 0) {
        args->files = malloc(args->file_count * sizeof(char *));
        if (args->files == NULL) {
            perror("Error allocating memory");
            exit(1);
        }

        for (int i = 0; i < args->file_count; i++) {
            args->files[i] = argv[i + 1];
        }
    }
}

size_t write_files(const char *path) {
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
