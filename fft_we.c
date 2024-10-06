#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <complex.h>
#include <fftw3.h>

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


  fftwf_plan p;
  p = fftwf_plan_dft_1d(pos, ret, arr, FFTW_FORWARD, FFTW_ESTIMATE);

  clock_gettime(CLOCK_MONOTONIC, &start);
  fftwf_execute(p);
  clock_gettime(CLOCK_MONOTONIC, &end);

  fftwf_destroy_plan(p);
  fftwf_cleanup();

  seconds = end.tv_sec - start.tv_sec;
  nanoseconds = end.tv_nsec - start.tv_nsec;
  elapsed = seconds + nanoseconds * 1e-9; // Convert to seconds

  fprintf(stderr, "Elapsed time: %.9f seconds\n", elapsed);

  fwrite(ret, sizeof *arr, pos, stdout);

  free(arr);
  return 0;
}
