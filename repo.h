#ifndef REPO_H
#define REPO_H

#include "reserva.h"

typedef struct {
    Reserva* vet;
    int n;     //usados
    int cap; 
    char path[MAX_PATH];  // "reservas.csv"
} Repo;

// ciclo de vida
void repo_init(Repo* repo, const char* path);
void repo_free(Repo* repo);

//arquivo
int  repo_carregar(Repo* repo);
int  repo_salvar(const Repo* repo);

//operações
int  repo_proximoId(const Repo* repo);
int  repo_adicionar(Repo* repo, const Reserva* r);
int  repo_cancelarPorId(Repo* repo, int id, const char* ra);
int  repo_atualizarHorario(Repo* repo, int id, const char* ra,
                           int novoQuadra, const char* novaData, const char* novaHora);

//consulta de disponibilidade
void repo_listarDoDia(const Repo* repo, int quadraId, const char* data);

//lista reservas por RA
void repo_listarPorRa(const Repo* repo, const char* ra);

//funções auxiliares
const Reserva* repo_buscarUltimaPorRa(const Repo* repo, const char* ra);

#endif
