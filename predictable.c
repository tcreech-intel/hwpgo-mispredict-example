#include <stdlib.h>

void nop(void*, int);

#define N 20000
#define ITERS 10000

static int *m_s1, *m_s2, *m_s3, *m_dst;

/* This loop body contains a condition which is predicted well because it's
 * usually false. This case demonstrates why static heuristics do not
 * aggressively speculate in order to remove control flow: the computation of z
 * would be almost never be used, and performance would decrease. PMU feedback
 * in this case will confirm that the branch is well-predicted, and no special
 * attempts at conditional move conversion will be made.
 */
void predictable(int *dst, int *s1, int *s2, int *s3) {
#pragma novector
#pragma nounroll
  for (int i = 0; i < N; i++) {
    int *p;
    if(s1[i] % 2) {
      p = &s2[i] ;
      int z = i * i * i * i * i * i * i;
      nop(p, z);
    } else {
      p =  &s3[i];
      nop(p, 3);
    }
    dst[i] = *p;
  }
}

void init(void) {
  m_s1 = malloc(sizeof(int)*N);
  m_s2 = malloc(sizeof(int)*N);
  m_s3 = malloc(sizeof(int)*N);
  m_dst = malloc(sizeof(int)*N);

  for (int i = 0; i < N; i++) {
    m_s1[i] = (i*i*i*i*i*i*i) % N;
    m_s2[i] = 0;
    m_s3[i] = 1;
  }
}

int main(void) {
  init();
  for(int i=0; i<ITERS; ++i)
#pragma noinline
    predictable(m_dst, m_s1, m_s2, m_s3);
  return 0;
}
