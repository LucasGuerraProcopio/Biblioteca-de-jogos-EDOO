#pragma once
#include <iostream>
#include <string>
#include "sqlite3.h"
#include "tipos_jogos.hpp"
using namespace std;


// Classe do banco de dados (SQLite)
class banco_de_dados
{
    private:
        sqlite3* conexao = nullptr;

        // Roda um comando SQL que não precisa de valores
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

            // O id é o mesmo do catálogo e jogo gratuito fica com preço 0
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

        // Não deixa copiar o banco, senão a conexão seria fechada duas vezes
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
};
