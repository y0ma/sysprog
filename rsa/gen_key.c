#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void print_help(int exval);

int main(int argc, char **argv) {
  int opt;

  while((opt = getopt(argc, argv, "p:s:h")) != -1) {
    switch(opt) {
    case 'h':
      print_help(0);
      break;
    case 'p':
      break;
    case 's':
      break;
    case ':':
      fprintf(stderr, "Error - Option `%c' needs a value\n\n", optopt);
      print_help(1);
      break;
    case '?':
      fprintf(stderr, "Error - No such option: `%c'\n\n", optopt);
      print_help(1);
    }
  }
  
  return 0;
}

void print_help(int exval) {
  printf("  -p FILE    file for public key\n\n");
  printf("  -s FILE    file for private key\n\n");
  exit(exval);
}
