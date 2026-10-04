#include <iostream>
#include "catalogo.hpp"
#include "usuario.hpp"
using namespace std;


int main()
{
    banco_de_dados banco("biblioteca.db");
    catalogo loja(&banco);
    repositorio_usuarios usuarios;

    // carrega os jogos salvos. Se o banco estiver vazio, cadastra os jogos iniciais
    int carregados = loja.carregar_do_banco();

    if(carregados == 0)
    {
        // jogos entram no catálogo e recebem um id
        loja.adicionar(new jogos_gratuitos("Fortinite", 90, "Torres_tortas", "Tripaboy_"));
        loja.adicionar(new jogos_gratuitos("Roblox", 5, "29/01/2021", "Francisco"));
        loja.adicionar(new jogos_gratuitos("Five nights at Freddy", 10, "FFCBG123456"));
        loja.adicionar(new jogos_gratuitos("Brawl Stars", 0.8, "HIPERCARGA67", "Piriquito Deuz"));
        loja.adicionar(new jogos_pagos("Rainbow six siege", 70, "Odeio_escudos123", 50));
        loja.adicionar(new jogos_pagos("Minecraft", 35, "I am Steve", 40, "Eduardo Cabral"));
        loja.adicionar(new jogos_pagos("Mortal Kombat", 80, "Get over here", 200));
        loja.adicionar(new jogos_pagos("FC 26", 80, "Futebol123", 500));
        loja.adicionar(new jogos_pagos("Grand Theft Auto 6", 220, "Rockstar mercenaria", 550, "CJ"));
        loja.adicionar(new jogos_gratuitos("Roblox", 5, "x")); // recusado: título repetido
    }
    else
    {
        cout << carregados << " jogos carregados do banco." << endl;
    };

    loja.listar();

    jogos_gratuitos* roblox = loja.buscar_por_titulo("Roblox");
    jogos_gratuitos* minecraft = loja.buscar_por_titulo("Minecraft");
    jogos_gratuitos* gta = loja.buscar_por_id(9);

    if(roblox == nullptr || minecraft == nullptr || gta == nullptr)
    {
        cout << "Os jogos do teste já foram removidos do banco. Apague o arquivo biblioteca.db para rodar o teste de novo." << endl;
        return 0;
    };

    usuario* ana = usuarios.criar("Ana", "senha1");
    usuario* bia = usuarios.criar("Bia", "senha2");
    usuarios.criar("Ana", "outra");

    usuarios.listar();

    gift_card card("GIFT-1111", 50);

    ana->depositar_pix(100, "biblioteca-jogos@exemplo.com");
    ana->Cadastrar_GiftCard(card, "GIFT-1111");
    ana->Cadastrar_GiftCard(card, "GIFT-1111");

    ana->adquirir(*roblox);
    ana->adquirir(*roblox);
    ana->adquirir(*gta);
    ana->adquirir(*minecraft);

    ana->instalar(minecraft->GetId());
    ana->instalar(roblox->GetId());
    ana->desinstalar(roblox->GetId());

    bia->depositar_pix(100, "biblioteca-jogos@exemplo.com");
    bia->adquirir(*minecraft);

    ana->mostrar_biblioteca(loja);
    bia->mostrar_biblioteca(loja);

    ana->reembolsar(*minecraft);

    ana->mostrar_biblioteca(loja);
    bia->mostrar_biblioteca(loja); 


    // deleta jogos
    cout << "\n===== DELETE =====" << endl;
    usuarios.excluir_jogo(loja, 6);
    usuarios.excluir_jogo(loja, 2);
    usuarios.excluir_jogo(loja, 99);
    usuarios.excluir_jogo(loja, 7);

    loja.listar();

    // deleta usuario
    usuarios.remover(2);
    usuarios.remover(2);
    usuarios.excluir_jogo(loja, 6);

    loja.listar();
    usuarios.listar();
    ana->mostrar_biblioteca(loja);

    return 0;
}