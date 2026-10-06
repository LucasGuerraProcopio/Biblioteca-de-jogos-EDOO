#include <iostream>
#include <string>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include "catalogo.hpp"
#include "usuario.hpp"
#include "menu.hpp"
#include "caminho.hpp"
#include "jogos_iniciais.hpp"
using namespace std;


int main(int argc, char* argv[])
{
    // deixa o terminal em UTF-8 para mostrar os acentos certos
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    // o banco fica na mesma pasta do programa
    string pasta = "";
    if(argc > 0)
    {
        pasta = pasta_do_programa(argv[0]);
    };

    banco_de_dados banco(pasta + "biblioteca.db");

    if(banco.aberto() == false)
    {
        cout << "Sem o banco de dados o programa não pode continuar." << endl;
        return 1;
    };

    catalogo loja(&banco);
    repositorio_usuarios usuarios(&banco);

    // carrega os jogos salvos
    int carregados = loja.carregar_do_banco();

    // os jogos iniciais só são cadastrados na primeira vez, mesmo que depois todos sejam removidos
    cadastrar_jogos_iniciais(banco, loja, carregados);

    if(carregados > 0)
    {
        cout << carregados << " jogos carregados do banco." << endl;
    };

    // carrega as contas salvas, com biblioteca, saldo e cartões
    int contas_carregadas = usuarios.carregar_do_banco();

    if(contas_carregadas > 0)
    {
        cout << contas_carregadas << " contas carregadas do banco." << endl;
    };

    executar_menu(loja, usuarios);

    return 0;
}