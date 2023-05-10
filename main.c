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
void printFile(char *filename) {
  char buf;
  int file = openFile(filename, O_RDWR | O_RDONLY);
  while (read(file, &buf, 1)) {
    printf("%s", &buf);
  }
  close(file);
}
char **listFiles(char *directoryName) {
  struct dirent *dp;
  int numberOfFiles = 0;
  DIR *dirp = opendir(directoryName);
  dp = readdir(dirp);
  while (dp != NULL) {

    char *foundFile = strrchr(dp->d_name, '.');
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
      numberOfFiles++;
    }
    dp = readdir(dirp);
  }
  closedir(dirp);
  char **files;
  files = malloc(numberOfFiles * sizeof(char *));
  int counter = 0;
  dirp = opendir(directoryName);
  dp = readdir(dirp);
  while (dp != NULL) {

    char *foundFile = strrchr(dp->d_name, '.');
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
      files[counter] = malloc(strlen(dp->d_name) + 1 * sizeof(char));
      files[counter] = dp->d_name;
      counter++;
    }
    dp = readdir(dirp);
  }
  closedir(dirp);
  return files;
}
int main(void) {
    char** files = listFiles("./");
    for(int i = 0; i <4; i++){
        printf("%s\n", files[i]);
    }
    for(int i = 0; i <4; i++){
        free(files[i]);
    }
    free(files);
  return 0;
}
