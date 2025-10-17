#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>

#define PATH_MAX 4096

int parse_flags(char* flag_string, int* recursive_flag, int* long_format_flag, int* group_format_flag) 
{
    while (*++flag_string)
    {
        switch (*flag_string)
        {
            case 'R': *recursive_flag = 1; break;
            case 'l': *long_format_flag = 1; break;
            case 'g': *group_format_flag = 1; break;
            default: return 0;
        }
    }
    return 1;
}

int print_file_info(char* file_name, int long_format_flag, int group_format_flag, char* directory_name)
{
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        return 1;

    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", directory_name, file_name);

    struct stat file_info;
    if (lstat(full_path, &file_info) == 0)
    {
        if (long_format_flag)
        {
            switch (file_info.st_mode & S_IFMT)
            {
                case S_IFDIR:  putchar('d'); break;
                case S_IFCHR:  putchar('c'); break;
                case S_IFBLK:  putchar('b'); break;
                case S_IFREG:  putchar('-'); break;
                case S_IFLNK:  putchar('l'); break;
                case S_IFSOCK: putchar('s'); break;
            }

            char permissions[] = "rwxrwxrwx";
            char file_permissions[10];
            file_permissions[9] = '\0';
            for (int i = 0, j = 1 << 8; i < 9; i++, j >>= 1)
                file_permissions[i] = (file_info.st_mode & j) ? permissions[i] : '-';

            printf("%s ", file_permissions);    

            struct passwd* user_info = getpwuid(file_info.st_uid); 
            printf("%s ", user_info ? user_info->pw_name : "unknown");

            if (group_format_flag)
            {
                struct group* group_info = getgrgid(file_info.st_gid);
                printf("%s ", group_info ? group_info->gr_name : "unknown");
            }
            printf("%lld ", file_info.st_size);
        }
        else if (group_format_flag)
        {
            struct group* group_info = getgrgid(file_info.st_gid);
            printf("%s ", group_info ? group_info->gr_name : "unknown");
        }

        printf("%s", file_name);

        if (S_ISLNK(file_info.st_mode))
        {
            char link_target[PATH_MAX + 1];
            int link_length = readlink(full_path, link_target, sizeof(link_target));
            link_target[link_length] = '\0';
            printf(" -> %s", link_target);
        }

        printf("\n");
    }

    return 0;
}

int process_directory(char* directory_name, int recursive_flag, int long_format_flag, int group_format_flag)
{
    DIR* dir = opendir(directory_name);
    if (dir == NULL)
    {
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
        return 1;
    }

    struct dirent* directory_entry;
    if (recursive_flag)
    {
        struct stat file_info;
        printf("\n%s:\n", directory_name);
        while ((directory_entry = readdir(dir)) != NULL)
        {
            print_file_info(directory_entry->d_name, long_format_flag, group_format_flag, directory_name);
            
            if (strcmp(directory_entry->d_name, ".") == 0 || strcmp(directory_entry->d_name, "..") == 0)
                continue;

            char full_path[PATH_MAX];
            snprintf(full_path, PATH_MAX, "%s/%s", directory_name, directory_entry->d_name);
            if (lstat(full_path, &file_info) == 0 && S_ISDIR(file_info.st_mode))
            {
                if (strcmp(directory_entry->d_name, ".git") != 0)
                    process_directory(full_path, recursive_flag, long_format_flag, group_format_flag);
            }
        }
    }
    else
    {
        while ((directory_entry = readdir(dir)) != NULL)
            print_file_info(directory_entry->d_name, long_format_flag, group_format_flag, directory_name);
    }

    closedir(dir);
    return 0;
}

int main(int argc, char* argv[])
{
    char current_directory[PATH_MAX]; 
    getcwd(current_directory, PATH_MAX);

    int recursive_flag = 0, long_format_flag = 0, group_format_flag = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            parse_flags(argv[i], &recursive_flag, &long_format_flag, &group_format_flag);
    }

    process_directory(current_directory, recursive_flag, long_format_flag, group_format_flag);
    return 0;
}

