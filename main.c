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
  char buffer[1000];
  // read file into buffer
  int fileRead = read(file1, buffer, sizeof(buffer));
  close(file1);
  // split file contents using new line character
  char *line = strtok(buffer, "\n");
  Result res;
  // first line of the file is the id of file
  // copy this string to the id section in the result struct
  // make sure there is room for the \0
  strncpy(res.id, line, 100 - 1);
  res.id[100 - 1] = '\0';
  // get next line and add it to the first operand
  line = strtok(NULL, "\n");
  int operand1 = atoi(line);
  // get next line and add it to the operator
  line = strtok(NULL, "\n");
  char operator= line[0];
  // get next line and add it to the second operator
  line = strtok(NULL, "\n");
  int operand2 = atoi(line);
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
  char result[200];
  sprintf(result, "%s: %d\n", res.id, res.result);
  // print to file
  write(file2, result, strlen(result));
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
  // two pipes for each file
  // one for resutls and another for fileName
  // 2d array having 0 for read end and 1 for write end
  int fileNamePipe[numberOfFiles][2];
  int resultPipe[numberOfFiles][2];
  // pipe for each file
  int id;
  int fileNum = 0;
  for (int i = 0; i < numberOfFiles; i++) {
    // create a 2 pipes for each file
    if (pipe(fileNamePipe[i]) == -1) {
      printf("Error occured with pipe");
    }
    if (pipe(resultPipe[i]) == -1) {
      printf("Error occured with pipe");
    }
  }
  // creating child processes for each file
  for (int i = 0; i < numberOfFiles; i++) {
    id = fork();
    if (id == -1) {
      printf("Error ocurred while forking");
    }
    // stops child from creating more processes
    if (id == 0) {
      fileNum = i;
      break;
    }
    // only parent exexcutes this part 
    // close read pipe as it is not used 
    close(fileNamePipe[i][0]);
    // file name written to file name pipe
    write(fileNamePipe[i][1], files[i], strlen(files[i]));
    close(fileNamePipe[i][1]);
  }
  if (id == 0) {
    // close write pipe because not using it
    close(fileNamePipe[fileNum][1]);
    // must initalize or else a valgrind error 
    char file[500] = " ";
    read(fileNamePipe[fileNum][0], file, sizeof(file));
    close(fileNamePipe[fileNum][0]);
    // close read pipe because not using it
    close(resultPipe[fileNum][0]);
    Result result = evaluate_file(file);
    write(resultPipe[fileNum][1], &result, sizeof(Result));
    close(resultPipe[fileNum][1]);
    // child process stop execution after file is evaluated
  }
  // only parent executes this part
  else {
    Result result;
    // writes results from each file descriptor to file
    for (int i = 0; i < numberOfFiles; i++) {
      // close write pipe because not using it 
      close(resultPipe[i][1]);
      read(resultPipe[i][0], &result, sizeof(Result));
      writeResultTofile(result);
      // close read pipe
      close(resultPipe[i][0]);
    }
    // wait for all child process to die
    for (int i = 0; i < numberOfFiles; i++) {
      wait(NULL);
    }
  }
  // memory clean up
  for (int i = 0; i < numberOfFiles; i++) {
    free(files[i]);
  }
  free(files);
  return 0;
}
