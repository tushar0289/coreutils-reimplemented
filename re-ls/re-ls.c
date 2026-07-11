#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>


void mode_string(mode_t mode, char *str);
void print_long(const char *dir, const char *name);

int show_all = 0;
int show_inodes = 0;
int show_long = 0;


int main(int argc, char *argv[]) {
    int opt;

    while((opt = getopt(argc, argv, "ali")) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'i':
                show_inodes = 1;
                break;
            case 'l':
                show_long = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-ali] [path]\n", argv[0]);
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
        if (show_inodes) printf("%lld ", (long long) entry->d_ino);
        if (show_long)
            print_long(path, entry->d_name);
        else
            printf("%s ", entry->d_name);

        if (show_inodes && !show_long) putchar(' ');
    }

    if (!show_long) putchar('\n');

    closedir(dir_ptr);
    return 0;
}

void mode_string(mode_t mode, char *str) {
    if (S_ISDIR(mode))       str[0] = 'd';
    else if (S_ISBLK(mode))  str[0] = 'b';
    else if (S_ISCHR(mode))  str[0] = 'c';
    else if (S_ISLNK(mode))  str[0] = 'l';
    else if (S_ISSOCK(mode)) str[0] = 's';
    else if (S_ISFIFO(mode)) str[0] = 'p';
    else                     str[0] = '-';

    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

void print_long(const char *dir, const char *name) {
    char fullpath[4096];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);

    struct stat st;
    if (lstat(fullpath, &st) < 0) {
        perror(name);
        return;
    }

    struct passwd *pass = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    char *user = pass ? pass->pw_name : "?";
    char *group = gr ? gr->gr_name: "?";

    char modes[11];
    mode_string(st.st_mode, modes);

    char timebuf[64];
    struct tm *time = localtime(&st.st_mtim.tv_sec);
    strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", time);

    printf("%s %lu %s %s %lld %s %s\n",
            modes, 
            (unsigned long) st.st_nlink,
            user,
            group,
            (long long) st.st_size,
            timebuf,
            name
            );
}
