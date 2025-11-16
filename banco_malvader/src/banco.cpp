#include "banco.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static void safe_strncpy(char* dest, const char* src, size_t n) {
    if (n == 0) return;
    strncpy(dest, src, n - 1);
    dest[n - 1] = '\0';
}

int carregar_clientes(const char* arq, ListaClientes* L);

void banco_init(Banco* b, const char* arq_clientes, const char* arq_movimentos) {
    lista_clientes_init(&b->clientes);
    strcpy(b->arquivo_clientes, arq_clientes);
    strcpy(b->arquivo_movimentos, arq_movimentos);
    printf("DEVBank inicializado com arquivos: %s e %s\n", arq_clientes, arq_movimentos);
}

int banco_carregar(Banco* b) {
    printf("Carregando clientes de %s...\n", b->arquivo_clientes);
    if (!carregar_clientes(b->arquivo_clientes, &b->clientes)) {
        return 0;
    }
    return 1;
}

int banco_salvar(Banco* b) {
    printf("Salvando clientes em %s...\n", b->arquivo_clientes);
    if (!salvar_clientes(b->arquivo_clientes, &b->clientes)) {
        fprintf(stderr, "Erro ao salvar clientes.\n");
        return 0;
    }
    printf("Clientes salvos.\n");
    return 1;
}


void banco_free(Banco* b) {
    lista_clientes_free(&b->clientes);
    printf("Recursos do DEVBank liberados.\n");
}

// Abertura de conta
int banco_abrir_conta(Banco* b, Cliente novo_cliente) {
    // Verificar se o CPF já existe
    if (lista_clientes_buscar_por_cpf(&b->clientes, novo_cliente.cpf) != -1) {
        fprintf(stderr, "Erro: Cliente com CPF %s ja existe.\n", novo_cliente.cpf);
        return 0;
    }
    // Verificar se a conta já existe
    if (lista_clientes_buscar_por_conta(&b->clientes, novo_cliente.conta) != -1) {
        fprintf(stderr, "Erro: Numero da conta %s ja esta em uso.\n", novo_cliente.conta);
        return 0;
    }

    if (!lista_clientes_add(&b->clientes, novo_cliente)) {
        fprintf(stderr, "Erro: Falha ao adicionar novo cliente a lista.\n");
        return 0;
    }
    printf("Conta aberta com sucesso para %s.\n", novo_cliente.nome);
    return 1;
}

int banco_encerrar_conta(Banco* b, const char* conta) {
    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        return 0;
    }

    if (b->clientes.dados[idx].saldo != 0.0) {
        fprintf(stderr, "Erro: Nao e possível encerrar conta com saldo diferente de zero (%.2f).\n", b->clientes.dados[idx].saldo);
        return 0;
    }

    b->clientes.dados[idx].ativo = 0;
    printf("Conta %s de %s encerrada com sucesso.\n", conta, b->clientes.dados[idx].nome);
    return 1;
}

int banco_depositar(Banco* b, const char* conta, double valor) {
    if (valor <= 0) {
        fprintf(stderr, "Erro: Valor de deposito deve ser positivo.\n");
        return 0;
    }

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) return 0;

    b->clientes.dados[idx].saldo += valor;
    printf("Deposito de %.2f realizado na conta %s. Novo saldo: %.2f\n", valor, conta, b->clientes.dados[idx].saldo);
    registrar_movimento(b->arquivo_movimentos, conta, "DEPOSITO", valor, b->clientes.dados[idx].saldo);
    return 1;
}

int banco_sacar(Banco* b, const char* conta, double valor) {
    if (valor <= 0) {
        fprintf(stderr, "Erro: Valor de saque deve ser positivo.\n");
        return 0;
    }

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
     if (idx == -1) return 0;

    if (b->clientes.dados[idx].saldo < valor) {
        fprintf(stderr, "Erro: Saldo insuficiente na conta %s. Saldo atual: %.2f\n", conta, b->clientes.dados[idx].saldo);
        return 0;
    }

    b->clientes.dados[idx].saldo -= valor;
    printf("Saque de %.2f realizado da conta %s. Novo saldo: %.2f\n", valor, conta, b->clientes.dados[idx].saldo);
    registrar_movimento(b->arquivo_movimentos, conta, "SAQUE", valor, b->clientes.dados[idx].saldo);
    return 1;
}

int banco_validar_senha(const Banco* b, int cliente_idx, const char* senha_digitada) {
    if (cliente_idx < 0 || (size_t)cliente_idx >= b->clientes.tam) {
        return 0;
    }

    if (strcmp(b->clientes.dados[cliente_idx].senha, senha_digitada) == 0) {
        return 1;
    }

    return 0;
}

