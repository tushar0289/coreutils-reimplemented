#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>


typedef struct File_info {
    char *modes;
    unsigned long link;
    char *user;
    char *group;
    long long size;
    char *timebuf;
    const char *name;
} File_info;

typedef struct Column {
    int max_link;
    int max_user;
    int max_group;
    long long max_size;
} Column;

void mode_string(mode_t mode, char *str);
void get_long(const char *dir, const char *name, File_info *info);
void print_long(const char *path, const char *name, File_info *info, Column *col); 
void track_len(const char *path, DIR *dirp, File_info *info, Column *col); 

int show_all = 0;
int show_inodes = 0;
int show_long = 0;


int main(int argc, char *argv[]) {
    int opt;
    File_info info = { 0 };
    Column col = { 0 };

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

    if (show_long) track_len(path, dir_ptr, &info, &col);

    struct dirent *entry;
    while ((entry = readdir(dir_ptr)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') continue;
        if (show_inodes) printf("%lld ", (long long) entry->d_ino);
        if (show_long){
            print_long(path, entry->d_name, &info, &col);
        }
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

void track_len(const char *path, DIR *dirp, File_info *info, Column *col) {
    rewinddir(dirp);
    struct dirent *entry;
    char max_size[50];
    char max_size_link[50];

    while ((entry = readdir(dirp)) != NULL) {
        get_long(path, entry->d_name, info);
        col->max_size = (col->max_size > info->size) ? col->max_size : info->size;
        col->max_user = (col->max_user > strlen(info->user)) ? col->max_user : strlen(info->user);
        col->max_group = (col->max_group > strlen(info->group)) ? col->max_group : strlen(info->group);
        col->max_link = (col->max_link > info->link) ? col->max_link : info->link;
    }
    snprintf(max_size, sizeof(max_size), "%lld", col->max_size);
    col->max_size = strlen(max_size);
    snprintf(max_size_link, sizeof(max_size_link), "%d", col->max_link);
    col->max_link = strlen(max_size_link);

    rewinddir(dirp);
}

void get_long(const char *dir, const char *name, File_info *info) {
    char fullpath[4096];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);

    struct stat st;
    if (lstat(fullpath, &st) < 0) {
        perror(name);
        return;
    }

    struct passwd *pass = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    info->user = pass ? pass->pw_name : "?";
    info->group = gr ? gr->gr_name: "?";

    char modes[11];
    mode_string(st.st_mode, modes);
    info->modes = modes;

    char timebuf[64];
    struct tm *time = localtime(&st.st_mtim.tv_sec);
    strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", time);
    info->timebuf = timebuf;
    
    info->link = st.st_nlink;
    info->size = st.st_size;
    info->name = name;
}

void print_long(const char *path, const char *name, File_info *info, Column *col) {
    get_long(path, name, info);
    printf("size: %lld\n", col->max_size);
    printf("%s %*lu %*s %*s %*lld %s %s\n",
            info->modes, 
            col->max_link,
            info->link,
            col->max_user,
            info->user,
            col->max_group,
            info->group,
            (int) col->max_size,
            info->size,
            info->timebuf,
            info->name
            );
}
