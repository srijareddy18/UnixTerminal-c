#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

time_t current_time, past_time, hourly_time;
struct tm time_modifier;
int modified_files = 0;

void listDirectoriesAndFiles(char* directoryName) {
    DIR* directory = opendir(directoryName);
    struct dirent* current_directory;
    struct stat file_info;
    int with_in_hour;
    time_t file_last_modified_time;
    if (directory == NULL) {
        if(errno == ENOENT)
            printf("File doesn\'t exist..\n");
        else if(errno == ENOTDIR)
            printf("Not a directory..\n");
        else
            printf("Please run the program with a directory.\n");
        exit(0);
    }

    while ((current_directory=readdir(directory))!= NULL) {
        char current_path[1024] = { 0 };
        strcpy(current_path, directoryName);
        strcat(current_path, "/");
        strcat(current_path, current_directory->d_name);
        stat(current_path, &file_info);
        if(strcmp(current_directory->d_name, ".") != 0 && strcmp(current_directory->d_name, "..") != 0){
            file_last_modified_time = file_info.st_mtime;
            with_in_hour = difftime(file_last_modified_time, hourly_time);
            if (S_ISREG(file_info.st_mode) && with_in_hour <= 3600 && with_in_hour >= 0) {
                modified_files++;
            }
             else if (S_ISDIR(file_info.st_mode)) {
                listDirectoriesAndFiles(current_path);
            }
        }

    }
    closedir(directory);
}

int main(int argc, char* argv[]) {

    int i = 1;
    if (argv[1] == NULL) {
        argv[1] = ".";
    }

    char *temp_hourly_time;
    current_time = time(NULL);
    time_modifier = *localtime(&current_time);
    time_modifier.tm_mday--;
    past_time = mktime(&time_modifier);
    hourly_time = past_time;
    do {
        listDirectoriesAndFiles(argv[1]);
        temp_hourly_time = ctime(&hourly_time);
        if (temp_hourly_time[strlen(temp_hourly_time)-1] == '\n') 
            temp_hourly_time[strlen(temp_hourly_time)-1] = '\0';
        printf("%s : %d\n", temp_hourly_time, modified_files);
        time_modifier.tm_hour++;
        hourly_time = mktime(&time_modifier);
        modified_files = 0;
    } while((i++) < 24);

    return 0;
}