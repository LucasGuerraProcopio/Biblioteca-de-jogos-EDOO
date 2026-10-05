#include <iostream>
#include "catalogo.hpp"
#include "usuario.hpp"
#include "menu.hpp"
using namespace std;


int main()
{
    banco_de_dados banco("biblioteca.db");

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
    if(banco.ler_controle("jogos_iniciais") == 0)
    {
        if(carregados == 0)
        {
            loja.adicionar(new jogos_gratuitos("Fortnite", 90, "Torres_tortas", "Tripaboy_"));
            loja.adicionar(new jogos_gratuitos("Roblox", 5, "29/01/2021", "Francisco"));
            loja.adicionar(new jogos_gratuitos("Five nights at Freddy", 10, "FFCBG123456"));
            loja.adicionar(new jogos_gratuitos("Brawl Stars", 0.8, "HIPERCARGA67", "Piriquito Deuz"));
            loja.adicionar(new jogos_pagos("Rainbow six siege", 70, "Odeio_escudos123", 50));
            loja.adicionar(new jogos_pagos("Minecraft", 35, "I am Steve", 40, "Eduardo Cabral"));
            loja.adicionar(new jogos_pagos("Mortal Kombat", 80, "Get over here", 200));
            loja.adicionar(new jogos_pagos("FC 26", 80, "Futebol123", 500));
            loja.adicionar(new jogos_pagos("Grand Theft Auto 6", 220, "Rockstar mercenaria", 550, "CJ"));
        };

        banco.salvar_controle("jogos_iniciais", 1);
    };

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