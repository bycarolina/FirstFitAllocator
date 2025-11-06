#include "mymemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*Ajusta tamanho ao alinhamento*/
static size_t align_size(size_t n) {
    const size_t A = sizeof(max_align_t);
    return (n + (A - 1)) & ~(A - 1);
}

/*Calcula deslocamento de um ponteiro*/
static size_t ptr_offset(mymemory_t *m, void *p) {
    return (size_t)((uintptr_t)p - (uintptr_t)m->pool);
}

/*Converte offset em ponteiro*/
static void* pool_at(mymemory_t *m, size_t off) {
    return (void *)((uintptr_t)m->pool + (uintptr_t)off);
}

/*Insere nó em ordem de endereço*/
static void insert_sorted(mymemory_t *m, allocation_t *node) {
    if (!m->head || m->head->start > node->start) {
        node->next = m->head;
        m->head = node;
        return;
    }
    allocation_t *cur = m->head;
    while (cur->next && cur->next->start < node->start)
        cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

/*Remove nó pelo ponteiro*/
static int remove_node(mymemory_t *m, void *ptr) {
    allocation_t *prev = NULL, *cur = m->head;
    while (cur) {
        if (cur->start == ptr) {
            if (prev) prev->next = cur->next;
            else m->head = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

/*Inicializa a pool*/
mymemory_t* mymemory_init(size_t size) {
    mymemory_t *memory = (mymemory_t*)malloc(sizeof *memory);
    if (!memory) return NULL;
    memset(memory, 0, sizeof *memory);

    size = align_size(size);
    memory->pool = malloc(size);
    if (!memory->pool) {
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL;
    return memory;
}

/*Aloca bloco (First Fit)*/
void* mymemory_alloc(mymemory_t *m, size_t size) {
    if (!m || !m->pool || size == 0) return NULL;
    size = align_size(size);

    /* 1) Lista vazia, início do pool */
    if (!m->head) {
        if (m->total_size < size) return NULL;
        allocation_t *node = malloc(sizeof *node);
        if (!node) return NULL;
        node->start = m->pool;
        node->size  = size;
        node->next  = NULL;
        m->head = node;
        return node->start;
    }

    /* 2) Antes do primeiro bloco */
    allocation_t *cur = m->head;
    size_t first_off = ptr_offset(m, cur->start);
    if (first_off >= size) {
        allocation_t *node = malloc(sizeof *node);
        if (!node) return NULL;
        node->start = m->pool;
        node->size  = size;
        node->next  = NULL;
        insert_sorted(m, node);
        return node->start;
    }

    /* 3) Entre blocos */
    while (cur) {
        size_t cur_end = ptr_offset(m, cur->start) + cur->size;
        allocation_t *nxt = cur->next;
        size_t nxt_off = nxt ? ptr_offset(m, nxt->start) : m->total_size;

        if (nxt_off >= cur_end + size) {
            allocation_t *node = malloc(sizeof *node);
            if (!node) return NULL;
            node->start = pool_at(m, cur_end);
            node->size  = size;
            node->next  = NULL;
            insert_sorted(m, node);
            return node->start;
        }
        cur = nxt;
    }

    /* 4) Sem espaço */
    return NULL;
}

/*Libera bloco*/
void mymemory_free(mymemory_t *m, void *ptr) {
    if (!m || !m->pool || !ptr) return;
    remove_node(m, ptr);
}

/*Exibe mapa do pool*/
void mymemory_display(mymemory_t *m) {
    if (!m || !m->pool) return;
    printf("== MYMEMORY MAPA ==\n");
    printf("pool @ %p | total: %zu bytes\n", m->pool, m->total_size);

    allocation_t *cur = m->head;
    if (!cur) {
        printf("[0 .. %zu) LIVRE\n", m->total_size);
        return;
    }

    size_t cursor = 0;
    while (cur) {
        size_t off = ptr_offset(m, cur->start);
        if (off > cursor)
            printf("[%zu .. %zu) LIVRE\n", cursor, off);
        printf("[%zu .. %zu) USO (%zu bytes)\n",
               off, off + cur->size, cur->size);
        cursor = off + cur->size;
        cur = cur->next;
    }

    if (cursor < m->total_size)
        printf("[%zu .. %zu) LIVRE\n", cursor, m->total_size);
}

/*Mostra estatísticas*/
void mymemory_stats(mymemory_t *m) {
    if (!m || !m->pool) return;

    size_t total_allocs = 0, total_alloc_bytes = 0;
    size_t free_fragments = 0, largest_free_block = 0;

    if (!m->head) {
        free_fragments = (m->total_size > 0) ? 1 : 0;
        largest_free_block = m->total_size;
    } else {
        allocation_t *cur = m->head;
        size_t gap = ptr_offset(m, cur->start);
        if (gap > 0) { free_fragments++; if (gap > largest_free_block) largest_free_block = gap; }

        while (cur) {
            total_allocs++;
            total_alloc_bytes += cur->size;

            allocation_t *nxt = cur->next;
            size_t cur_end = ptr_offset(m, cur->start) + cur->size;

            if (nxt) {
                size_t nxt_off = ptr_offset(m, nxt->start);
                size_t g = (nxt_off > cur_end) ? (nxt_off - cur_end) : 0;
                if (g > 0) { free_fragments++; if (g > largest_free_block) largest_free_block = g; }
            } else {
                size_t g = (m->total_size > cur_end) ? (m->total_size - cur_end) : 0;
                if (g > 0) { free_fragments++; if (g > largest_free_block) largest_free_block = g; }
            }
            cur = nxt;
        }
    }

    size_t total_free = m->total_size - total_alloc_bytes;

    printf("== MYMEMORY STATS ==\n");
    printf("Total de alocações: %zu\n", total_allocs);
    printf("Memória alocada:    %zu bytes\n", total_alloc_bytes);
    printf("Memória livre:      %zu bytes\n", total_free);
    printf("Maior bloco livre:  %zu bytes\n", largest_free_block);
    printf("Fragmentos livres:  %zu\n", free_fragments);
}

/*Limpa a pool*/
void mymemory_cleanup(mymemory_t *m) {
    if (!m) return;
    allocation_t *cur = m->head;
    while (cur) { allocation_t *nxt = cur->next; free(cur); cur = nxt; }
    if (m->pool) free(m->pool);
    free(m);
}
