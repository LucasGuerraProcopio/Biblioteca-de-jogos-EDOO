#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
#include "ins_e_del.hpp"
using namespace std;

// Classe do usuario 
class usuario : public acoes_jogos
{
    // Informações da conta
    protected:
        string nome_conta;


    // Login e saldo da conta
    private:
        string senha_conta;
        long numero_usuário;
        double saldo = 0;
        vector <cartao_de_credito> cartoes_cadastrados;


    // Operações com o usuario
    public:
        // Construtor de uma nova conta
        usuario(string novo_nick, long novo_numero, string nova_password)
        {
            this->nome_conta = novo_nick;
            this->numero_usuário = novo_numero;
            this->senha_conta = nova_password;
            cout << "Seja bem vindo a nossa biblioteca de jogos." << endl;
        };


        // Destrutor para apagar um usuario
        ~usuario()
        {
            cout << "Usuário deletado da biblioteca de jogos." <<  endl;
        };


        // A cada bloco temos funções que trabalham em conjunto
        // Nick da conta
        void SetNome(string novo_nick)
        {
            this->nome_conta = novo_nick;
            cout << "Seu nome foi alterado para: " << novo_nick << endl;
        };
        string Getnome()
        {
            return nome_conta;
        };


        // Senha da conta
        void SetSenha(string nova_senha)
        {
            this->senha_conta = nova_senha;
            cout << "Sua senha foi alterada com sucesso." << endl;
        };
        string GetSenha()
        {
            return senha_conta;
        };


        // Saldo da conta
        void depositar(double valor_deposito)
        {
            this->saldo += valor_deposito;
            cout << "Depósito de: " << valor_deposito << " realizado. Saldo atual: " << saldo << endl;
        };
        double GetSaldo()
        {
            return saldo;
        };


        // Cartões cadastrados
        void adicionar_cartao(cartao_de_credito novo_cartao)
        {
            cartoes_cadastrados.emplace_back(novo_cartao);
            cout << "Cartão cadastrado com sucesso." << endl;
        };

};



int main()
{
    jogos_gratuitos Fortinite("Fortinite", 90, "Torres_tortas", "Embananado123");
    jogos_gratuitos Roblox("Roblox", 5, "29/01/2021", "Tripa Boy");
    jogos_gratuitos FNAF("Five nights at Freddy", 10, "FFCBG123456");
    jogos_gratuitos Brawl("Brawl Stars", 0.8, "HIPERCARGA67", "Piriquito Deuz");
    jogos_pagos R6("Rainbow six siege", 70, "Odeio_escudos123", 50);
    jogos_pagos Minecraft("Minecraft", 35, "I am Steve", 40, "Eduardo Cabral");
    jogos_pagos MK("Mortal Kombat", 80, "Get over here", 200);
    jogos_pagos Fifa("FC 26", 80, "Futebol123", 500);
    jogos_pagos Gta("Grand Theft Auto 6", 220, "Rockstar mercenaria", 550, "CJ");
};