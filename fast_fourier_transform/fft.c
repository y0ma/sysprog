#include <stdio.h>
#include <malloc.h>
#include <math.h>

#define MY_MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MY_PI (3.14159265358979323846)
#define FORWARD (1)
#define BACKWARD (-1)

struct complex_num {
  double re;
  double im;
};

int round2(int n);
struct complex_num* fft(struct complex_num *a, struct complex_num *y, int n, int direction);
struct complex_num* forward_fft(struct complex_num *a, struct complex_num *y, int n);
struct complex_num* backward_fft(struct complex_num *y, struct complex_num *a, int n);
struct complex_num* complex_add(struct complex_num *a, struct complex_num *b, struct complex_num *res);
struct complex_num* complex_sub(struct complex_num *a, struct complex_num *b, struct complex_num *res);
struct complex_num* complex_mul(struct complex_num *a, struct complex_num *b, struct complex_num *res);

int main(void) {
  int deg[3], deg2;
  struct complex_num *polynomials[3];
  struct complex_num *dft[3]; //Discrete Fourier Transform
  int i, j;

  printf("Введите степень 1-го многочлена:\n");
  scanf("%d", &deg[0]);
  
  printf("Введите степень 2-го многочлена:\n");
  scanf("%d", &deg[1]);

  if(deg[0] <= 0 || deg[1] <= 0) {
    printf("Некорректные данные\n");
    return 1;
  }

  deg[2] = deg[0] + deg[1] - 1;

  deg2 = 2 * round2(MY_MAX2(deg[0], deg[1]));
  
  for(i = 0; i < 3; ++i) {
    polynomials[i] = (struct complex_num*) calloc(deg2, sizeof(struct complex_num));
    dft[i] = (struct complex_num*) calloc(deg2, sizeof(struct complex_num));
  }

  for(i = 0; i < 2; ++i) {
    printf("Введите коэффициенты %d-го многочлена (в порядке возрастания степеней):\n", i+1);
    for(j = 0; j < deg[i]; ++j) {
      scanf("%lf", &polynomials[i][j].re);
    }
  }
 
  for(i = 0; i < 2; ++i) {
    forward_fft(polynomials[i], dft[i], deg2);
  }

  for(i = 0; i < deg2; ++i) {
    complex_mul(&dft[0][i], &dft[1][i], &dft[2][i]);
  }
  
  backward_fft(dft[2], polynomials[2], deg2);

  printf("\nРезультат:\n");
  for(i = 0; i < deg[2]; ++i) {
    printf("(%lf, %lf) ", polynomials[2][i].re, polynomials[2][i].im);
  }
  printf("\n");
  
  for(i = 0; i < 3; ++i) {
    free(polynomials[i]);
    free(dft[i]);
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

struct complex_num* forward_fft(struct complex_num *a, struct complex_num *y, int n) {
  return fft(a, y, n, FORWARD);
}

struct complex_num* backward_fft(struct complex_num *y, struct complex_num *a, int n) {
  int i;
  
  fft(y, a, n, BACKWARD);

  for(i = 0; i < n; ++i) {
    a[i].re /= n;
    a[i].im /= n;
  }

  return a;
}

struct complex_num* fft(struct complex_num *a, struct complex_num *y, int n, int direction) {
  struct complex_num w, wn;
  struct complex_num *_a[2], *_y[2];
  double alpha;
  int i, j, nn;
  
  if(n == 1) {
    *y = *a;
    return y;
  }
  
  nn = n >> 1;

  w = (struct complex_num) {1, 0};
  
  alpha = 2*MY_PI/n;
  wn = (struct complex_num) {cos(alpha), sin(alpha) * direction};
  
  for(i = 0; i < 2; ++i) {
    _a[i] = (struct complex_num*) calloc(nn, sizeof(struct complex_num));
    _y[i] = (struct complex_num*) calloc(nn, sizeof(struct complex_num));
    
    for(j = 0; j < nn; ++j) {
      _a[i][j] = a[2*j + i];
    }
    
    _y[i] = fft(_a[i], _y[i], nn, direction);
  }

  for(i = 0; i < nn; ++i) {
    complex_mul(&w, &_y[1][i], &y[i]);
    
    complex_sub(&_y[0][i], &y[i], &y[i + nn]);
    complex_add(&_y[0][i], &y[i], &y[i]);
    
    complex_mul(&w, &wn, &w);
  }

  for(i = 0; i < 2; ++i) {
    free(_a[i]);
    free(_y[i]);
  }
  
  return y;
}

struct complex_num* complex_add(struct complex_num *a, struct complex_num *b, struct complex_num *res) {
  res->re = a->re + b->re;
  res->im = a->im + b->im;
  
  return res;
}

struct complex_num* complex_sub(struct complex_num *a, struct complex_num *b, struct complex_num *res) {
  res->re = a->re - b->re;
  res->im = a->im - b->im;
  
  return res;
}

struct complex_num* complex_mul(struct complex_num *a, struct complex_num *b, struct complex_num *res) {
  double re;
  double im;
  
  re = a->re*b->re - a->im*b->im;
  im = a->re*b->im + a->im*b->re;

  *res = (struct complex_num) {re, im};
  
  return res;
}
