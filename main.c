  #include <stdio.h>
  #include <stdlib.h>
  #include <time.h>
  #include "mymemory.h"

  static void shuffle(size_t *a, size_t n) {
      for (size_t i = 0; i + 1 < n; i++) {
          size_t j = i + (size_t)(rand() % (int)(n - i));
          size_t t = a[i]; a[i] = a[j]; a[j] = t;
      }
  }

  int main(void) {
      srand((unsigned)time(NULL));

      /* 1) inicia pool */
      size_t POOL = 8000; /* 8 KB */
      mymemory_t *mem = mymemory_init(POOL);
      if (!mem) { fprintf(stderr, "Falha ao inicializar a pool.\n"); return 1; }

      /* 2) gera 50 tamanhos variados */
      size_t base_sizes[] = {10,25,50,100,250,500,1000,200,40,80};
      size_t N = 50;
      size_t sizes[50];
      for (size_t i = 0; i < N; i++) sizes[i] = base_sizes[i % (sizeof(base_sizes)/sizeof(base_sizes[0]))];

      /* 3) embaralha ordem */
      shuffle(sizes, N);

      /* 4) aloca o que couber */
      void *ptrs[50] = {0};
      size_t ok = 0;
      for (size_t i = 0; i < N; i++) {
          void *p = mymemory_malloc(mem, sizes[i]);
          ptrs[i] = p;
          if (p) ok++;
      }

      printf("\nApós alocar (tentativa de %zu blocos; %zu sucederam):\n", N, ok);
      mymemory_display(mem);
      mymemory_stats(mem);

      /* 5) libera metade (índices pares) */
      for (size_t i = 0; i < N; i += 2) {
          if (ptrs[i]) mymemory_free(mem, ptrs[i]);
      }

      printf("\nApós liberar metade (índices pares):\n");
      mymemory_display(mem);
      mymemory_stats(mem);

      /* 6) novas alocações (First Fit) */
      size_t extra_sizes[] = {60,120,240,480,30,90,300};
      void *extras[sizeof(extra_sizes)/sizeof(extra_sizes[0])] = {0};
      printf("\nTentando novas alocações (extras):\n");
      for (size_t i = 0; i < sizeof(extra_sizes)/sizeof(extra_sizes[0]); i++) {
          extras[i] = mymemory_malloc(mem, extra_sizes[i]);
          if (extras[i])
              printf("  extra[%zu] alocado em %p (%zu bytes)\n", i, extras[i], extra_sizes[i]);
          else
              printf("  extra[%zu] falhou (%zu bytes)\n", i, extra_sizes[i]);
      }

      printf("\nApós novas alocações em buracos:\n");
      mymemory_display(mem);
      mymemory_stats(mem);

      /* 7) limpa */
      mymemory_release(mem);
      return 0;
  }