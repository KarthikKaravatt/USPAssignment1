#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
int openFile(char *fileName, int flags) {
  int fileDiscriptor;
  char fileError[] = "Error: file could not be read\n";
  fileDiscriptor = open(fileName, flags);
  if (fileDiscriptor == -1) {
    printf("%s", fileError);
  }
  return fileDiscriptor;
}

int listNumberOfFiles(char *directoryName) {
  struct dirent *dp;
  int files = 0;
  DIR *dirp = opendir(directoryName);
  dp = readdir(dirp);
  while (dp != NULL) {

    char *foundFile = strrchr(dp->d_name, '.');
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
      files++;
    }
    dp = readdir(dirp);
  }
  closedir(dirp);
  return files;
}

char **listFiles(int numberOfFiles, char *directoryName) {
  char **files = malloc(sizeof(char *) * numberOfFiles);
  struct dirent *dp;
  DIR *dirp = opendir(directoryName);
  int counter = 0;
  dp = readdir(dirp);
  while (dp != NULL) {

    char *foundFile = strrchr(dp->d_name, '.');
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
        files[counter] = malloc(sizeof(char)*256);
        strcpy(files[counter], dp->d_name);
        counter++;
    }
    dp = readdir(dirp);
  }
  closedir(dirp);
  return files;
}
int main(void) {
    int numberOfFiles = listNumberOfFiles("./"); 
    char** files = listFiles(numberOfFiles , "./");
    for (int i = 0; i < numberOfFiles; i++) {
        printf("%s\n", files[i]);
    }
    for (int i = 0; i < numberOfFiles; i++) {
        free(files[i]);
    }
    free(files);
}
