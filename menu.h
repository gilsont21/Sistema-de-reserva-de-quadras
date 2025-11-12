#ifndef MENU_H
#define MENU_H

#include "repo.h"

//Exibe o menu principal e solicita uma opção
int menu_principal();

//Lida com a opção escolhida pelo usuário, chamando as funções do Repo
void menu_processar(Repo* repo, int opcao);

//Funções utilitárias para entrada de dados
void menu_nova_reserva(Repo* repo);
void menu_listar_dia(const Repo* repo);
void menu_cancelar_reserva(Repo* repo);
void menu_atualizar_reserva(Repo* repo);
void menu_consultar_por_ra(Repo* repo);

#endif