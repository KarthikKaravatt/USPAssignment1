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
void printFile(char *filename) {
  char buf;
  int file = openFile(filename, O_RDWR | O_RDONLY);
  int counter = 0;
  // reads the file one character at a time
  while (read(file, &buf, 1)) {
    printf("%c", buf);
    counter++;
  }
  close(file);
}
void evaluate_file(char *filename) {
  int file1 = open(filename, O_RDONLY);
  char buffer[1000];
  int fileRead = read(file1, buffer, sizeof(buffer));
  close(file1);
  char *token = strtok(buffer, "\n");
  token = strtok(NULL, "\n");
  int operand1 = atoi(token);
  token = strtok(NULL, "\n");
  char operator= token[0];
  token = strtok(NULL, "\n");
  int operand2 = atoi(token);
  int result;
  switch (operator) {
  case '+':
    result = operand1 + operand2;
    break;
  case '-':
    result = operand1 - operand2;
    break;
  case '*':
    result = operand1 * operand2;
    break;
  case '/':
    result = operand1 / operand2;
    break;
  }
  int file2 = open("results", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (result < 0) {
    write(file2, "-", 1);
    result = -result;
  }
  int num_digits = 0;
  int temp_result = result;
  do {
    num_digits++;
    temp_result /= 10;
  } while (temp_result > 0);

  char result_str[num_digits];
  for (int i = num_digits - 1; i >= 0; i--) {
    result_str[i] = '0' + (result % 10);
    result /= 10;
  }
  write(file2, result_str, num_digits);
  write(file2, "\n", 1);
  close(file2);
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
      // dp name has a max of 256 characters
      files[counter] = malloc(sizeof(char) * 256);
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
  char **files = listFiles(numberOfFiles, "./");
  for (int i = 0; i < numberOfFiles; i++) {
    evaluate_file(files[i]);
  }
  for (int i = 0; i < numberOfFiles; i++) {
    free(files[i]);
  }
  free(files);
}
