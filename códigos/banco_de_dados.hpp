#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
using namespace std;


// dados de uma conta como ficam salvos na tabela usuarios
struct dados_conta
{
    int id = 0;
    string nome;
    string senha;
    double saldo = 0;
};


// Classe do banco de dados
class banco_de_dados
{
    private:
        sqlite3* conexao = nullptr;

        // SQL
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

        // le uma coluna de texto e devolve "" se estiver vazia
        string ler_coluna_texto(sqlite3_stmt* comando, int coluna)
        {
            const unsigned char* texto = sqlite3_column_text(comando, coluna);

            if(texto == nullptr)
            {
                return "";
            };

            return string((const char*)texto);
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
                     "preco REAL NOT NULL DEFAULT 0, "
                     "usuario TEXT NOT NULL DEFAULT '', "
                     "senha TEXT NOT NULL DEFAULT '')");

            executar("CREATE TABLE IF NOT EXISTS usuarios ("
                     "id INTEGER PRIMARY KEY, "
                     "nome TEXT UNIQUE NOT NULL, "
                     "senha TEXT NOT NULL, "
                     "saldo REAL NOT NULL DEFAULT 0)");

            executar("CREATE TABLE IF NOT EXISTS biblioteca ("
                     "id_usuario INTEGER NOT NULL, "
                     "id_jogo INTEGER NOT NULL, "
                     "instalado INTEGER NOT NULL DEFAULT 0, "
                     "horas_jogadas INTEGER NOT NULL DEFAULT 0, "
                     "data_compra INTEGER NOT NULL DEFAULT 0, "
                     "PRIMARY KEY (id_usuario, id_jogo))");

