#pragma once
#include <iostream>
#include <string>
using namespace std;


// Classe dos jogos gratuitos
class jogos_gratuitos
{
    // Informações sobre o jogo
    protected:
        int id = 0;
        string titulo_jogo;
        double tamanho_jogo;


    // login e senha do jogo
    private:
        string senha_jogo;
        string usuario_jogo;


    // Operações dos jogos gratuitos
    public:
        // Construtor dos jogos gratuitos
        jogos_gratuitos()
        {
            this->titulo_jogo = "Jogo sem nome";
            this->tamanho_jogo = 1;
            this->senha_jogo = "";
            this->usuario_jogo = "New_Player123";
        };

        // Construtor do jogo gratuito
        jogos_gratuitos(string titulo_jogo_gratuito, double tamanho_jogo_gratuito, string senha_gratuito, string usuario_gratuito = "Player 1")
        {
            this->titulo_jogo = titulo_jogo_gratuito;
            this->tamanho_jogo = tamanho_jogo_gratuito;
            this->senha_jogo = senha_gratuito;
            this->usuario_jogo = usuario_gratuito;
        };

        // construtor de copia
        jogos_gratuitos(const jogos_gratuitos &jogoantigo)
        {
            this->id = jogoantigo.id;
            this->titulo_jogo = jogoantigo.titulo_jogo;
            this->tamanho_jogo = jogoantigo.tamanho_jogo;
            this->senha_jogo = jogoantigo.senha_jogo;
            this->usuario_jogo = jogoantigo.usuario_jogo;
        };

        // destrutor virtual
        virtual ~jogos_gratuitos() {};

        // comparadores pelo tamanho do jogo
        bool operator<(const jogos_gratuitos &outro) const
        {
            return tamanho_jogo < outro.tamanho_jogo;
        };

        bool operator>(const jogos_gratuitos &outro) const
        {
            return tamanho_jogo > outro.tamanho_jogo;
        };

        bool operator==(const jogos_gratuitos &outro) const
        {
            return tamanho_jogo == outro.tamanho_jogo;
        };


        // Funções virtuais
        virtual double GetPreco() const
        {
            return 0;
        };
        virtual bool EhPago() const
        {
            return false;
        };

        // tipo do jogo como fica salvo na coluna tipo do banco
        string GetTipo() const
        {
            if(EhPago() == true)
            {
                return "pago";
            };
            return "gratuito";
        };


        // A cada bloco temos funções que trabalham em conjunto
        // id do jogo
        void SetId(int novo_id)
        {
            this->id = novo_id;
        };
        int GetId() const
        {
            return id;
        };


        // Titulo do jogo
        void SetTitulo(string titulo_jogo_gratuito)
        {
            this->titulo_jogo = titulo_jogo_gratuito;
        };
        string GetTitulo() const
        {
            return titulo_jogo;
        };


        // Tamanho do jogo
        void SetTamanho(double tamanho_jogo_gratuito)
        {
            this->tamanho_jogo = tamanho_jogo_gratuito;
        };
        double GetTamanho() const
        {
            return tamanho_jogo;
        };


        // Senha do jogo
        void SetSenha(string senha_gratuito)
        {
            this->senha_jogo = senha_gratuito;
        };
        string GetSenha() const
        {
            return senha_jogo;
        };
        bool verificar_senha(string tentativa) const
        {
            return tentativa == senha_jogo;
        };


        // usuario do jogo
        void SetConta(string usuario_gratuito)
        {
            this->usuario_jogo = usuario_gratuito;
        };
        string GetConta() const
        {
            return usuario_jogo;
        };


        // função das informações
        void mostrar_informacoes() const
        {
            cout << "[" << id << "] " << titulo_jogo << " | " << tamanho_jogo << " Gigabytes | ";

            if(EhPago() == true)
            {
                cout << "R$ " << GetPreco() << endl;
            }
            else
            {
                cout << "Gratuito" << endl;
            };
        };
};


// Classe dos jogos pagos que herda da classe dos jogos gratuitos
class jogos_pagos : public jogos_gratuitos
{
    // Informações do preço do jogo pago
    private:
        double valor_jogo;


    // Operações com os jogos pagos
    public:
        // construtor
        jogos_pagos(string titulo_jogo_pago, double tamanho_jogo_pago, string senha_pago, double preco_jogo, string usuario_pago = "Player 1") : jogos_gratuitos(titulo_jogo_pago, tamanho_jogo_pago, senha_pago, usuario_pago)
        {
            this->valor_jogo = preco_jogo;
        };

        // destrutor
        ~jogos_pagos() override {};

        //jogo pago troca o resultado das funções virtuais
        double GetPreco() const override
        {
            return valor_jogo;
        };
        bool EhPago() const override
        {
            return true;
        };

        
        // valor do jogo
        void SetValor(double preco_jogo)
        {
            this->valor_jogo = preco_jogo;
        };
        double Getvalor() const
        {
            return valor_jogo;
        };
};


// jogos da biblioteca de UM usuario
struct item_biblioteca
{
    int id_jogo = 0;
    bool instalado = false;
    int horas_jogadas = 0;
    long long data_compra = 0;
};