#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


// Classe do cartão de crédito
class cartao_de_credito
{
    // informações do cartão
    private:
        string numero_credito;
        int cvc_credito;
        string validade_credito;
        double limite_credito;
        double gastos_credito = 0;


    // Operações com o cartão de crédito
    public:
        // construtor do novo cartão de crédito
        cartao_de_credito(string numero_credito_novo, int cvc_credito_novo, string validade_credito_novo, double limite_credito_novo, double gastos_credito_novo = 0)
        {
            this->numero_credito = numero_credito_novo;
            this->cvc_credito = cvc_credito_novo;
            this->validade_credito = validade_credito_novo;
            this->limite_credito = limite_credito_novo;
            this->gastos_credito = gastos_credito_novo;
        };


        // Verifica se é possível realizar a compra
        bool gastar(double valor_compra)
        {
            if(valor_compra <= 0)
            {
                cout << "Valor de compra inválido." << endl;
                return false;
            };

            if((gastos_credito + valor_compra) > limite_credito)
            {
                cout << "Você ultrapassou seu limite, compra negada." << endl;
                return false;
            };

            gastos_credito += valor_compra;
            cout << "Compra aprovada no valor de: " << valor_compra << "\nGastos totais com esse cartão na biblioteca de jogos: " << gastos_credito << endl;
            return true;
        };

        // devolve o valor pro cartão
        void estornar(double valor_estorno)
        {
            if(valor_estorno <= 0)
            {
                return;
            };

            gastos_credito -= valor_estorno;

            if(gastos_credito < 0)
            {
                gastos_credito = 0;
            };

            cout << "Estorno de " << valor_estorno << " reais no cartão. Gastos totais: " << gastos_credito << endl;
        };

        string GetNumero() const
        {
            return numero_credito;
        };

        int GetCvc() const
        {
            return cvc_credito;
        };

        string GetValidade() const
        {
            return validade_credito;
        };

        double GetGastos() const
        {
            return gastos_credito;
        };

        double GetLimite() const
        {
            return limite_credito;
        };
};


// classe das formas de pagamento
class forma_de_pagamento
{
    public:
        virtual ~forma_de_pagamento() {};

        virtual bool pagar(double valor) = 0;

        // devolve o valor para onde ele saiu
        virtual void estornar(double valor) = 0;

        // código guardado na biblioteca
        virtual int GetCodigo() const = 0;

        // mensagem de reembolso
        virtual string GetDestino() const = 0;

        // posição do cartão na lista de cartões
        virtual int GetIndiceCartao() const
        {
            return -1;
        };
};


// pagar com o saldo da conta
class pagamento_saldo : public forma_de_pagamento
{
    private:
        double& saldo_conta; // mexe direto no saldo do usuário

    public:
        pagamento_saldo(double& saldo_da_conta) : saldo_conta(saldo_da_conta)
        {
        };

        bool pagar(double valor) override
        {
            if(valor <= 0)
            {
                cout << "Valor de compra inválido." << endl;
                return false;
            };

            if(saldo_conta < valor)
            {
                cout << "Saldo insuficiente. Valor: " << valor << " | Saldo: " << saldo_conta << endl;
                return false;
            };

            saldo_conta -= valor;
            return true;
        };

        void estornar(double valor) override
        {
            saldo_conta += valor;
        };

        int GetCodigo() const override
        {
            return 1;
        };

        string GetDestino() const override
        {
            return "ao seu saldo";
        };
};


// pagar com um cartão de crédito cadastrado
class pagamento_cartao : public forma_de_pagamento
{
    private:
        cartao_de_credito& cartao; // mexe direto no cartão do usuário
        int indice_cartao;

    public:
        pagamento_cartao(cartao_de_credito& cartao_usado, int indice) : cartao(cartao_usado), indice_cartao(indice)
        {
        };

        bool pagar(double valor) override
        {
            return cartao.gastar(valor);
        };

        void estornar(double valor) override
        {
            cartao.estornar(valor);
        };

        int GetCodigo() const override
        {
            return 2;
        };

        string GetDestino() const override
        {
            return "ao cartão";
        };

        int GetIndiceCartao() const override
        {
            return indice_cartao;
        };
};


// Classe do pix
class pix
{
    private:
        string chave_pix;
        double taxa_da_biblioteca;

    public:
        pix(string chave = "gcromeiro@gmail.com", double taxa = 0.02)
        {
            this->chave_pix = chave;
            this->taxa_da_biblioteca = taxa;
        };

        string GetChave() const
        {
            return chave_pix;
        };

        void SetTaxa(double nova_taxa)
        {
            if(nova_taxa < 0 || nova_taxa > 1)
            {
                cout << "Taxa inválida. Use um valor entre 0 e 1." << endl;
                return;
            };
            this->taxa_da_biblioteca = nova_taxa;
        };

        double GetTaxa() const
        {
            return taxa_da_biblioteca;
        };
};


// Classe do gift card
class gift_card
{
    private:
        string codigo;
        double valor;
        bool valido = true;

    public:
        gift_card()
        {
            this->codigo = "1234 5678 9000";
            this->valor = 100.00;
        };

        gift_card(string numero, double dinheiro)
        {
            this->codigo = numero;
            this->valor = dinheiro;
        };

        string GetCodigo() const
        {
            return codigo;
        };

        double GetValor() const
        {
            return valor;
        };

        bool EstaValido() const
        {
            return valido;
        };

        void GiftCardRegistrado()
        {
            this->valido = false;
        };
};