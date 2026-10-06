#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
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
        banco_de_dados* banco = nullptr; // Banco onde os jogos são salvos

        // copia do texto com as letras em minúsculas
        static string para_minusculas(string texto)
        {
            for(size_t i = 0; i < texto.size(); i++)
            {
                texto[i] = (char)tolower((unsigned char)texto[i]);
            };
            return texto;
        };


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

        // não deixa copiar o catálogo, senão os mesmos jogos seriam apagados duas vezes
        catalogo(const catalogo&) = delete;
        catalogo& operator=(const catalogo&) = delete;

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

            if(banco != nullptr)
            {
                // se o banco recusar o jogo, ele não entra na loja
                if(banco->inserir_jogo(*novo_jogo) == false)
                {
                    cout << "O jogo " << novo_jogo->GetTitulo() << " não foi cadastrado." << endl;
                    delete novo_jogo;
                    return -1;
                };

                // guarda o maior id usado, assim o id de um jogo removido nunca é reaproveitado
                banco->salvar_controle("maior_id_jogo", proximo_id);
            };

            proximo_id++;
            jogos.push_back(novo_jogo);
            return novo_jogo->GetId();
        };


        // carrega os jogos salvos no banco e devolve quantos foram carregados
        int carregar_do_banco()
        {
            if(banco == nullptr || jogos.size() > 0)
            {
                return 0;
            };

            vector <jogos_gratuitos*> jogos_salvos = banco->carregar_jogos();

            for(size_t i = 0; i < jogos_salvos.size(); i++)
            {
                jogos.push_back(jogos_salvos[i]);

                if(jogos_salvos[i]->GetId() >= proximo_id)
                {
                    proximo_id = jogos_salvos[i]->GetId() + 1;
                };
            };

            // o id de um jogo removido não volta, mesmo que ele fosse o último
            int maior_id_usado = banco->ler_controle("maior_id_jogo");

            if(maior_id_usado >= proximo_id)
            {
                proximo_id = maior_id_usado + 1;
            };

            banco->salvar_controle("maior_id_jogo", proximo_id - 1);

            return (int)jogos_salvos.size();
        };

        // salva no banco as mudanças feitas no jogo
        bool salvar(jogos_gratuitos* jogo)
        {
            if(jogo == nullptr || banco == nullptr)
            {
                return false;
            };
            return banco->atualizar_jogo(*jogo);
        };

        // remove o jogo da loja e do banco
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


        // devolve os jogos cujo título contém o texto, sem diferenciar maiúsculas de minúsculas
        vector <jogos_gratuitos*> buscar_por_trecho(string trecho) const
        {
            vector <jogos_gratuitos*> encontrados;
            string trecho_minusculo = para_minusculas(trecho);

            for(size_t i = 0; i < jogos.size(); i++)
            {
                if(para_minusculas(jogos[i]->GetTitulo()).find(trecho_minusculo) != string::npos)
                {
                    encontrados.push_back(jogos[i]);
                };
            };
            return encontrados;
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