#ifndef BANCO_H
#define BANCO_H

#include "lista_clientes.h"

typedef struct {
    ListaClientes clientes;
    char arquivo_clientes[256];
    char arquivo_movimentos[256];
} Banco;

void banco_init(Banco* b, const char* arq_clientes, const char* arq_movimentos);

int banco_carregar(Banco* b);

int banco_salvar(Banco* b);

void banco_free(Banco* b);

int banco_abrir_conta(Banco* b, Cliente novo_cliente);
int banco_encerrar_conta(Banco* b, const char* conta);
int banco_depositar(Banco* b, const char* conta, double valor);
int banco_sacar(Banco* b, const char* conta, double valor);

int banco_alterar_dados_cliente(Banco* b, int cliente_idx, int campo, const char* novo_valor);

void banco_exibir_extrato(const Banco* b, const char* conta);

int banco_validar_senha(const Banco* b, int cliente_idx, const char* senha_digitada);

void banco_listar_clientes(const Banco* b);
void banco_listar_clientes_ordenado_por_nome(Banco* b);

int salvar_clientes(const char* arq, const ListaClientes* L);

int registrar_movimento(const char* arq_movimentos, const char* conta, const char* tipo, double valor, double saldo_apos);

void gerar_numero_conta(char destino[], int novo_numero);

#endif