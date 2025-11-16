#ifndef CLIENTE_H
#define CLIENTE_H

#include <stddef.h>

typedef struct {
    char agencia[8];
    char conta[16];
    char nome[200];
    char cpf[14];
    char data_nasc[14];
    char telefone[22];
    char endereco[120];
    char cep[12];
    char local[60];
    char numero_casa[6];
    char bairro[60];
    char cidade[60];
    char estado[4];
    char senha[20];
    double saldo;
    int ativo;
} Cliente;

Cliente novo_cliente_padrao(void);

void imprimir_dados_cliente(const Cliente* c);

int nome_menor(const Cliente* a, const Cliente* b);

void trocar_clientes(Cliente* a, Cliente* b);

#endif