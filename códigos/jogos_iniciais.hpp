#pragma once
#include "catalogo.hpp"
using namespace std;


// cadastra os jogos iniciais só na primeira vez que o banco é usado,
// mesmo que depois todos sejam removidos (usado pelo menu e pelo servidor)
inline void cadastrar_jogos_iniciais(banco_de_dados& banco, catalogo& loja, int carregados)
{
    if(banco.ler_controle("jogos_iniciais") != 0)
    {
        return;
    };

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
}
