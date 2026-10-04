#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "banco_de_dados.hpp"
using namespace std;


// Classe do catálogo
class catalogo
{
    // Lista dos jogos que guarda PONTEIROS para os objetos
    private:
        vector <jogos_gratuitos*> jogos;
        int proximo_id = 1;
        banco_de_dados* banco = nullptr;


    // Operações com o catálogo
    public:
        // Construtor
        catalogo(banco_de_dados* banco_usado = nullptr)
        {
            this->banco = banco_usado;
        };
        // Destrutor
        ~catalogo()
        {
            for(size_t i = 0; i < jogos.size(); i++)
            {
                delete jogos[i];
            };
        };


        // Cadastra o jogo e devolve o id
        int adicionar(jogos_gratuitos* novo_jogo)
        {
            if(buscar_por_titulo(novo_jogo->GetTitulo()) != nullptr)
            {
                cout << "Já existe um jogo com o título: " << novo_jogo->GetTitulo() << endl;
                delete novo_jogo;
                return -1;
            };

            novo_jogo->SetId(proximo_id);
            proximo_id++;
            jogos.push_back(novo_jogo);
            if(banco != nullptr)
            {
                banco->inserir_jogo(*novo_jogo);
            };
            return novo_jogo->GetId();
        };


        // Remove o jogo da loja e do banco
        bool remover(int id_jogo)
        {
            for(size_t i = 0; i < jogos.size(); i++)
            {
                if(jogos[i]->GetId() == id_jogo)
                {
                    cout << "O jogo: " << jogos[i]->GetTitulo() << " foi removido da loja." << endl;
                    delete jogos[i];
                    jogos.erase(jogos.begin() + i);
                    if(banco != nullptr)
                    {
                        banco->remover_jogo(id_jogo);
                    };
                    return true;
                };
            };

            cout << "Não existe jogo com o id: " << id_jogo << endl;
            return false;
        };


        // Buscas
        jogos_gratuitos* buscar_por_id(int id_procurado) const
        {
            for(size_t i = 0; i < jogos.size(); i++)
            {
                if(jogos[i]->GetId() == id_procurado)
                {
                    return jogos[i];
                };
            };
            return nullptr;
        };

        jogos_gratuitos* buscar_por_titulo(string titulo_procurado) const
        {
            for(size_t i = 0; i < jogos.size(); i++)
            {
                if(jogos[i]->GetTitulo() == titulo_procurado)
                {
                    return jogos[i];
                };
            };
            return nullptr;
        };


        // Lista dos jogos da loja
        void listar() const
        {
            cout << "\n--- Catálogo (" << jogos.size() << " jogos) ---" << endl;
            for(size_t i = 0; i < jogos.size(); i++)
            {
                jogos[i]->mostrar_informacoes();
            };
            cout << endl;
        };
};