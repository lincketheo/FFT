
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

uint32_t reverse(uint32_t num, unsigned int bit_count) {
  uint32_t reverse = 0;
  for (uint32_t i = 0; i < bit_count; i++) {
    if (num & (1 << i)) {
        reverse |= (1 << (bit_count - 1 - i));
    }
  }
  return reverse;
}

void fft(complex float *data, uint32_t N) {
  assert(N > 0);
  if(N == 1)
    return;

  uint32_t l2N = log2f(N); // Number of bits to reverse

  // Reverse bits
  for(uint32_t i = 0; i < N; ++i) {
    uint32_t ir = reverse(i, l2N);

    // Avoid double swap
    if(i < ir) {
      complex float temp = data[i];
      data[i] = data[ir];
      data[ir] = temp;
    }
  }

  for(int s = 1; s <= l2N; ++s) {
    uint32_t m = 1 << s;    
    complex float wm = cexpf(-2 * M_PI * I / m);
    for(int k = 0; k < N; k += m) {
      complex float w = 1;
      for(int j = 0; j < m/2; ++j) {
        complex float odd = w * data[k + j + m/2];
        complex float even = data[k + j];
        data[k + j] = even + odd;
        data[k + j + m/2] = even - odd;
        w = w * wm;
      }
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "required file\n");
    return -1;
  }
  FILE *ifp = fopen(argv[1], "rb");
  fseek(ifp, 0, SEEK_END);
  long pos = ftell(ifp) / sizeof(complex float);
  fseek(ifp, 0, SEEK_SET);

  complex float *arr = malloc(pos * sizeof *arr);

  fread(arr, sizeof *arr, pos, ifp);

  struct timespec start, end;
  long seconds, nanoseconds;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  fft(arr, pos);

  clock_gettime(CLOCK_MONOTONIC, &end);

  seconds = end.tv_sec - start.tv_sec;
  nanoseconds = end.tv_nsec - start.tv_nsec;
  elapsed = seconds + nanoseconds * 1e-9; // Convert to seconds

  fprintf(stderr, "Elapsed time: %.9f seconds\n", elapsed);

  fwrite(arr, sizeof *arr, pos, stdout);

  free(arr);
  return 0;
}
