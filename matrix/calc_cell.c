
#include "common.h"

#define CELL_ID (argv[1])

/*argv:
  [0] name,
  [1] cell_id
*/

int calc(matrix*, int);

int main(int argc, char** argv) {
  key_t key;
  int shm_id;
  int *addr, *addr_origin;
  matrix m[NUM_MATRIX];
  
  int i, retval;
  
  if(argc < 2) {
    fprintf(stderr, "Argument error\n");
    exit(1);
  }
  
  key = ftok(".", PROJ_ID);
  if(key < 0) {
    perror("ftok");
  }
  
  shm_id = shmget(key, 0, 0);
  if(shm_id < 0) {
    perror("shmget");
    exit(1);
  }
  
  addr_origin = shmat(shm_id, NULL, 0);
  if(addr_origin == (void*) -1) {
    perror("shmat");
    exit(1);
  }
  
  addr = (int*) addr_origin;
  for(i = 0; i < NUM_MATRIX; i++) {
    m[i].rows = *addr; addr++;
    m[i].cols = *addr; addr++;
  }
  
  for(i = 0; i < NUM_MATRIX; i++) {
    m[i].body = addr;
    addr += (m[i].cols * m[i].rows);
  }
  
  //вычисление значения ячейки...
  calc(m, atoi(CELL_ID));
  
  retval = shmdt(addr_origin);
  if(retval < 0) {
    perror("shmdt");
    exit(1);
  }
  
  return 0;
}

int calc(matrix *m, int cell_id) {
  int *cell, *i, *j;
  int k;
  
  cell = m[C].body + cell_id;
  *cell = 0;
  
  i = m[A].body + (cell_id/m[C].cols) * m[A].cols;
  j = m[B].body + (cell_id%m[B].cols);
  
  for(k = 0; k < m[A].cols; k++) {
    *cell += (*i) * (*j);
    i += 1;
    j += m[B].cols;
  }
  
  return 0;
}
