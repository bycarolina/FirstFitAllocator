  # T2 - Gerenciador de Memória em C (FirstFitAllocator)

  ## Descrição
  Este trabalho implementa um alocador de memória personalizado em linguagem C.
  O programa cria um pool de memória contínuo e gerencia as alocações dentro dele usando a estratégia First Fit (primeiro espaço livre disponível).

  O sistema exibe o mapa da memória (blocos usados e livres) e estatísticas como:
  - Total de alocações
  - Memória livre e utilizada
  - Maior bloco livre
  - Número de fragmentos

    ## Estrutura do Projeto
      main.c          # Programa principal
      mymemory.c      # Implementação do alocador
      mymemory.h      # Definição das estruturas e funções
      Makefile        # Script de compilação

  ## Como Compilar e Executar

  1. Compilar:
      make

  2. Executar:
      ./t2

  3. Limpar arquivos gerados:
      make clean
