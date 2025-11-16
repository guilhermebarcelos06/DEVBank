#ifndef LISTA_CLIENTES_H
#define LISTA_CLIENTES_H

#include "cliente.h"
#include <stddef.h>

typedef struct {
    Cliente* dados;
    size_t tam;
    size_t cap;
} ListaClientes;

void lista_clientes_init(ListaClientes* L);

int lista_clientes_add(ListaClientes* L, Cliente c);

void lista_clientes_free(ListaClientes* L);

int lista_clientes_buscar_por_cpf(const ListaClientes* L, const char* cpf);

int lista_clientes_buscar_por_conta(const ListaClientes* L, const char* conta);

void quicksort_clientes_por_nome(Cliente v[], int l, int r);

int quicksort_particao_nome(Cliente v[], int l, int r);

#endif