int banco_alterar_dados_cliente(Banco* b, int cliente_idx, int campo, const char* novo_valor) {
    if (cliente_idx < 0 || (size_t)cliente_idx >= b->clientes.tam) {
        return 0;
    }

    Cliente* c = &b->clientes.dados[cliente_idx];

    switch(campo) {
        case 1:
            safe_strncpy(c->telefone, novo_valor, sizeof(c->telefone));
            printf("Telefone alterado com sucesso.\n");
            break;
        case 2:
            printf("Digite o novo Endereço (Rua, Numero): ");
            ler_linha(c->endereco, sizeof(c->endereco));
            printf("Digite o novo Numero da Casa: ");
            ler_linha(c->numero_casa, sizeof(c->numero_casa));
            printf("Digite o novo Bairro: ");
            ler_linha(c->bairro, sizeof(c->bairro));
            printf("Digite o novo CEP: ");
            ler_linha(c->cep, sizeof(c->cep));
            printf("Digite o novo Local (Ponto de referência): ");
            ler_linha(c->local, sizeof(c->local));
            printf("Digite a nova Cidade: ");
            ler_linha(c->cidade, sizeof(c->cidade));
            printf("Digite o novo Estado (UF): ");
            ler_linha(c->estado, sizeof(c->estado));
            printf("Endereço alterado com sucesso.\n");
            break;
        case 3:
            safe_strncpy(c->senha, novo_valor, sizeof(c->senha));
            printf("Senha alterada com sucesso.\n");
            break;
        default:
            return 0;
    }
    return 1;
}

void banco_exibir_extrato(const Banco* b, const char* conta) {
    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        fprintf(stderr, "Erro: Conta %s nao encontrada.\n", conta);
        return;
    }
    const Cliente* c = &b->clientes.dados[idx];

    printf("\n--- Extrato da Conta ---\n");
    printf("Cliente: %s\n", c->nome);
    printf("Agência: %s\n", c->agencia);
    printf("Conta: %s\n", c->conta);
    printf("------------------------\n");
    printf("Data;Tipo;Valor;Saldo\n");
    printf("------------------------\n");

    FILE* f = fopen(b->arquivo_movimentos, "r");
    if (!f) {
        printf("Nenhum movimento registrado.\n");
        printf("------------------------\n");
        printf("Saldo Atual: %.2f\n", c->saldo);
        printf("------------------------\n");
        return;
    }

    char linha[256];
    int movimentos_encontrados = 0;

    char data[11];
    char conta_mov[16];
    char tipo_mov[20];
    double valor_mov;
    char saldo_desc[50];

    while (fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\n")] = 0;
        if (linha[0] == '\0') continue;

        if (sscanf(linha, "%10[^;];%15[^;];%19[^;];%lf;%49[^\n]",
                   data, conta_mov, tipo_mov, &valor_mov, saldo_desc) == 5)
        {

            if (strcmp(conta_mov, conta) == 0) {

                char* comma = strchr(saldo_desc, ',');
                if(comma) *comma = '.';

                printf("%s;%s;%.2f;%s\n", data, tipo_mov, valor_mov, saldo_desc);
                movimentos_encontrados++;
            }
        }
    }
    fclose(f);

    if (movimentos_encontrados == 0) {
        printf("Nenhum movimento encontrado para esta conta.\n");
    }

    printf("------------------------\n");
    printf("Saldo Atual: %.2f\n", c->saldo);
    printf("------------------------\n");
}

void banco_listar_clientes(const Banco* b) {
    printf("\n--- Lista de Clientes ---\n");
    if (b->clientes.tam == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    for (size_t i = 0; i < b->clientes.tam; i++) {
        imprimir_dados_cliente(&b->clientes.dados[i]);
    }
    printf("------------------------\n");
}

void banco_listar_clientes_ordenado_por_nome(Banco* b) {
    if (b->clientes.tam == 0) {
        printf("Nenhum cliente cadastrado para ordenar.\n");
        return;
    }

    Cliente* copia_clientes = (Cliente*)malloc(b->clientes.tam * sizeof(Cliente));
    if (copia_clientes == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para cópia de clientes.\n");
        return;
    }
    memcpy(copia_clientes, b->clientes.dados, b->clientes.tam * sizeof(Cliente));

    quicksort_clientes_por_nome(copia_clientes, 0, (int)b->clientes.tam - 1);

    printf("\n--- Lista de Clientes (Ordenada por Nome) ---\n");
    for (size_t i = 0; i < b->clientes.tam; i++) {
        imprimir_dados_cliente(&copia_clientes[i]);
    }
    printf("---------------------------------------------\n");

    free(copia_clientes);
}

int salvar_clientes(const char* arq, const ListaClientes* L) {
    FILE* f = fopen(arq, "w");
    if (!f) {
        perror("Erro ao abrir arquivo de clientes para escrita");
        return 0;
    }

    for (size_t i = 0; i < L->tam; i++) {
        const Cliente* c = &L->dados[i];
        fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%.2f;%d\n",
                c->agencia, c->conta, c->nome, c->cpf, c->data_nasc, c->telefone,
                c->endereco, c->cep, c->local, c->numero_casa, c->bairro,
                c->cidade, c->estado, c->senha, c->saldo, c->ativo);
    }

    fclose(f);
    return 1;
}

