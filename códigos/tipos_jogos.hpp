#pragma once
#include <iostream>
#include <string>
using namespace std;


// Classe de todos os jogos
class jogo_base
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


    // Operações comuns a todos os jogos
    public:
        // Construtor padrão
        jogo_base()
        {
            this->titulo_jogo = "Jogo sem nome";
            this->tamanho_jogo = 1;
            this->senha_jogo = "";
            this->usuario_jogo = "New_Player123";
        };

        // Construtor com os dados do jogo
        jogo_base(string novo_titulo, double novo_tamanho, string nova_senha, string novo_usuario = "Player 1")
        {
            this->titulo_jogo = novo_titulo;
            this->tamanho_jogo = novo_tamanho;
            this->senha_jogo = nova_senha;
            this->usuario_jogo = novo_usuario;
        };

        // construtor de copia
        jogo_base(const jogo_base &jogoantigo)
        {
            this->id = jogoantigo.id;
            this->titulo_jogo = jogoantigo.titulo_jogo;
            this->tamanho_jogo = jogoantigo.tamanho_jogo;
            this->senha_jogo = jogoantigo.senha_jogo;
            this->usuario_jogo = jogoantigo.usuario_jogo;
        };

        // destrutor virtual
        virtual ~jogo_base() {};

        // comparam pelo tamanho do jogo
        bool operator<(const jogo_base &outro) const
        {
            return tamanho_jogo < outro.tamanho_jogo;
        };

        bool operator>(const jogo_base &outro) const
        {
            return tamanho_jogo > outro.tamanho_jogo;
        };

        bool operator==(const jogo_base &outro) const
        {
            return titulo_jogo == outro.titulo_jogo;
        };


        // Funções virtuais
        virtual double GetPreco() const = 0;
        virtual bool EhPago() const = 0;

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


// Classe dos jogos gratuitos
class jogos_gratuitos : public jogo_base
{
    public:
        // construtor
        jogos_gratuitos(string titulo_jogo_gratuito, double tamanho_jogo_gratuito, string senha_gratuito, string usuario_gratuito = "Player 1") : jogo_base(titulo_jogo_gratuito, tamanho_jogo_gratuito, senha_gratuito, usuario_gratuito)
        {
        };

        // destrutor
        ~jogos_gratuitos() override {};

        // jogo gratuito não tem preço
        double GetPreco() const override
        {
            return 0;
        };
        bool EhPago() const override
        {
            return false;
        };
};


// Classe dos jogos pagos, também herda da classe base
class jogos_pagos : public jogo_base
{
    // Informações do preço do jogo pago
    private:
        double valor_jogo;


    // Operações com os jogos pagos
    public:
        // construtor
        jogos_pagos(string titulo_jogo_pago, double tamanho_jogo_pago, string senha_pago, double preco_jogo, string usuario_pago = "Player 1") : jogo_base(titulo_jogo_pago, tamanho_jogo_pago, senha_pago, usuario_pago)
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


// jogos da biblioteca do usuario
struct item_biblioteca
{
    int id_jogo = 0;
    bool instalado = false;
    int horas_jogadas = 0;
    long long data_compra = 0;
    int forma_pagamento = 0;
    double valor_pago = 0;
    int cartao_pago = -1;
};