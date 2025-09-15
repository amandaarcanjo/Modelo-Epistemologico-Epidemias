#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// se quiser repetir o resultado, descomente e escolha uma semente fixa:
// #define SEED 12345

// rede aleatoria erdos-renyi, mas eu vou guardar só a lista de vizinhos (fica leve e rapido)
#define N 5000     // número de indivíduos
#define T 1000     // tempo total
#define kmed 5     // grau medio da rede aleatória, dado kmed vc acha o p
#define p ((float)kmed / N) // chance de conexão da rede aleatoria

// epidemiologia + fofoca (info)
#define Pinfect 0.2f       // probabilidade de infecção (base)
#define Pdie 0.01f         // chance de morrer (removido)
#define Ptell 0.8f         // chance de contar como se proteger da doença
#define Prevention 0.3f    // fator de prevenção baseado da pessoa que tem informação (multiplica Pinfect)

// parametro do "salto quântico" / tunelamento: infecta espontaneamente sem ter contato
#define Ptunelar 0.01f     // se não quiser isso, bota 0.0f

// numero aleatorio [0,1)
#define rn ((float)rand() / RAND_MAX)

// eu vou fazer um "vetor dinamico" bem simples pra lista de adjacencia (sem firula)
typedef struct {
  int *v;       // vizinhos
  int tam;      // quantos tem
  int cap;      // capacidade
} lista;

// cria lista vazia
void cria_lista(lista *L) {
  L->v = NULL;
  L->tam = 0;
  L->cap = 0;
}

// empurra valor pro final
void empurra(lista *L, int x) {
  if (L->tam == L->cap) {
    int nova = (L->cap == 0 ? 4 : L->cap * 2);
    int *tmp = (int*)realloc(L->v, nova * sizeof(int));
    if (!tmp) { fprintf(stderr, "acabou a memória (viz)\n"); exit(1); }
    L->v = tmp;
    L->cap = nova;
  }
  L->v[L->tam++] = x;
}

int main(void) {
  // estados e info (eu uso unsigned char pra economizar memória msm)
  // state[i]=0 -> saudável, 1 -> infectado, 2 -> removido
  // know[i]=1 -> sabe a informação da dodoi, 0 -> não sabe
  unsigned char *state = (unsigned char*)calloc(N, sizeof(unsigned char));
  unsigned char *know  = (unsigned char*)calloc(N, sizeof(unsigned char));
  if (!state || !know) { fprintf(stderr, "sem memória (state/know)\n"); return 1; }

  // rede: lista de adjacencia
  lista *viz = (lista*)malloc(N * sizeof(lista));
  if (!viz) { fprintf(stderr, "sem memória (viz)\n"); return 1; }
  for (int i = 0; i < N; i++) cria_lista(&viz[i]);

  // semente aleatoria
  #ifdef SEED
    srand((unsigned)SEED);
  #else
    srand((unsigned)time(NULL));
  #endif

  // constroi rede erdos-renyi G(N,p) (não direcionada)
  for (int i = 0; i < N; i++) {
    for (int j = i + 1; j < N; j++) {
      if (rn < p) {
        empurra(&viz[i], j);
        empurra(&viz[j], i);
      }
    }
  }

  // inicial
  for (int i = 0; i < N; i++) {
    state[i] = 0; // ninguem ta dodoi
    know[i]  = 0; // ninguem sabe de nada
  }
  state[0] = 1;   // paciente zero
  know[1]  = 1;   // indivíduo informado

  // arquivo de saída
  char txt[320];
  snprintf(txt, sizeof(txt),
           "Epidemia-N%d-Pinfect%.3f-Ptell%.3f-Pdie%.3f-Prevention%.3f-Ptunelar%.3f.csv",
           N, Pinfect, Ptell, Pdie, Prevention, Ptunelar);
  FILE *fp = fopen(txt, "w");
  if (!fp) { fprintf(stderr, "não abriu arquivo\n"); return 1; }

  // cabecalho bonitinho (ajuda no excel/origin)
  fprintf(fp, "t,S,I,R\n");

  // buffers p/ atualizar sem bagunca no mesmo passo
  unsigned char *novo_state = (unsigned char*)malloc(N * sizeof(unsigned char));
  unsigned char *novo_know  = (unsigned char*)malloc(N * sizeof(unsigned char));
  if (!novo_state || !novo_know) { fprintf(stderr, "sem memória (buff)\n"); return 1; }

  for (int t = 0; t < T; t++) {
    // copia base
    for (int i = 0; i < N; i++) {
      novo_state[i] = state[i];
      novo_know[i]  = know[i];
    }

    // 1) fofoca: quem sabe tenta ensinar os vizinhos
    for (int i = 0; i < N; i++) {
      if (know[i] == 1) {
        for (int k = 0; k < viz[i].tam; k++) {
          int j = viz[i].v[k];
          if (novo_know[j] == 0 && rn < Ptell) {
            novo_know[j] = 1; // propagação de informação de qualidade
          }
        }
      }
    }

    // 2) contágio vizinho-a-vizinho (com prevenção se know==1)
    for (int i = 0; i < N; i++) {
      if (state[i] == 1) { // i infectado passa para vizinho saudável
        for (int k = 0; k < viz[i].tam; k++) {
          int j = viz[i].v[k];
          if (state[j] == 0) {
            float pinf = (know[j] ? (Prevention * Pinfect) : Pinfect);
            if (rn < pinf) {
              novo_state[j] = 1;
            }
          }
        }
      }
    }

    // 3) morte (remoção)
    for (int i = 0; i < N; i++) {
      if (state[i] == 1) {
        if (rn < Pdie) {
          novo_state[i] = 2; // foi de base
        }
      }
    }

    // 4) salto quântico (tunelamento): infecta espontaneamente
    for (int i = 0; i < N; i++) {
      if (state[i] == 0) {
        if (rn < Ptunelar) {
          novo_state[i] = 1;
        }
      }
    }

    // aplica atualizacao
    for (int i = 0; i < N; i++) {
      state[i] = novo_state[i];
      know[i]  = novo_know[i];
    }

    // contagens
    int s_count = 0, i_count = 0, r_count = 0;
    for (int i = 0; i < N; i++) {
      if (state[i] == 0) s_count++;
      else if (state[i] == 1) i_count++;
      else if (state[i] == 2) r_count++;
    }

    // escreve (t,S,I,R) no CSV
    fprintf(fp, "%d,%d,%d,%d\n", t, s_count, i_count, r_count);

    // stop cedo se não tem mais infectado (economiza)
    if (i_count == 0) break;
  }

  fclose(fp);

  // limpeza
  for (int i = 0; i < N; i++) free(viz[i].v);
  free(viz);
  free(state);
  free(know);
  free(novo_state);
  free(novo_know);

  return 0;
}