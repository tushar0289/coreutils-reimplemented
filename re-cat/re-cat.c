#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arguments {
    int file_count;
    char **files;
} Arguments;

void parse_arguments(int argc, char **argv, Arguments *args);

int main(int argc, char **argv) {
    Arguments args = {0};
    size_t size = 0;

    parse_arguments(argc, argv, &args);
    printf("Total files: %d\n", args.file_count);
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

    for (int i = 1; i < argc; i++) {
        index++;
    }
    args->file_count = index;
}
