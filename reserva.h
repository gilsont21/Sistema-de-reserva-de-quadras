#ifndef RESERVA_H
#define RESERVA_H

#include <stdio.h>

#define MAX_PATH   64
#define MAX_DATA   13   //"YYYY-MM-DD"
#define MAX_HORA    8   //"HH:MM"
#define MAX_RA     16
#define MAX_NOME   41
#define MAX_EMAIL  41
#define MAX_TEL    16

typedef struct {
    int  id;
    int  quadraId;
    char data[MAX_DATA];
    char hora[MAX_HORA];
    char ra[MAX_RA];
    char nome[MAX_NOME];
    char email[MAX_EMAIL];
    char telefone[MAX_TEL];
    int  ativa;           //1=ativa, 0=cancelada
} Reserva;

//utilitário simples para imprimir uma linha da reserva
void reserva_printLinha(const Reserva* r);

#endif