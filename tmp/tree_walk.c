#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
  printf("%-3s %2d %7lld   %-40s %d %s\n",
    (tflag == FTW_D) ? "d" : (tflag == FTW_DNR) ? "dnr" :
    (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
    (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_SL) ?  "sl" :
    (tflag == FTW_SLN) ? "sln" : "???",
    ftwbuf->level, (long long) sb->st_size,
    fpath, ftwbuf->base, fpath + ftwbuf->base);

  return 0;
}

int main(int argc, char **argv ) {
  int flags = 0;

  if(argc > 2 && strchr(argv[2], 'd') != NULL) {
    flags |= FTW_DEPTH;
  }

  if(argc > 2 && strchr(argv[2], 'p') != NULL) {
    flags |= FTW_PHYS;
  }

  nftw((argc < 2) ? "." : argv[1], display_info, 20, flags);

  exit(EXIT_SUCCESS);
}