int carregar_clientes(const char* arq, ListaClientes* L) {
    FILE* f = fopen(arq, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de clientes para leitura (pode nao existir ainda)");
        return 1;
    }

    char linha[512];
    while (fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\n")] = 0;
        if (linha[0] == '\0') continue;

        Cliente c;
        memset(&c, 0, sizeof(Cliente));

        char* p = linha;
        int campos_lidos = 0;

        for (int i = 0; i < 16; i++) {
            char buffer_campo[200] = {0};
            char* inicio_campo = p;

            while (*p != ';' && *p != '\0') {
                p++;
            }

            size_t len = p - inicio_campo;
            if (len > 199) len = 199;
            strncpy(buffer_campo, inicio_campo, len);
            buffer_campo[len] = '\0';

            campos_lidos++;

            switch (i) {
                case 0: safe_strncpy(c.agencia, buffer_campo, sizeof(c.agencia)); break;
                case 1: safe_strncpy(c.conta, buffer_campo, sizeof(c.conta)); break;
                case 2: safe_strncpy(c.nome, buffer_campo, sizeof(c.nome)); break;
                case 3: safe_strncpy(c.cpf, buffer_campo, sizeof(c.cpf)); break;
                case 4: safe_strncpy(c.data_nasc, buffer_campo, sizeof(c.data_nasc)); break;
                case 5: safe_strncpy(c.telefone, buffer_campo, sizeof(c.telefone)); break;
                case 6: safe_strncpy(c.endereco, buffer_campo, sizeof(c.endereco)); break;
                case 7: safe_strncpy(c.cep, buffer_campo, sizeof(c.cep)); break;
                case 8: safe_strncpy(c.local, buffer_campo, sizeof(c.local)); break;
                case 9: safe_strncpy(c.numero_casa, buffer_campo, sizeof(c.numero_casa)); break;
                case 10: safe_strncpy(c.bairro, buffer_campo, sizeof(c.bairro)); break;
                case 11: safe_strncpy(c.cidade, buffer_campo, sizeof(c.cidade)); break;
                case 12: safe_strncpy(c.estado, buffer_campo, sizeof(c.estado)); break;
                case 13: safe_strncpy(c.senha, buffer_campo, sizeof(c.senha)); break;
                case 14: {
                    char* comma = strchr(buffer_campo, ',');
                    if (comma) *comma = '.';
                    c.saldo = atof(buffer_campo);
                    break;
                }
                case 15: c.ativo = atoi(buffer_campo); break;
            }

            if (*p == '\0') {
                break;
            }

            p++;
        }

        if (campos_lidos == 16) {
            if (!lista_clientes_add(L, c)) {
                fprintf(stderr, "Erro ao adicionar cliente carregado à lista.\n");
                fclose(f);
                return 0;
            }
        } else if (campos_lidos > 1) {
            fprintf(stderr, "Erro ao ler linha (campos insuficientes, lidos %d): %s\n", campos_lidos, linha);
        }
    }

    fclose(f);
    printf("Clientes carregados: %zu\n", L->tam);
    return 1;
}

int registrar_movimento(const char* arq_movimentos, const char* conta, const char* tipo, double valor, double saldo_apos) {
    FILE* f = fopen(arq_movimentos, "a");
    if (!f) {
        perror("Erro ao abrir arquivo de movimentos para escrita");
        return 0;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(f, "%d-%02d-%02d;%s;%s;%.2f;saldo=%.2f\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            conta, tipo, valor, saldo_apos);

    fclose(f);
    return 1;
}

void gerar_numero_conta(char destino[], int novo_numero) {
    sprintf(destino, "%04d", novo_numero);
}
