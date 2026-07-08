#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int show_all = 0;
int show_inodes = 0;

int main(int argc, char *argv[]) {
    int opt;

    while((opt = getopt(argc, argv, "ai")) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'i':
                show_inodes = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] [path]\n", argv[0]);
                return 1;
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    DIR *dir_ptr = opendir(path);
    if (!dir_ptr) {
        perror(path);
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir_ptr)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') continue;
        if(show_inodes) printf("%lld ", (long long) entry->d_ino);
        printf("%s\n", entry->d_name);
    }

    closedir(dir_ptr);
    return 0;
}
