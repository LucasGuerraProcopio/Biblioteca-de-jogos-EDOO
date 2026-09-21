#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
using namespace std;

// Classe do usuario 
class usuario 
{
    // Informações da conta
    protected:
        string nome_conta;
        int total_jogos_instalados = 0; 
        int total_jogos_comprados = 0;
        vector <jogos_gratuitos*> jogos_gratis; // Listas dos jogos que guarda PONTEIROS para os objetos
        vector <jogos_pagos*> jogos_comprados;
        pix pix_da_biblioteca;


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


        // Função para depositar dinheiro, vai haver outra de deposito que funcione cartão, mas esse metodo não vai aceitar cartão.
        void depositar_pix(double valor, string codigo){
            if(codigo == pix_da_biblioteca.GetChave()){
                this->saldo += valor;
                cout << "Um depósito no valor de: " << valor << " reais foi feito na sua conta via pix" << endl;
            }
            else{
                cout << "Chave pix inválida";
            };
            };

        void Cadastrar_GiftCard(gift_card novo_card, string codigo){
            if(novo_card.GetCodigo() == codigo){
                saldo += novo_card.GetValor();
                novo_card.GiftCardRegistrado();                
            }
            else{
                cout << "Codigo de Gift card inválido" << endl;
            };
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
                cout << "O jogo: " << titulo_jogo << " não está instalado ou não existe na biblioteca." << endl;
            };
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