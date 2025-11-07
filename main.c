#include <stdio.h>
#include <stdlib.h>
#include "mymemory.h"

static void mostrar_estado(mymemory_t *mem, const char *etapa) {
    printf("\n-- %s --\n", etapa);
    mymemory_display(mem);
    mymemory_stats(mem);
}

static void titulo(const char *texto) {
    printf("\n%s\n", texto);
}

int main(void) {
    mymemory_t *mem = NULL;

    titulo("INICIALIZACAO");
    mem = mymemory_init(1024);
    if (!mem) { fprintf(stderr, "Falha ao criar pool de 1024 bytes.\n"); return 1; }
    mostrar_estado(mem, "Pool criada (vazia)");

    titulo("CENARIO 1");
    mymemory_release(mem);
    mem = mymemory_init(1024);

    void *a = mymemory_malloc(mem, 512);
    void *b = mymemory_malloc(mem, 512);
    printf("Alocadas duas areas de 512 bytes: %p e %p\n", a, b);
    mostrar_estado(mem, "Após alocar 2 blocos de 512B");

    mymemory_free(mem, a);
    mostrar_estado(mem, "Após liberar o primeiro bloco");

    void *blocos128[4];
    for (int i = 0; i < 4; i++) blocos128[i] = mymemory_malloc(mem, 128);
    printf("Alocados quatro blocos de 128 bytes.\n");
    mostrar_estado(mem, "Após alocar 4 blocos de 128B");

    for (int i = 0; i < 4; i++) {
        if (blocos128[i]) mymemory_free(mem, blocos128[i]);
    }
    mostrar_estado(mem, "Após liberar os 4 blocos de 128B (limpeza do Cenario 1)");

    titulo("CENARIO 2");
    mymemory_release(mem);
    mem = mymemory_init(1024);

    void *c2[8];
    for (int i = 0; i < 8; i++) c2[i] = mymemory_malloc(mem, 128);
    printf("Criados 8 blocos de 128 bytes.\n");
    mostrar_estado(mem, "Após preencher com 8 blocos");

    for (int i = 2; i <= 5; i++) mymemory_free(mem, c2[i]);
    printf("Liberados os blocos centrais (3, 4, 5 e 6).\n");
    mostrar_estado(mem, "Após liberar blocos centrais");

    void *novo512 = mymemory_malloc(mem, 512);
    printf("Nova alocacao de 512 bytes em %p\n", novo512);
    mostrar_estado(mem, "Após nova alocacao de 512B");

    titulo("CENARIO 3");
    mymemory_release(mem);
    mem = mymemory_init(1024);

    void *c3[64];
    for (int i = 0; i < 64; i++) c3[i] = mymemory_malloc(mem, 16);
    printf("Preenchido com 64 blocos de 16 bytes.\n");
    mostrar_estado(mem, "Após preencher 64x16");

    for (int i = 0; i < 64; i += 2) mymemory_free(mem, c3[i]);
    printf("Liberados blocos alternados.\n");
    mostrar_estado(mem, "Após liberar blocos alternados");

    void *extra = mymemory_malloc(mem, 32);
    printf("Nova alocacao de 32 bytes em %p\n", extra);
    mostrar_estado(mem, "Após alocar 32B");

    titulo("FINAL");
    mostrar_estado(mem, "Estado final antes da limpeza");

    mymemory_release(mem);
    return 0;
}
