#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
using namespace std;

// Classe do usuario 
class acoes_jogos
{
    // Informações da conta
    protected:
        int total_jogos_instalados = 0; 
        int total_jogos_comprados = 0;
        vector <jogos_gratuitos*> jogos_gratis; // Listas dos jogos que guarda PONTEIROS para os objetos
        vector <jogos_pagos*> jogos_comprados;


    // Operações com o usuario
    public:
        // Instalar e desinstalar os jogos
        void installar(jogos_gratuitos& jogo)
        {
            jogos_pagos* jogo_pago = dynamic_cast<jogos_pagos*>(&jogo);

            if(jogo_pago != nullptr)
            {
                jogo_pago->comprar();
                jogos_comprados.emplace_back(jogo_pago);
                total_jogos_comprados++;
            }
            else
            {
                jogos_gratis.emplace_back(&jogo);
                total_jogos_instalados++;
            };

            cout << "O jogo: " << jogo.GetTitulo() << " Foi instalado com sucesso." << endl;
        };
        void desintallar(string titulo_jogo)
        {
            bool jogo_existe = false;

            for(int i = 0; i < jogos_gratis.size(); i++)
            {
                if((*jogos_gratis[i]).GetTitulo() == titulo_jogo)
                {
                    jogos_gratis.erase(jogos_gratis.begin() + i);
                    total_jogos_instalados--;
                    cout << "O jogo: " << titulo_jogo << " foi desinstalado com sucesso." << endl;
                    jogo_existe = true;
                    break;
                };
            };

            if(jogo_existe == false)
            {
                for(int i = 0; i < jogos_comprados.size(); i++)
                {
                    if((*jogos_comprados[i]).GetTitulo() == titulo_jogo)
                    {
                        jogos_comprados.erase(jogos_comprados.begin() + i);
                        total_jogos_comprados--;
                        cout << "O jogo: " << titulo_jogo << " foi desinstalado com sucesso." << endl;
                        jogo_existe = true;
                        break;
                    };
                };
            };

            if(jogo_existe == false)
            {
                cout << "O jogo: " << titulo_jogo << " não está instalado ou não existe na biblioteca." << endl;
            };
        };

};