#include "lista_clientes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAMANHO_BLOCO_INICIAL 4

void lista_clientes_init(ListaClientes* L) {
    L->dados = NULL;
    L->tam = 0;
    L->cap = 0;
}

int lista_clientes_add(ListaClientes* L, Cliente c) {

    if (L->tam == L->cap) {
        size_t nova_capacidade = (L->cap == 0) ? TAMANHO_BLOCO_INICIAL : L->cap * 2;
        Cliente* temp = (Cliente*)realloc(L->dados, nova_capacidade * sizeof(Cliente));

        if (temp == NULL) {
            fprintf(stderr, "Erro: Falha ao realocar memoria para a lista de clientes.\n");
            return 0;
        }
        L->dados = temp;
        L->cap = nova_capacidade;
    }

    L->dados[L->tam] = c;
    L->tam++;
    return 1;
}

void lista_clientes_free(ListaClientes* L) {
    free(L->dados);
    L->dados = NULL;
    L->tam = 0;
    L->cap = 0;
}

int lista_clientes_buscar_por_cpf(const ListaClientes* L, const char* cpf) {
    for (size_t i = 0; i < L->tam; i++) {
        if (strcmp(L->dados[i].cpf, cpf) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int lista_clientes_buscar_por_conta(const ListaClientes* L, const char* conta) {
    for (size_t i = 0; i < L->tam; i++) {
        if (strcmp(L->dados[i].conta, conta) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int quicksort_particao_nome(Cliente v[], int l, int r) {
    Cliente pivot = v[r];
    int i = (l - 1);

    for (int j = l; j <= r - 1; j++) {
        if (strcmp(v[j].nome, pivot.nome) <= 0) {
            i++;
            trocar_clientes(&v[i], &v[j]);
        }
    }
    trocar_clientes(&v[i + 1], &v[r]);
    return (i + 1);
}

void quicksort_clientes_por_nome(Cliente v[], int l, int r) {
    if (l < r) {
        int pi = quicksort_particao_nome(v, l, r);
        quicksort_clientes_por_nome(v, l, pi - 1);
        quicksort_clientes_por_nome(v, pi + 1, r);
    }
}
