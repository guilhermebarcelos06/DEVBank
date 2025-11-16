#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "banco.h"
#include "io.h"
#include "cliente.h"

void exibir_menu(void);
int ler_opcao(void);
void lidar_abrir_conta(Banco* b);
void lidar_encerrar_conta(Banco* b);
void lidar_depositar(Banco* b);
void lidar_sacar(Banco* b);
void lidar_consultar_cliente(const Banco* b);
void lidar_alterar_dados_cliente(Banco* b);
void lidar_exibir_extrato(Banco* b);


int main(void) {
	setlocale(LC_ALL, "Portuguese");
    Banco meu_banco;
    const char* arq_clientes = "data/clientes.txt";
    const char* arq_movimentos = "data/movimentos.txt";

    banco_init(&meu_banco, arq_clientes, arq_movimentos);
    banco_carregar(&meu_banco);

    int opcao;
    do {
        exibir_menu();
        opcao = ler_opcao();

        switch (opcao) {
            case 1:
                lidar_abrir_conta(&meu_banco);
                break;
            case 2:
                lidar_encerrar_conta(&meu_banco);
                break;
            case 3:
                lidar_consultar_cliente(&meu_banco);
                break;
            case 4:
                lidar_alterar_dados_cliente(&meu_banco);
                break;
            case 5:
                lidar_depositar(&meu_banco);
                break;
            case 6:
                lidar_sacar(&meu_banco);
                break;
            case 7:
                banco_listar_clientes(&meu_banco);
                break;
            case 8:
                banco_listar_clientes_ordenado_por_nome(&meu_banco);
                break;
            case 9:
                lidar_exibir_extrato(&meu_banco);
                break;
            case 0:
                printf("Saindo do Banco Malvader. Salvando dados...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }

        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (opcao != 0);

    banco_salvar(&meu_banco);
    banco_free(&meu_banco);

    return 0;
}

void exibir_menu(void) {
    printf("\n--- DEVBank ---\n");
    printf("1. Abrir nova conta\n");
    printf("2. Encerrar conta\n");
    printf("3. Consultar dados do cliente\n");
    printf("4. Alterar dados do cliente\n");
    printf("5. Realizar deposito\n");
    printf("6. Realizar saque\n");
    printf("7. Listar todos os clientes\n");
    printf("8. Listar clientes (Ordenado por Nome)\n");
    printf("9. Exibir extrato da conta\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

int ler_opcao(void) {
    int op;
    char buffer[100];

    ler_linha(buffer, sizeof(buffer));

    if (sscanf(buffer, "%d", &op) != 1) {
        return -1;
    }

    return op;
}


void lidar_abrir_conta(Banco* b) {
    Cliente novo = novo_cliente_padrao();

    printf("\n--- Abrir Nova Conta ---\n");

    printf("Nome completo: ");
    ler_linha(novo.nome, sizeof(novo.nome));

    printf("CPF (apenas numeros): ");
    ler_linha(novo.cpf, sizeof(novo.cpf));

	int proximo_numero = (int)b->clientes.tam + 1;
    gerar_numero_conta(novo.conta, proximo_numero);
    printf("Numero da Conta gerado: %s\n", novo.conta);

    printf("Senha: ");
    ler_linha(novo.senha, sizeof(novo.senha));

    printf("Data de Nascimento (DD-MM-AAAA): ");
    ler_linha(novo.data_nasc, sizeof(novo.data_nasc));

    printf("Telefone: ");
    ler_linha(novo.telefone, sizeof(novo.telefone));

    printf("Endereço (Rua, Numero): ");
    ler_linha(novo.endereco, sizeof(novo.endereco));

    printf("Numero da Casa: ");
    ler_linha(novo.numero_casa, sizeof(novo.numero_casa));

    printf("Bairro: ");
    ler_linha(novo.bairro, sizeof(novo.bairro));

    printf("CEP: ");
    ler_linha(novo.cep, sizeof(novo.cep));

    printf("Local (Ponto de referencia): ");
    ler_linha(novo.local, sizeof(novo.local));

    printf("Cidade: ");
    ler_linha(novo.cidade, sizeof(novo.cidade));

    printf("Estado (UF): ");
    ler_linha(novo.estado, sizeof(novo.estado));

    if (banco_abrir_conta(b, novo)) {
        printf("Conta aberta e cliente cadastrado com sucesso!\n");
    } else {
        printf("Falha ao abrir conta. Verifique os dados.\n");
    }
}

void lidar_encerrar_conta(Banco* b) {
    char conta[16];
    printf("\n--- Encerrar Conta ---\n");
    printf("Digite o numero da conta a ser encerrada: ");
    ler_linha(conta, sizeof(conta));

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        printf("Erro: Conta %s nao encontrada.\n", conta);
        return;
    }

    char senha_digitada[50];
    printf("Digite sua senha para confirmar: ");
    ler_linha(senha_digitada, sizeof(senha_digitada));

    if (!banco_validar_senha(b, idx, senha_digitada)) {
        printf("Senha incorreta. Operacao cancelada.\n");
        return;
    }

    if (banco_encerrar_conta(b, conta)) {

    } else {

    }
}

void lidar_depositar(Banco* b) {
    char conta[16];
    double valor;
    printf("\n--- Realizar Deposito ---\n");
    printf("Digite o numero da conta: ");
    ler_linha(conta, sizeof(conta));

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        printf("Erro: Conta %s nao encontrada.\n", conta);
        return;
    }
    if (!b->clientes.dados[idx].ativo) {
        fprintf(stderr, "Erro: Conta %s esta inativa e nao pode receber depositos.\n", conta);
        return;
    }

    char senha_digitada[50];
    printf("Digite sua senha: ");
    ler_linha(senha_digitada, sizeof(senha_digitada));

    if (!banco_validar_senha(b, idx, senha_digitada)) {
        printf("Senha incorreta. Operacao cancelada.\n");
        return;
    }

    printf("Digite o valor do deposito: ");
    char buffer_valor[50];
    ler_linha(buffer_valor, sizeof(buffer_valor));
    // Substitui vírgula por ponto para o sscanf/atof
    char* comma = strchr(buffer_valor, ',');
    if(comma) *comma = '.';

    if (sscanf(buffer_valor, "%lf", &valor) != 1) {
        printf("Valor invalido.\n");
        return;
    }

    if (banco_depositar(b, conta, valor)) {
        printf("Deposito realizado com sucesso.\n");
    } else {
        printf("Falha ao realizar deposito.\n");
    }
}

void lidar_sacar(Banco* b) {
    char conta[16];
    double valor;
    printf("\n--- Realizar Saque ---\n");
    printf("Digite o numero da conta: ");
    ler_linha(conta, sizeof(conta));

    // --- VALIDAÇÃO DE SENHA ---
    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        printf("Erro: Conta %s nao encontrada.\n", conta);
        return;
    }
    if (!b->clientes.dados[idx].ativo) {
        fprintf(stderr, "Erro: Conta %s esta inativa e nao pode realizar saques.\n", conta);
        return;
    }

    char senha_digitada[50];
    printf("Digite sua senha: ");
    ler_linha(senha_digitada, sizeof(senha_digitada));

    if (!banco_validar_senha(b, idx, senha_digitada)) {
        printf("Senha incorreta. Operacao cancelada.\n");
        return;
    }

    printf("Digite o valor do saque: ");
    char buffer_valor[50];
    ler_linha(buffer_valor, sizeof(buffer_valor));
    // Substitui vírgula por ponto
    char* comma = strchr(buffer_valor, ',');
    if(comma) *comma = '.';

    if (sscanf(buffer_valor, "%lf", &valor) != 1) {
        printf("Valor invalido.\n");
        return;
    }

    if (banco_sacar(b, conta, valor)) {
        printf("Saque realizado com sucesso.\n");
    } else {
        printf("Falha ao realizar saque.\n");
    }
}

void lidar_consultar_cliente(const Banco* b) {
    char termo_busca[100];
    printf("\n--- Consultar Cliente ---\n");
    printf("Deseja buscar por CPF (1) ou Numero da Conta (2)? ");
    int tipo_busca = ler_opcao();
    int idx = -1;

    if (tipo_busca == 1) {
        printf("Digite o CPF do cliente: ");
        ler_linha(termo_busca, sizeof(termo_busca));
        idx = lista_clientes_buscar_por_cpf(&b->clientes, termo_busca);
    } else if (tipo_busca == 2) {
        printf("Digite o Numero da Conta do cliente: ");
        ler_linha(termo_busca, sizeof(termo_busca));
        idx = lista_clientes_buscar_por_conta(&b->clientes, termo_busca);
    } else {
        printf("Opcao de busca invalida.\n");
        return;
    }

    if (idx != -1) {
        imprimir_dados_cliente(&b->clientes.dados[idx]);
    } else {
        printf("Cliente nao encontrado.\n");
    }
}

void lidar_alterar_dados_cliente(Banco* b) {
    char conta[16];
    printf("\n--- Alterar Dados do Cliente ---\n");
    printf("Digite o numero da conta: ");
    ler_linha(conta, sizeof(conta));

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        printf("Erro: Conta %s nao encontrada.\n", conta);
        return;
    }

    // Validação de senha
    char senha_digitada[50];
    printf("Digite sua senha para autorizar a alteração: ");
    ler_linha(senha_digitada, sizeof(senha_digitada));

    if (!banco_validar_senha(b, idx, senha_digitada)) {
        printf("Senha incorreta. Operacao cancelada.\n");
        return;
    }

    printf("\nCliente encontrado: %s\n", b->clientes.dados[idx].nome);
    printf("O que deseja alterar?\n");
    printf("1. Telefone\n");
    printf("2. Endereço Completo\n");
    printf("3. Senha\n");
    printf("0. Cancelar\n");
    printf("Escolha uma opção: ");
    int op_alterar = ler_opcao();

    char novo_valor[120];

    switch(op_alterar) {
        case 1:
            printf("Digite o novo telefone: ");
            ler_linha(novo_valor, sizeof(novo_valor));
            banco_alterar_dados_cliente(b, idx, op_alterar, novo_valor);
            break;
        case 2:
            banco_alterar_dados_cliente(b, idx, op_alterar, NULL);
            break;
        case 3:
            printf("Digite a nova senha: ");
            ler_linha(novo_valor, sizeof(novo_valor));
            banco_alterar_dados_cliente(b, idx, op_alterar, novo_valor);
            break;
        case 0:
            printf("Alteracao cancelada.\n");
            break;
        default:
            printf("Opcao invalida.\n");
            break;
    }
}

void lidar_exibir_extrato(Banco* b) {
    char conta[16];
    printf("\n--- Exibir Extrato ---\n");
    printf("Digite o numero da conta: ");
    ler_linha(conta, sizeof(conta));

    int idx = lista_clientes_buscar_por_conta(&b->clientes, conta);
    if (idx == -1) {
        printf("Erro: Conta %s nao encontrada.\n", conta);
        return;
    }

    char senha_digitada[50];
    printf("Digite sua senha para ver o extrato: ");
    ler_linha(senha_digitada, sizeof(senha_digitada));

    if (!banco_validar_senha(b, idx, senha_digitada)) {
        printf("Senha incorreta. Operacao cancelada.\n");
        return;
    }

    banco_exibir_extrato(b, conta);
}
