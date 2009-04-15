#include <stdio.h>
#include <malloc.h>

#define MY_MAX2(a, b) ((a) > (b) ? (a) : (b))

int round2(int n);

int main(void) {
  int deg[2], deg2;
  int *polynomials[3];
  int dft[3];
  int i, j;

  scanf("%d %d", deg, deg + 1);
  deg2 = 2 * round2(MY_MAX2(deg[0], deg[1]));
  
  for(i = 0; i < 3; ++i) {
    polynomials[i] = calloc(deg2 * sizeof(int));
    dft[i] = calloc(deg2 * sizeof(int));
  }

  for(i = 0; i < 2; ++i) {
    for(j = 0; j < deg[i]; ++j) {
      scanf("%d", &polynomials[i][j]);
    }
  }

  for(i = 0; i < 2; ++i) {
    fft(polynomials[i], dft[i], deg2);
  }

  for(i = 0; i < 3; ++i) {
    free(polynomials[i]);
  }
  
  return 0;
}

int round2(int n) {
  int shift, step;

  if(!n) {
    return 1;
  }

  if(n < 3) {
    return n;
  }

  shift = step = sizeof(int) * 4;
  
  while(step) {
    if((n >> shift) == 1) {
      break;
    }

    step >>= 1;
    shift += (n >> shift) ? step : -step;
  }
  
  if(~(-1 << shift) & n) {
    shift++;
  }
  
  return (1 << shift);
}
