#include "cliente.h"
#include <stdio.h>
#include <string.h>

Cliente novo_cliente_padrao(void) {
    Cliente c = {0};
    strcpy(c.agencia, "0001");
    strcpy(c.estado, "DF");
    c.saldo = 0.0;
    c.ativo = 1;
    return c;
}

void imprimir_dados_cliente(const Cliente* c) {
    printf("--- Dados do Cliente ---\n");
    printf("Agencia: %s\n", c->agencia);
    printf("Conta: %s\n", c->conta);
    printf("Nome: %s\n", c->nome);
    printf("CPF: %s\n", c->cpf);
    printf("Data Nasc.: %s\n", c->data_nasc);
    printf("Telefone: %s\n", c->telefone);
    printf("Endereco: %s, %s - %s, %s - %s/%s\n", c->endereco, c->numero_casa, c->bairro, c->local, c->cidade, c->estado);
    printf("CEP: %s\n", c->cep);
    printf("Saldo: %.2f\n", c->saldo);
    printf("Status: %s\n", c->ativo ? "Ativa" : "Encerrada");
    printf("------------------------\n");
}

int nome_menor(const Cliente* a, const Cliente* b) {
    return strcmp(a->nome, b->nome);
}

void trocar_clientes(Cliente* a, Cliente* b) {
    Cliente temp = *a;
    *a = *b;
    *b = temp;
}
