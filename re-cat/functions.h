#ifndef FUNCTIONS_H
#define FUNCTIONS_H
// #pragma once
#include <stdbool.h>
#include <stdio.h>

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
size_t process_inputs(char **argv, Arguments *args);
size_t write_files(const char *path, FILE *file_ptr);
size_t write_numbered_files(const char *path, FILE *file_ptr);
size_t write_file_stream(FILE *stream, const char *source_name);
void parse_long_option(int index, char **argv, Arguments *arg);
void parse_short_option(int index, char **argv, Arguments *arg);
void write_help(void);

#endif
