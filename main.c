#include "repo.h"
#include "menu.h"
#include <stdio.h>
#include <locale.h>

int main() {

    // Para utilizar caractere especial
    setlocale(LC_ALL, "Portuguese");
    // 1. Inicializa o reposit�rio
    Repo repo;
    repo_init(&repo, "reservas.csv");
    printf("Sistema de Agendamento iniciado.\n");

    // 2. Carrega os dados do arquivo
    if (repo_carregar(&repo)) {
        printf("Dados carregados com sucesso do arquivo '%s'.\n", repo.path);
    } else {
        // repo_carregar retorna 1 mesmo se o arquivo n�o existir
        // A mensagem de erro s� � relevante se ocorrer um erro de aloca��o, etc.
        fprintf(stderr, "Aten��o: N�o foi poss�vel carregar os dados. Iniciando com reposit�rio vazio.\n");
    }
    
    // 3. Loop principal
    int opcao;
    do {
        opcao = menu_principal();
        menu_processar(&repo, opcao);
    } while (opcao != 0);

    // 4. Salva os dados (se Sair for escolhido)
    if (repo_salvar(&repo)) {
        printf("Dados salvos no arquivo '%s'.\n", repo.path);
    } else {
        fprintf(stderr, "ERRO FATAL: Falha ao salvar os dados no arquivo '%s'.\n", repo.path);
    }

    // 5. Libera a mem�ria
    repo_free(&repo);
    return 0;
}