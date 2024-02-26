#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

void formatAsTree(int level, int isFile, char *fileName)
{
    int i;
    for (i = 0; i < level + 1; i++)
        printf("│  ");
    if (isFile)
        printf("|---");
    printf("%s\n", fileName);
}

void listDirectoriesAndFiles(const char* directoryName, int indentlevel) {
    DIR* directory = opendir(directoryName);
    struct dirent* current_directory;

    if (directory == NULL) {
        perror("Directory doesn\'t exist.\n");
        return;
    }
    current_directory = readdir(directory);

    while (current_directory != NULL) {
        if(current_directory->d_name[0] != '.' && strcmp(current_directory->d_name, ".") != 0 && strcmp(current_directory->d_name, "..") != 0){
              formatAsTree(indentlevel-1, 1, current_directory->d_name);
        }
        if (current_directory->d_type == DT_DIR && strcmp(current_directory->d_name, ".") != 0 && strcmp(current_directory->d_name, "..") != 0) {
            char current_path[100] = { 0 };
            strcat(current_path, directoryName);
            strcat(current_path, "/");
            strcat(current_path, current_directory->d_name);
            listDirectoriesAndFiles(current_path, indentlevel+1);
        }
        current_directory = readdir(directory);
    }

    closedir(directory);
}

int main(int argc, char* argv[]) {
    if (argv[1] == NULL) {
        argv[1] = ".";
    }
    printf("%s\n", argv[1]);
    listDirectoriesAndFiles(argv[1], 0);
    return 0;
}