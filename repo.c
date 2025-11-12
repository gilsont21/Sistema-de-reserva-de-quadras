#include "repo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// catálogo simples de horas
const char* REPO_HORAS[] = {
    "08:00","09:00","10:00","11:00","12:00","13:00",
    "14:00","15:00","16:00","17:00","18:00","19:00",
    "20:00","21:00","22:00"
};
int REPO_QTD_HORAS = 15;

//helpers locais
int repo_str_igual(const char* a, const char* b){ return strcmp(a,b) == 0; }

int repo_conflito_mesmo_slot(const Reserva* r, int quadraId, const char* data, const char* hora){
    return (r->ativa == 1 &&
            r->quadraId == quadraId &&
            strcmp(r->data, data) == 0 &&
            strcmp(r->hora, hora) == 0);
}

// implementação recursiva para listar reservas por RA
static int repo_listarPorRa_recursivo(const Repo* repo, const char* ra, int idx) {

    if (idx == repo->n) {
        return 0;
    }

    int encontrei_agora = 0;

    // Verifica se o RA da reserva é o procurado
    if (repo_str_igual(repo->vet[idx].ra, ra)) {
        // Se for, imprime a reserva
        reserva_printLinha(&repo->vet[idx]);
        encontrei_agora = 1;
    }

    return encontrei_agora + repo_listarPorRa_recursivo(repo, ra, idx + 1);
}

int repo_garante_cap(Repo* repo){
    if (repo->n < repo->cap) return 1;
    int novo = (repo->cap == 0 ? 8 : repo->cap*2);
    Reserva* p = (Reserva*) realloc(repo->vet, novo * sizeof(Reserva));
    if (!p) return 0;
    repo->vet = p;
    repo->cap = novo;
    return 1;
}

//ciclo de vida
void repo_init(Repo* repo, const char* path){
    repo->vet = NULL;
    repo->n = 0;
    repo->cap = 0;
    strncpy(repo->path, path, MAX_PATH-1);
    repo->path[MAX_PATH-1] = '\0';
}

void repo_free(Repo* repo){
    free(repo->vet);
    repo->vet = NULL;
    repo->n = repo->cap = 0;
}

//arquivo
int repo_salvar(const Repo* repo){
    FILE* f = fopen(repo->path, "w");
    if (!f) return 0;
    fprintf(f, "id;quadraId;data;hora;ra;nome;email;telefone;ativa\n");
    for (int i=0;i<repo->n;i++){
        const Reserva* r = &repo->vet[i];
        fprintf(f, "%d;%d;%s;%s;%s;%s;%s;%s;%d\n",
            r->id, r->quadraId, r->data, r->hora,
            r->ra, r->nome, r->email, r->telefone, r->ativa);
    }
    fclose(f);
    return 1;
}

int repo_carregar(Repo* repo){
    FILE* f = fopen(repo->path, "r");
    if (!f){
        // se não existir, tudo bem, será criado no primeiro salvar
        return 1;
    }
    char linha[512];

    // tentar descartar cabeçalho, se houver
    if (!fgets(linha, sizeof(linha), f)){
        fclose(f);
        return 1;
    }

    while (fgets(linha, sizeof(linha), f)){
        Reserva r;
        char *tok;
        tok = strtok(linha, ";\n"); if(!tok) continue; r.id = atoi(tok);
        tok = strtok(NULL, ";\n"); if(!tok) continue; r.quadraId = atoi(tok);
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.data, tok, MAX_DATA-1); r.data[MAX_DATA-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.hora, tok, MAX_HORA-1); r.hora[MAX_HORA-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.ra, tok, MAX_RA-1); r.ra[MAX_RA-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.nome, tok, MAX_NOME-1); r.nome[MAX_NOME-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.email, tok, MAX_EMAIL-1); r.email[MAX_EMAIL-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; strncpy(r.telefone, tok, MAX_TEL-1); r.telefone[MAX_TEL-1]='\0';
        tok = strtok(NULL, ";\n"); if(!tok) continue; r.ativa = atoi(tok);

        if (!repo_garante_cap(repo)){ fclose(f); return 0; }
        repo->vet[repo->n++] = r;
    }
    fclose(f);
    return 1;
}

