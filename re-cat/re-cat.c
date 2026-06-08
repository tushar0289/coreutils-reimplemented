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

int main(int argc, char **argv) {
    Arguments args = {0};
    size_t size = 0;
    size_t total_size = 0;

    parse_arguments(argc, argv, &args);

    for (int i = 0; i < args.file_count; i++) {
        putchar('\n');
        size = write_files(args.files[i]);

        if (!size)
            continue;

        printf("%zu bytes read from %s", size, args.files[i]);
        total_size += size;
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
    args->files = malloc(args->file_count * sizeof(char *));
    if (args->files == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

    for (int i = 0; i < args->file_count; i++) {
        args->files[i] = argv[i + 1];
    }
}

size_t write_files(const char *path) {
    size_t read_bytes = 0;
    size_t size = 0;
    char buffer[MAX_LEN];

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        perror("Error oepning the file");
        return 0;
    }

    while ((read_bytes = fread(buffer, sizeof(char), sizeof(buffer), fp)) !=
           0) {
        fwrite(buffer, sizeof(char), read_bytes, stdout);
        size += read_bytes;
    }

    if (ferror(fp)) {
        perror(path);
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return size;
}
