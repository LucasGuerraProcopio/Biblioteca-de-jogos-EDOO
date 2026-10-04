#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "tipos_jogos.hpp"
using namespace std;


// Classe do banco de dados
class banco_de_dados
{
    private:
        sqlite3* conexao = nullptr;

        // Roda um SQL
        bool executar(const string& comando)
        {
            char* erro = nullptr;
            if(sqlite3_exec(conexao, comando.c_str(), nullptr, nullptr, &erro) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << erro << endl;
                sqlite3_free(erro);
                return false;
            };
            return true;
        };


    public:
        banco_de_dados(const string& arquivo)
        {
            if(sqlite3_open(arquivo.c_str(), &conexao) != SQLITE_OK)
            {
                cout << "Não foi possível abrir o banco: " << sqlite3_errmsg(conexao) << endl;
                return;
            };

            executar("CREATE TABLE IF NOT EXISTS jogos ("
                     "id INTEGER PRIMARY KEY, "
                     "titulo TEXT UNIQUE NOT NULL, "
                     "tamanho REAL NOT NULL, "
                     "preco REAL NOT NULL DEFAULT 0)");
        };

        ~banco_de_dados()
        {
            sqlite3_close(conexao);
        };

        banco_de_dados(const banco_de_dados&) = delete;
        banco_de_dados& operator=(const banco_de_dados&) = delete;


        // CREATE: salva o jogo no banco
        bool inserir_jogo(const jogos_gratuitos& jogo)
        {
            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "INSERT INTO jogos (id, titulo, tamanho, preco) VALUES (?, ?, ?, ?)", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            sqlite3_bind_int(comando, 1, jogo.GetId());
            sqlite3_bind_text(comando, 2, jogo.GetTitulo().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(comando, 3, jogo.GetTamanho());
            sqlite3_bind_double(comando, 4, jogo.GetPreco());

            bool sucesso = (sqlite3_step(comando) == SQLITE_DONE);

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "O jogo " << jogo.GetTitulo() << " já está salvo no banco." << endl;
            };
            return sucesso;
        };


        // lê todos os jogos salvos e devolve PONTEIROS para objetos criados com new
        vector <jogos_gratuitos*> carregar_jogos()
        {
            vector <jogos_gratuitos*> jogos_salvos;

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "SELECT id, titulo, tamanho, preco FROM jogos ORDER BY id", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return jogos_salvos;
            };

            while(sqlite3_step(comando) == SQLITE_ROW)
            {
                int id_jogo = sqlite3_column_int(comando, 0);
                string titulo = (const char*)sqlite3_column_text(comando, 1);
                double tamanho = sqlite3_column_double(comando, 2);
                double preco = sqlite3_column_double(comando, 3);

                jogos_gratuitos* jogo = nullptr;

                if(preco > 0)
                {
                    jogo = new jogos_pagos(titulo, tamanho, "", preco);
                }
                else
                {
                    jogo = new jogos_gratuitos(titulo, tamanho, "");
                };

                jogo->SetId(id_jogo);
                jogos_salvos.push_back(jogo);
            };

            sqlite3_finalize(comando);
            return jogos_salvos;
        };


        // apaga o jogo do banco pelo id
        bool remover_jogo(int id_jogo)
        {
            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "DELETE FROM jogos WHERE id = ?", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            sqlite3_bind_int(comando, 1, id_jogo);

            bool sucesso = (sqlite3_step(comando) == SQLITE_DONE);

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "Não foi possível apagar o jogo " << id_jogo << " do banco." << endl;
            };
            return sucesso;
        };
};