// buscas / regras
int repo_proximoId(const Repo* repo){
    int maior = 0;
    for (int i=0;i<repo->n;i++)
        if (repo->vet[i].id > maior) maior = repo->vet[i].id;
    return maior + 1;
}

int repo_buscarIdxPorId(const Repo* repo, int id){
    for (int i=0;i<repo->n;i++)
        if (repo->vet[i].id == id) return i;
    return -1;
}

int repo_buscarIdxSlotAtivo(const Repo* repo, int quadraId, const char* data, const char* hora){
    for (int i=0;i<repo->n;i++)
        if (repo_conflito_mesmo_slot(&repo->vet[i], quadraId, data, hora))
            return i;
    return -1;
}

//operações
int repo_adicionar(Repo* repo, const Reserva* r){
    if (repo_buscarIdxSlotAtivo(repo, r->quadraId, r->data, r->hora) != -1)
        return 0; /* conflito */
    if (!repo_garante_cap(repo)) return 0;
    repo->vet[repo->n++] = *r;
    return 1;
}

int repo_cancelarPorId(Repo* repo, int id, const char* ra){
    int idx = repo_buscarIdxPorId(repo, id);
    if (idx < 0) return -1;                 // não achou
    if (!repo_str_igual(repo->vet[idx].ra, ra)) return -2; //RA diferente
    if (repo->vet[idx].ativa == 0) return -3;             //já cancelada 
    repo->vet[idx].ativa = 0;
    return 1;
}

int repo_atualizarHorario(Repo* repo, int id, const char* ra,
                          int novoQuadra, const char* novaData, const char* novaHora){
    int idx = repo_buscarIdxPorId(repo, id);
    if (idx < 0) return -1;
    if (!repo_str_igual(repo->vet[idx].ra, ra)) return -2;
    if (repo->vet[idx].ativa == 0) return -3;

    //se novo slot já está ocupado por outra reserva ativa, bloqueia
    int conflito = repo_buscarIdxSlotAtivo(repo, novoQuadra, novaData, novaHora);
    if (conflito != -1 && repo->vet[conflito].id != id) return 0;

    repo->vet[idx].quadraId = novoQuadra;
    strncpy(repo->vet[idx].data, novaData, MAX_DATA-1); repo->vet[idx].data[MAX_DATA-1]='\0';
    strncpy(repo->vet[idx].hora, novaHora, MAX_HORA-1); repo->vet[idx].hora[MAX_HORA-1]='\0';
    return 1;
}

//consulta de disponibilidade, imprime o dia
void repo_listarDoDia(const Repo* repo, int quadraId, const char* data){
    printf("\nQuadra %d | Data %s\n", quadraId, data);
    for (int h=0; h<REPO_QTD_HORAS; h++){
        int idx = repo_buscarIdxSlotAtivo(repo, quadraId, data, REPO_HORAS[h]);
        if (idx == -1){
            printf("%s - LIVRE\n", REPO_HORAS[h]);
        } else {
            const Reserva* r = &repo->vet[idx];
            printf("%s - OCUPADO (RA:%s | %s)\n", REPO_HORAS[h], r->ra, r->nome);
        }
    }
}

//lista reservas por RA
void repo_listarPorRa(const Repo* repo, const char* ra) {
    
    // 1. Dispara a função recursiva, começando pelo índice 0
    int totalEncontrado = repo_listarPorRa_recursivo(repo, ra, 0);

    if (totalEncontrado == 0) {
        printf("-> Nenhuma reserva (ativa ou cancelada) encontrada para este RA.\n");
    } else {
        printf("-> Total de %d reserva(s) encontrada(s) para este RA.\n", totalEncontrado);
    }
}

const Reserva* repo_buscarUltimaPorRa(const Repo* repo, const char* ra) {

    for (int i = repo->n - 1; i >= 0; i--) {
        // Check ra
        if (repo_str_igual(repo->vet[i].ra, ra)) {
            return &repo->vet[i]; 
        }
    }

    return NULL; 
}