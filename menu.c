#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funcao para exibir e ler a opcao do menu
int menu_principal() {
    int opcao;
    printf("\n--- Sistema de Agendamento de Quadras ---\n");
    printf("1. Adicionar Nova Reserva\n");
    printf("2. Listar Disponibilidade do Dia\n");
    printf("3. Cancelar Reserva\n");
    printf("4. Atualizar Hor�rio de Reserva\n");
    printf("5. Listar Minhas Reservas (por RA)\n");
    printf("0. Sair e Salvar\n");
    printf("Escolha uma op��o: ");
    scanf("%d", &opcao);
    // Limpa o buffer aps scanf para evitar problemas com fgets/gets futuros
    while (getchar() != '\n'); 
    return opcao;
}

// Implementa��o das fun��es de opera��o

void menu_nova_reserva(Repo* repo) {
    Reserva r;
    r.id = repo_proximoId(repo);
    r.ativa = 1;
    
    printf("\n--- Nova Reserva ---\n");
    printf("ID da Reserva: %d\n", r.id);
    
    printf("Quadra (1 ou 2): ");
    scanf("%d", &r.quadraId);
    while (getchar() != '\n'); // Limpa buffer

    printf("Data (YYYY-MM-DD): ");
    fgets(r.data, MAX_DATA, stdin);
    r.data[strcspn(r.data, "\n")] = 0;

    printf("Hora (HH:MM - ex: 18:00): ");
    fgets(r.hora, MAX_HORA, stdin);
    r.hora[strcspn(r.hora, "\n")] = 0;

    printf("RA (Registro Acadêmico): ");
    fgets(r.ra, MAX_RA, stdin);
    r.ra[strcspn(r.ra, "\n")] = 0;

    /* * AUTOPREENCHIMENTO  * */
    const Reserva* dadosAntigos = repo_buscarUltimaPorRa(repo, r.ra);

    if (dadosAntigos == NULL) {
        // Usuário NÃO encontrado. Esta é a primeira reserva dele.
        printf("\n-> Primeira reserva. Por favor, complete seus dados:\n");
        
        printf("Nome: ");
        fgets(r.nome, MAX_NOME, stdin);
        r.nome[strcspn(r.nome, "\n")] = 0;

        printf("Email: ");
        fgets(r.email, MAX_EMAIL, stdin);
        r.email[strcspn(r.email, "\n")] = 0;

        printf("Telefone: ");
        fgets(r.telefone, MAX_TEL, stdin);
        r.telefone[strcspn(r.telefone, "\n")] = 0;

    } else {
        // Usuário ENCONTRADO.
        printf("\n-> Usuário encontrado! Dados preenchidos automaticamente.\n");
        
        // Copia os dados da reserva antiga para a nova
        strcpy(r.nome, dadosAntigos->nome);
        strcpy(r.email, dadosAntigos->email);
        strcpy(r.telefone, dadosAntigos->telefone);

        // Apenas exibe os dados para o usuário confirmar
        printf("Nome: %s\n", r.nome);
        printf("Email: %s\n", r.email);
        printf("Telefone: %s\n", r.telefone);
    }

    if (repo_adicionar(repo, &r)) {
        printf("\n Reserva %d adicionada com sucesso!\n", r.id);
    } else {
        printf("\n ERRO: Horário já ocupado (Quadra:%d, Data:%s, Hora:%s).\n", 
                r.quadraId, r.data, r.hora);
    }
}

void menu_listar_dia(const Repo* repo) {
    int quadraId;
    char data[MAX_DATA];

    printf("\n--- Consultar Disponibilidade ---\n");
    printf("Quadra (1 ou 2): ");
    scanf("%d", &quadraId);
    while (getchar() != '\n');

    printf("Data (YYYY-MM-DD): ");
    fgets(data, MAX_DATA, stdin);
    data[strcspn(data, "\n")] = 0;

    repo_listarDoDia(repo, quadraId, data);
}

