
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void dft(complex float *dest, complex float *src, size_t slen) {
  for (int k = 0; k < slen; ++k) {
    complex float val = 0;
    for (int n = 0; n < slen; ++n) {
      val += src[n] * cexpf(-I * 2 * M_PI / (float)slen * k * n);
    }
    dest[k] = val;
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
  complex float *ret = malloc(pos * sizeof *arr);

  fread(arr, sizeof *arr, pos, ifp);

  struct timespec start, end;
  long seconds, nanoseconds;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  dft(ret, arr, pos);

  clock_gettime(CLOCK_MONOTONIC, &end);

  seconds = end.tv_sec - start.tv_sec;
  nanoseconds = end.tv_nsec - start.tv_nsec;
  elapsed = seconds + nanoseconds * 1e-9; // Convert to seconds

  fprintf(stderr, "Elapsed time: %.9f seconds\n", elapsed);
  fwrite(ret, sizeof *arr, pos, stdout);

  free(arr);
  return 0;
}
