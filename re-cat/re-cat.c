#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

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
        total_size = process_inputs(argv, &args);
    }

    if (!args.line_numbers)
        printf("Total bytes written: %zu\n", total_size);

    free(args.files);
    return 0;
}
