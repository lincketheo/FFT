
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


complex float* fft(const complex float *src, size_t N, size_t stride) {
  complex float* ret = malloc(N * sizeof* ret);
  if(N == 1) {
    ret[0] = src[0];
    return ret;
  }

  complex float* even = fft(src, N/2, stride * 2);
  complex float* odd = fft(&src[stride], N/2, stride * 2);

  for(int k = 0; k < N / 2; ++k) {
    complex float p = even[k];
    complex float q = cexpf(-2 * M_PI * I / N * k) * odd[k];
    ret[k] = p + q;
    ret[k + N/2] = p - q;
  }

  free(even);
  free(odd);

  return ret;
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

  ret = fft(arr, pos, 1);

  clock_gettime(CLOCK_MONOTONIC, &end);

  seconds = end.tv_sec - start.tv_sec;
  nanoseconds = end.tv_nsec - start.tv_nsec;
  elapsed = seconds + nanoseconds * 1e-9; // Convert to seconds

  fprintf(stderr, "Elapsed time: %.9f seconds\n", elapsed);

  fwrite(ret, sizeof *arr, pos, stdout);

  free(arr);
  return 0;
}