void menu_cancelar_reserva(Repo* repo) {
    int id;
    char ra[MAX_RA];

    printf("\n--- Cancelar Reserva ---\n");
    printf("ID da Reserva a cancelar: ");
    scanf("%d", &id);
    while (getchar() != '\n');

    printf("RA (para confirma��o): ");
    fgets(ra, MAX_RA, stdin);
    ra[strcspn(ra, "\n")] = 0;
    
    int resultado = repo_cancelarPorId(repo, id, ra);

    if (resultado == 1) {
        printf("\n? Reserva %d cancelada com sucesso.\n", id);
    } else if (resultado == -1) {
        printf("\n? ERRO: Reserva ID %d n�o encontrada.\n", id);
    } else if (resultado == -2) {
        printf("\n? ERRO: RA fornecido n�o corresponde ao titular da reserva.\n");
    } else if (resultado == -3) {
        printf("\n? ERRO: Reserva ID %d j� estava cancelada.\n", id);
    }
}

void menu_atualizar_reserva(Repo* repo) {
    int id, novoQuadra;
    char ra[MAX_RA], novaData[MAX_DATA], novaHora[MAX_HORA];

    printf("\n--- Atualizar Hor�rio de Reserva ---\n");
    printf("ID da Reserva a atualizar: ");
    scanf("%d", &id);
    while (getchar() != '\n');

    printf("RA (para confirma��o): ");
    fgets(ra, MAX_RA, stdin);
    ra[strcspn(ra, "\n")] = 0;

    printf("\n-- Novas Informa��es --\n");
    printf("Nova Quadra (1 ou 2): ");
    scanf("%d", &novoQuadra);
    while (getchar() != '\n');

    printf("Nova Data (YYYY-MM-DD): ");
    fgets(novaData, MAX_DATA, stdin);
    novaData[strcspn(novaData, "\n")] = 0;

    printf("Nova Hora (HH:MM - ex: 18:00): ");
    fgets(novaHora, MAX_HORA, stdin);
    novaHora[strcspn(novaHora, "\n")] = 0;

    int resultado = repo_atualizarHorario(repo, id, ra, novoQuadra, novaData, novaHora);

    if (resultado == 1) {
        printf("\n? Reserva %d atualizada para Quadra %d em %s %s.\n", id, novoQuadra, novaData, novaHora);
    } else if (resultado == 0) {
        printf("\n? ERRO: O novo hor�rio (Quadra:%d, Data:%s, Hora:%s) j� est� ocupado por outra reserva.\n", 
               novoQuadra, novaData, novaHora);
    } else if (resultado == -1) {
        printf("\n? ERRO: Reserva ID %d n�o encontrada.\n", id);
    } else if (resultado == -2) {
        printf("\n? ERRO: RA fornecido n�o corresponde ao titular da reserva.\n");
    } else if (resultado == -3) {
        printf("\n? ERRO: Reserva ID %d est� cancelada e n�o pode ser atualizada.\n", id);
    }
}

void menu_consultar_por_ra(Repo* repo) {
    char ra[MAX_RA];

    printf("\n--- Listar Minhas Reservas ---\n");
    printf("Digite o seu RA para a consulta: ");
    
    fgets(ra, MAX_RA, stdin);
    ra[strcspn(ra, "\n")] = 0;

    printf("\nBuscando reservas para o RA: %s\n", ra);
    
    // Função recursiva criada em repo.c
    repo_listarPorRa(repo, ra);
}

// Processa a op��o escolhida no menu principal
void menu_processar(Repo* repo, int opcao) {
    switch (opcao) {
        case 1:
            menu_nova_reserva(repo);
            break;
        case 2:
            menu_listar_dia(repo);
            break;
        case 3:
            menu_cancelar_reserva(repo);
            break;
        case 4:
            menu_atualizar_reserva(repo);
            break;
        case 5:
            menu_consultar_por_ra(repo);
            break;
        case 0:
            printf("\nSalvando dados e encerrando o sistema. At� mais!\n");
            break;
        default:
            printf("\nOp��o inv�lida. Por favor, escolha uma op��o entre 0 e 5.\n");
            break;
    }
}