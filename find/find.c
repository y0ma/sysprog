#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

void check_args(int argc, char **argv);
void file_tree_walk(const char *dirpath, const char *substr);

int main(int argc, char **argv) {
  check_args(argc, argv);
  
  file_tree_walk(argv[1], argv[2]);
  
  return 0;
}

void check_args(int argc, char **argv) {
  if(argc < 3) {
    fprintf(stderr, "Argument error\n");
    exit(1);
  }
}

void file_tree_walk(const char *dirpath, const char *substr) {
  DIR *dp;
  int retval;
  struct dirent *entry;
  struct stat statbuf;
  char *fullpath;

  fullpath = NULL;
  
  dp = opendir(dirpath);
  if(dp == NULL) {
    perror("opendir");
    return;
  }
  
  while((entry = readdir(dp)) != NULL) {
    fullpath = (char*) malloc((strlen(dirpath) + strlen(entry->d_name) + 2) * sizeof(char));
    
    sprintf(fullpath, "%s/%s", dirpath, entry->d_name);

    retval = lstat(fullpath, &statbuf);
    
    if(retval < 0) {
      perror("lstat");
      free(fullpath);
      continue;
    }

    if(strstr(entry->d_name, substr)) {
      printf("%s\n", fullpath);
    }
    
    if(S_ISDIR(statbuf.st_mode) && strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".")) {
      file_tree_walk(fullpath, substr);
    }
    
    free(fullpath);
  }
  
  retval = closedir(dp);
  if(retval < 0) {
    perror("closedir");
    exit(1);
  }
}
