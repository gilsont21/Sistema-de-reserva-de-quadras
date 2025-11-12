#include "reserva.h"
#include <stdio.h>

void reserva_printLinha(const Reserva* r){
    printf("ID:%d | Q:%d | %s %s | RA:%s | %s | %s | %s | %s\n",
           r->id, r->quadraId, r->data, r->hora,
           r->ra, r->nome, r->email, r->telefone,
           r->ativa ? "ATIVA" : "CANCELADA");
}
