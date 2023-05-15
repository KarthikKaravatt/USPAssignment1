#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
typedef struct Result {
  char id[100];
  int result;
} Result;
int listNumberOfFiles(char *directoryName) {
  struct dirent *dp;
  int files = 0;
  DIR *dirp = opendir(directoryName);
  dp = readdir(dirp);
  // looks at each file in directory
  while (dp != NULL) {

    // stores pointer to the last occurrence of .
    // this is the file extension part of the file
    char *foundFile = strrchr(dp->d_name, '.');
    // compares file extension to .usp
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
      // increment if found
      files++;
    }
    // get next file
    dp = readdir(dirp);
  }
  closedir(dirp);
  return files;
}
Result evaluate_file(char *filename) {
  // open file in read only mode
  int file1 = open(filename, O_RDONLY);
  // file buffer
  char buffer[1000];
  // read file into buffer
  int fileRead = read(file1, buffer, sizeof(buffer));
  // close file
  close(file1);
  // split file contents using new line character
  char *token = strtok(buffer, "\n");
  Result res;
  // first line of the file is the id of file
  // copy this string to the id section in the result struct
  // make sure there is room for the \0
  strncpy(res.id, token, 100 - 1);
  res.id[100 - 1] = '\0';
  // get next line and add it to the first operand
  token = strtok(NULL, "\n");
  int operand1 = atoi(token);
  // get next line and add it to the operator
  token = strtok(NULL, "\n");
  char operator= token[0];
  // get next line and add it to the second operator
  token = strtok(NULL, "\n");
  int operand2 = atoi(token);
  int result;
  // evaluate result
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
  res.result = result;
  return res;
}
void writeResultTofile(Result res) {
  // open file
  int file2 = open("results", O_WRONLY | O_CREAT | O_APPEND, 0777);
  // results buffer
  char result_str[200];
  sprintf(result_str, "%s: %d\n", res.id, res.result);
  // print to file
  write(file2, result_str, strlen(result_str));
  close(file2);
}
char **listFiles(int numberOfFiles, char *directoryName) {
  // make array to store files
  char **files = malloc(sizeof(char *) * numberOfFiles);
  struct dirent *dp;
  // open directory
  DIR *dirp = opendir(directoryName);
  int counter = 0;
  // resd next file
  dp = readdir(dirp);
  // add all usp files to array
  while (dp != NULL) {

    char *foundFile = strrchr(dp->d_name, '.');
    if (foundFile && strcmp(foundFile, ".usp") == 0) {
      // dp name has a max of 256 characters
      files[counter] = malloc(sizeof(char) * 256);
      // copy file name to array
      strcpy(files[counter], dp->d_name);
      counter++;
    }
    // read next file
    dp = readdir(dirp);
  }
  closedir(dirp);
  return files;
}
int main(void) {
  // Find the number of usp files
  int numberOfFiles = listNumberOfFiles("./");
  // store file names in a array
  char **files = listFiles(numberOfFiles, "./");
  // create file descriptors, 2 for each file (read and write)
  int fd[2 * numberOfFiles];
  // pipe for each file
  int id;
  for (int i = 0; i < numberOfFiles; i++) {
    // calcualtes index of file descriptor
    // and gives address of it
    if (pipe(fd + 2 * i) == -1) {
      printf("Error occured with pipe");
    }
  }
  // create a process to evaluate each file
  for (int i = 0; i < numberOfFiles; i++) {
    id = fork();
    if (id == -1) {
      printf("Error ocurred while forking");
    }
    if (id == 0) {
      // close read pipe because not using it
      close(fd[2 * i]);
      Result result = evaluate_file(files[i]);
      // accessing the write fd by incrementing by one
      write(fd[2 * i + 1], &result, sizeof(Result));
      close(fd[2 * i + 1]);
      // child process stop execution after file is evaluated
      exit(0);
    }
  }
  // only parent executes this part
  Result result;
  // writes results from each file descriptor to file
  for (int i = 0; i < numberOfFiles; i++) {
    // close write pipe because not using it
    close(fd[2 * i + 1]);
    read(fd[2 * i], &result, sizeof(Result));
    writeResultTofile(result);
    // close read pipe
    close(fd[2 * i]);
  }
  // wait for child process to die
  wait(&id);
  // memory clean up
  for (int i = 0; i < numberOfFiles; i++) {
    free(files[i]);
  }
  free(files);
  return 0;
}
