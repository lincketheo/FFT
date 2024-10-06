
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// runtime assert
#define panic_if(expr)                                                         \
  do {                                                                         \
    if (expr) {                                                                \
      exit(-1);                                                                \
    }                                                                          \
  } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
  float a;
  float p0;
  float f;
} pargs;

int parse_pargs_str(const char *str, pargs *p) {
  assert(str);
  assert(p);

  p->a = 1.0f;  // Default value for amplitude
  p->p0 = 0.0f; // Default value for phase
  p->f = NAN;

  const char *ptr = str;

  while (*ptr) {
    char *end;
    float value = strtof(ptr, &end);
    if (end == ptr) {
      fprintf(stderr, "Failed to parse pargs argument: %s starting from %s\n",
              str, ptr);
      return -1;
    }

    if (*end == 'f') {
      p->f = value;
      ptr = end + 1;
    } else if (*end == 'a') {
      p->a = value;
      ptr = end + 1;
    } else if (*end == 'p') {
      p->p0 = value;
      ptr = end + 1;
    } else {
      ptr = end;
    }
  }

  if (isnanf(p->f)) {
    fprintf(stderr, "f is required in periodic args string\n");
    return -1;
  }

  return 0;
}

typedef struct {
  float t0;
  float tf;
  int isinf;
} trange;

typedef struct {
  int k0;
  int kf;
  int isinf;
} krange;

int parse_time_range(const char *str, trange *t) {
  t->t0 = 0.0f;
  t->tf = 0.0f;
  t->isinf = 0;

  float t0, tf;

  if (sscanf(str, "%f:%f", &t0, &tf) == 2) {
    t->t0 = t0;
    t->tf = tf;
  } else if (sscanf(str, "%f:", &t0) == 1) {
    t->t0 = t0;
    t->isinf = 1;
  } else if (sscanf(str, ":%f", &tf) == 1) {
    t->t0 = 0.0f;
    t->tf = tf;
  } else if (strcmp(str, ":") == 0) {
    t->isinf = 1; // If no tf provided, consider tf as "infinity"
  } else {
    return -1;
  }
  return 0;
}

int parse_k_range(const char *str, krange *k) {
  k->k0 = 0;
  k->kf = 0;
  k->isinf = 0;

  int k0, kf;

  if (sscanf(str, "%d:%d", &k0, &kf) == 2) {
    k->k0 = k0;
    k->kf = kf;
  } else if (sscanf(str, "%d:", &k0) == 1) {
    k->k0 = k0;
    k->isinf = 1;
  } else if (sscanf(str, ":%d", &kf) == 1) {
    k->k0 = 0;
    k->kf = kf;
  } else if (strcmp(str, ":") == 0) {
    k->isinf = 1;
  } else {
    return -1;
  }
  return 0;
}

#define expect_next_arg(label)                                                 \
  do {                                                                         \
    if (++i >= argc) {                                                         \
      fprintf(stderr, "Required argument after " label "\n");                  \
      goto failed;                                                             \
    }                                                                          \
  } while (0)

int main(int argc, char **argv) {
  trange t = {0};
  krange k = {0};
  int tset = 0;
  int kset = 0;
  float fs_value = NAN;

  pargs *p = malloc(10 * sizeof *p);
  panic_if(p == NULL);
  size_t plen = 0;
  size_t pcap = 10;

  for (int i = 1; i < argc; ++i) {
    float _fs_value;
    if (strcmp(argv[i], "-fs") == 0) {
      expect_next_arg("-fs");

      if (!isnanf(fs_value)) {
        fprintf(stderr,
                "Supplied multiple sample frequencies. Second was: %s\n",
                argv[i]);
        goto failed;
      }

      if (sscanf(argv[i], "%f", &_fs_value) == 1) {
        fs_value = _fs_value;
      } else {
        fprintf(stderr, "Failed to parse float: %s\n", argv[i]);
        goto failed;
      }
    } else if (strcmp(argv[i], "-t") == 0) {
      expect_next_arg("-t");
      if (tset || kset) {
        fprintf(
            stderr,
            "Already supplied a sample or time range. Second passed was: %s\n",
            argv[i]);
        goto failed;
      }
      if (parse_time_range(argv[i], &t)) {
        fprintf(stderr, "Failed to parse time range: %s\n", argv[i]);
        goto failed;
      }
      tset = 1;
    } else if (strcmp(argv[i], "-k") == 0) {
      expect_next_arg("-k");
      if (tset || kset) {
        fprintf(
            stderr,
            "Already supplied a sample or time range. Second passed was: %s\n",
            argv[i]);
        goto failed;
      }
      if (parse_k_range(argv[i], &k)) {
        fprintf(stderr, "Failed to parse sample range: %s\n", argv[i]);
        goto failed;
      }
      kset = 1;
    } else {
      if (plen == pcap) {
        pargs *temp = realloc(p, 2 * pcap * sizeof *temp);
        panic_if(temp == NULL);
        p = temp;
        pcap = 2 * pcap;
      }
      if (parse_pargs_str(argv[i], &p[plen++])) {
        fprintf(stderr, "Failed to parse periodic args: %s\n", argv[i]);
        goto failed;
      }
    }
  }

  if (plen == 0) {
    fprintf(stderr, "Expected at least one periodic argument\n");
    goto failed;
  }
  if (isnanf(fs_value)) {
    fprintf(stderr, "Expected a sample frequency -fs\n");
    goto failed;
  }
  if (!tset && !kset) {
    k = (krange){.k0 = 0, .isinf = 1};
    kset = 1;
  }
  if (tset) {
    k = (krange){
        .k0 = t.t0 * fs_value, .kf = t.tf * fs_value, .isinf = t.isinf};
    kset = 1;
  }

  complex float buffer[2048];

  while (k.isinf || k.k0 < k.kf) {
    size_t tofill = MIN(2048, k.kf - k.k0);
    for (size_t _k = 0; _k < tofill; ++_k) {
      buffer[_k] = 0;
      for (int s = 0; s < plen; ++s) {
        buffer[_k] += p[s].a * cexpf(2 * M_PI * I * p[s].f *
                                         (float)(_k + k.k0) / fs_value +
                                     p[s].p0);
      }
    }
    fwrite(buffer, tofill, sizeof *buffer, stdout);
    k.k0 += tofill;
  }

  return EXIT_SUCCESS;

failed:
  return EXIT_FAILURE;
}
