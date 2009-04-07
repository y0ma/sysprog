#include <stdlib.h>
#include <stdio.h>

#define A (0)
#define B (1)
#define C (2)

#define P (2)

#define CH (0)
#define CW (2)

static int mul_cnt;

int round2(int n);
int max3(int *arg);

int** new_sq_matrix(int size);
void free_matrix(int **p);
void read_matrix(int **p, int h, int w);
void print_matrix(int **p, int h, int w);

void shtrassen_mul(int **m1, int **m2, int **res, int n);

void get_subm(int **source, int **dest, int d_size, int x, int y);
void put_subm(int **part, int **dest, int p_size, int x, int y);

void cpy_matrix(int **source, int **dest, int size);
void sum_matrix(int **a, int **b, int **dest, int size);
void sub_matrix(int **a, int **b, int **sest, int size);

int main(void) {
  int **matrix[3];
  int sizes[3];
  int n;
  int i;

  printf("A[n:m], B[m:p]\n\n");
  printf("input [n, m, p]: ");
  
  for(i = 0; i < 3; ++i) {
    scanf("%d", sizes + i);
    
    if(sizes[i] <= 0) {
      printf("Incorrect data.\n");
      return 1;
    }
  }
  
  n = round2(max3(sizes));

  for(i = 0; i < 3; ++i) {
    matrix[i] = new_sq_matrix(n);
  }
  
  for(i = 0; i < 2; ++i) {
    read_matrix(matrix[i], sizes[i], sizes[i + 1]);
  }

  shtrassen_mul(matrix[A], matrix[B], matrix[C], n);
  
  printf("\n");
  print_matrix(matrix[C], CH[sizes], CW[sizes]);
  printf("MUL: %d\n", mul_cnt);
  
  for(i = 0; i < 3; ++i) {
    free_matrix(matrix[i]);
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

int max3(int *arg) {
  return ((arg[0] > arg[1]) ? ((arg[0] > arg[2]) ? arg[0] : arg[2]) : ((arg[1] > arg[2]) ? arg[1] : arg[2]));
}

int** new_sq_matrix(int size) {
  int **p, *tmp;
  int i;

  p = (int**) calloc(size, sizeof(int*));
  tmp = (int*) calloc(size*size, sizeof(int));

  for(i = 0; i < size; ++i) {
    p[i] = tmp;
    tmp += size;
  }
  
  return p;
}

void free_matrix(int **p) {
  free(*p);
  free(p);
}

void read_matrix(int **p, int h, int w) {
  int i, j;
  
  for(i = 0; i < h; ++i) {
    for(j = 0; j < w; ++j) {
      scanf("%d", &p[i][j]);
    }
  }
}

void print_matrix(int **p, int h, int w) {
  int i, j;
  
  for(i = 0; i < h; ++i) {
    for(j = 0; j < w; ++j) {
      printf("%d ", p[i][j]);
    }
    printf("\n");
  }
}

void shtrassen_mul(int **a, int **b, int **c, int n) {
  int **subm[4][3]; // a .. d, e .. h, r .. u
  int **_m[7][3]; // A1 .. A7, B1 .. B7, P1 .. P7
  int i, j;
  int nn;

  if(n == 1) {
    (**c) = (**a) * (**b);
    mul_cnt++;
    return;
  }

  nn = n >> 1;

  for(i = 0; i < 4; ++i) {
    subm[i][A] = new_sq_matrix(nn);
    subm[i][B] = new_sq_matrix(nn);
  }

  for(i = 0; i < 2; ++i) {
    for(j = 0; j < 2; ++j) {
      get_subm(a, subm[2*i + j][A], nn, i, j);
      get_subm(b, subm[i + 2*j][B], nn, i, j);
    }
  }

  for(i = 0; i < 7; ++i) {
    _m[i][A] = new_sq_matrix(nn);
    _m[i][B] = new_sq_matrix(nn);
    _m[i][P] = new_sq_matrix(nn);
  }

  cpy_matrix(subm[0][A], _m[0][A], nn);
  sub_matrix(subm[2][B], subm[3][B], _m[0][B], nn);

  sum_matrix(subm[0][A], subm[1][A], _m[1][A], nn);
  cpy_matrix(subm[3][B], _m[1][B], nn);

  sum_matrix(subm[2][A], subm[3][A], _m[2][A], nn);
  cpy_matrix(subm[0][B], _m[2][B], nn);

  cpy_matrix(subm[3][A], _m[3][A], nn);
  sub_matrix(subm[1][B], subm[0][B], _m[3][B], nn);

  sum_matrix(subm[0][A], subm[3][A], _m[4][A], nn);
  sum_matrix(subm[0][B], subm[3][B], _m[4][B], nn);

  sub_matrix(subm[1][A], subm[3][A], _m[5][A], nn);
  sum_matrix(subm[1][B], subm[3][B], _m[5][B], nn);
  
  sub_matrix(subm[0][A], subm[2][A], _m[6][A], nn);
  sum_matrix(subm[0][B], subm[2][B], _m[6][B], nn);

  for(i = 0; i < 4; ++i) {
    free_matrix(subm[i][A]);
    free_matrix(subm[i][B]);
  }

  for(i = 0; i < 7; ++i) {
    shtrassen_mul(_m[i][A], _m[i][B], _m[i][P], nn);
    free_matrix(_m[i][A]);
    free_matrix(_m[i][B]);
  }

  for(i = 0; i < 4; ++i) {
    subm[i][C] = new_sq_matrix(nn);
  }

  sum_matrix(_m[4][P], _m[3][P], subm[0][C], nn);
  sub_matrix(subm[0][C], _m[1][P], subm[0][C], nn);
  sum_matrix(subm[0][C], _m[5][P], subm[0][C], nn);

  sum_matrix(_m[2][P], _m[3][P], subm[1][C], nn);
  
  sum_matrix(_m[0][P], _m[1][P], subm[2][C], nn);

  sum_matrix(_m[4][P], _m[0][P], subm[3][C], nn);
  sub_matrix(subm[3][C], _m[2][P], subm[3][C], nn);
  sub_matrix(subm[3][C], _m[6][P], subm[3][C], nn);

  for(i = 0; i < 7; ++i) {
    free_matrix(_m[i][P]);
  }

  for(i = 0; i < 2; ++i) {
    for(j = 0; j < 2; ++j) {
      put_subm(subm[i + 2*j][C], c, nn, i, j);
    }
  }
  
  for(i = 0; i < 4; ++i) {
    free_matrix(subm[i][C]);
  }
}

void get_subm(int **source, int **dest, int d_size, int x, int y) {
  int i, j;

  for(i = 0; i < d_size; ++i) {
    for(j = 0; j < d_size; ++j) {
      dest[i][j] = source[x*d_size + i][y*d_size + j];
    }
  }
}

void put_subm(int **part, int **dest, int p_size, int x, int y) {
  int i, j;

  for(i = 0; i < p_size; ++i) {
    for(j = 0; j < p_size; ++j) {
      dest[x*p_size + i][y*p_size + j] = part[i][j];
    }
  }
}

void cpy_matrix(int **source, int **dest, int size) {
  int i, j;

  for(i = 0; i < size; ++i) {
    for(j = 0; j < size; ++j) {
      dest[i][j] = source[i][j];
    }
  }
}

void sum_matrix(int **a, int **b, int **dest, int size) {
  int i, j;

  for(i = 0; i < size; ++i) {
    for(j = 0; j < size; ++j) {
      dest[i][j] = a[i][j] + b[i][j];
    }
  }
}

void sub_matrix(int **a, int **b, int **dest, int size) {
  int i, j;

  for(i = 0; i < size; ++i) {
    for(j = 0; j < size; ++j) {
      dest[i][j] = a[i][j] - b[i][j];
    }
  }
}