            executar("CREATE TABLE IF NOT EXISTS cartoes ("
                     "id_usuario INTEGER NOT NULL, "
                     "numero TEXT NOT NULL, "
                     "cvc INTEGER NOT NULL, "
                     "validade TEXT NOT NULL, "
                     "limite REAL NOT NULL, "
                     "gastos REAL NOT NULL DEFAULT 0)");
        };

        ~banco_de_dados()
        {
            sqlite3_close(conexao);
        };

        banco_de_dados(const banco_de_dados&) = delete;
        banco_de_dados& operator=(const banco_de_dados&) = delete;


        // JOGOS
        // salva o jogo no banco
        bool inserir_jogo(const jogos_gratuitos& jogo)
        {
            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "INSERT INTO jogos (id, titulo, tamanho, preco, usuario, senha) VALUES (?, ?, ?, ?, ?, ?)", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            sqlite3_bind_int(comando, 1, jogo.GetId());
            sqlite3_bind_text(comando, 2, jogo.GetTitulo().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(comando, 3, jogo.GetTamanho());
            sqlite3_bind_double(comando, 4, jogo.GetPreco());
            sqlite3_bind_text(comando, 5, jogo.GetConta().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(comando, 6, jogo.GetSenha().c_str(), -1, SQLITE_TRANSIENT);

            bool sucesso = (sqlite3_step(comando) == SQLITE_DONE);

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "O jogo " << jogo.GetTitulo() << " já está salvo no banco." << endl;
            };
            return sucesso;
        };


        // le todos os jogos salvos e devolve PONTEIROS para objetos criados com new
        vector <jogos_gratuitos*> carregar_jogos()
        {
            vector <jogos_gratuitos*> jogos_salvos;

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "SELECT id, titulo, tamanho, preco, usuario, senha FROM jogos ORDER BY id", -1, &comando, nullptr) != SQLITE_OK)
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
                string usuario = (const char*)sqlite3_column_text(comando, 4);
                string senha = (const char*)sqlite3_column_text(comando, 5);

                jogos_gratuitos* jogo = nullptr;

                if(preco > 0)
                {
                    jogo = new jogos_pagos(titulo, tamanho, senha, preco, usuario);
                }
                else
                {
                    jogo = new jogos_gratuitos(titulo, tamanho, senha, usuario);
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


        // salva a conta, se o id já existe, troca os dados
        bool salvar_conta(int id_conta, const string& nome, const string& senha, double saldo)
        {
            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "INSERT OR REPLACE INTO usuarios (id, nome, senha, saldo) VALUES (?, ?, ?, ?)", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            sqlite3_bind_int(comando, 1, id_conta);
            sqlite3_bind_text(comando, 2, nome.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(comando, 3, senha.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(comando, 4, saldo);

            bool sucesso = (sqlite3_step(comando) == SQLITE_DONE);

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "Não foi possível salvar a conta " << nome << " no banco." << endl;
            };
            return sucesso;
        };


        // salva a biblioteca da conta: apaga os itens antigos e grava a lista atual
        bool salvar_biblioteca(int id_usuario, const vector <item_biblioteca>& itens)
        {
            if(executar("DELETE FROM biblioteca WHERE id_usuario = " + to_string(id_usuario)) == false)
            {
                return false;
            };

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "INSERT INTO biblioteca (id_usuario, id_jogo, instalado, horas_jogadas, data_compra) VALUES (?, ?, ?, ?, ?)", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            bool sucesso = true;

            for(size_t i = 0; i < itens.size(); i++)
            {
                sqlite3_bind_int(comando, 1, id_usuario);
                sqlite3_bind_int(comando, 2, itens[i].id_jogo);
                sqlite3_bind_int(comando, 3, itens[i].instalado ? 1 : 0);
                sqlite3_bind_int(comando, 4, itens[i].horas_jogadas);
                sqlite3_bind_int64(comando, 5, itens[i].data_compra);

                if(sqlite3_step(comando) != SQLITE_DONE)
                {
                    sucesso = false;
                };

                // prepara o mesmo comando para o próximo item
                sqlite3_reset(comando);
            };

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "Não foi possível salvar a biblioteca da conta " << id_usuario << "." << endl;
            };
            return sucesso;
        };


        // salva os cartões da conta
        bool salvar_cartoes(int id_usuario, const vector <cartao_de_credito>& cartoes)
        {
            if(executar("DELETE FROM cartoes WHERE id_usuario = " + to_string(id_usuario)) == false)
            {
                return false;
            };

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "INSERT INTO cartoes (id_usuario, numero, cvc, validade, limite, gastos) VALUES (?, ?, ?, ?, ?, ?)", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return false;
            };

            bool sucesso = true;

            for(size_t i = 0; i < cartoes.size(); i++)
            {
                sqlite3_bind_int(comando, 1, id_usuario);
                sqlite3_bind_text(comando, 2, cartoes[i].GetNumero().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(comando, 3, cartoes[i].GetCvc());
                sqlite3_bind_text(comando, 4, cartoes[i].GetValidade().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_double(comando, 5, cartoes[i].GetLimite());
                sqlite3_bind_double(comando, 6, cartoes[i].GetGastos());

                if(sqlite3_step(comando) != SQLITE_DONE)
                {
                    sucesso = false;
                };

                sqlite3_reset(comando);
            };

            sqlite3_finalize(comando);

            if(sucesso == false)
            {
                cout << "Não foi possível salvar os cartões da conta " << id_usuario << "." << endl;
            };
            return sucesso;
        };


        // lê todas as contas salvas
        vector <dados_conta> carregar_contas()
        {
            vector <dados_conta> contas_salvas;

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "SELECT id, nome, senha, saldo FROM usuarios ORDER BY id", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return contas_salvas;
            };

            while(sqlite3_step(comando) == SQLITE_ROW)
            {
                dados_conta conta;
                conta.id = sqlite3_column_int(comando, 0);
                conta.nome = ler_coluna_texto(comando, 1);
                conta.senha = ler_coluna_texto(comando, 2);
                conta.saldo = sqlite3_column_double(comando, 3);
                contas_salvas.push_back(conta);
            };

            sqlite3_finalize(comando);
            return contas_salvas;
        };


        // lê a biblioteca de uma conta
        vector <item_biblioteca> carregar_biblioteca(int id_usuario)
        {
            vector <item_biblioteca> itens;

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "SELECT id_jogo, instalado, horas_jogadas, data_compra FROM biblioteca WHERE id_usuario = ? ORDER BY rowid", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return itens;
            };

            sqlite3_bind_int(comando, 1, id_usuario);

            while(sqlite3_step(comando) == SQLITE_ROW)
            {
                item_biblioteca item;
                item.id_jogo = sqlite3_column_int(comando, 0);
                item.instalado = (sqlite3_column_int(comando, 1) != 0);
                item.horas_jogadas = sqlite3_column_int(comando, 2);
                item.data_compra = sqlite3_column_int64(comando, 3);
                itens.push_back(item);
            };

            sqlite3_finalize(comando);
            return itens;
        };


        // lê os cartões de uma conta
        vector <cartao_de_credito> carregar_cartoes(int id_usuario)
        {
            vector <cartao_de_credito> cartoes;

            sqlite3_stmt* comando = nullptr;
            if(sqlite3_prepare_v2(conexao, "SELECT numero, cvc, validade, limite, gastos FROM cartoes WHERE id_usuario = ? ORDER BY rowid", -1, &comando, nullptr) != SQLITE_OK)
            {
                cout << "Erro no banco de dados: " << sqlite3_errmsg(conexao) << endl;
                return cartoes;
            };

            sqlite3_bind_int(comando, 1, id_usuario);

            while(sqlite3_step(comando) == SQLITE_ROW)
            {
                string numero = ler_coluna_texto(comando, 0);
                int cvc = sqlite3_column_int(comando, 1);
                string validade = ler_coluna_texto(comando, 2);
                double limite = sqlite3_column_double(comando, 3);
                double gastos = sqlite3_column_double(comando, 4);

                cartoes.push_back(cartao_de_credito(numero, cvc, validade, limite, gastos));
            };

            sqlite3_finalize(comando);
            return cartoes;
        };


        // apaga a conta, a biblioteca e os cartões dela
        bool remover_conta(int id_conta)
        {
            string id_texto = to_string(id_conta);
            bool sucesso = true;

            if(executar("DELETE FROM biblioteca WHERE id_usuario = " + id_texto) == false)
            {
                sucesso = false;
            };

            if(executar("DELETE FROM cartoes WHERE id_usuario = " + id_texto) == false)
            {
                sucesso = false;
            };

            if(executar("DELETE FROM usuarios WHERE id = " + id_texto) == false)
            {
                sucesso = false;
            };

            return sucesso;
        };
};