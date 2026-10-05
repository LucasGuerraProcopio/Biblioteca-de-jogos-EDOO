#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
#include "catalogo.hpp"
using namespace std;


// jogos da biblioteca do usuario
struct item_biblioteca
{
    int id_jogo = 0;
    bool instalado = false;
    int horas_jogadas = 0;
    long long data_compra = 0;
};


// Classe do usuario
class usuario
{
    // informações da conta
    protected:
        int id = 0;
        string nome_conta;
        vector <item_biblioteca> biblioteca;
        pix pix_da_biblioteca;


    // login, saldo e métodos auxiliares
    private:
        string senha_conta;
        double saldo = 0;
        vector <cartao_de_credito> cartoes_cadastrados;

        // endereço do item na biblioteca ou nullptr se o usuario não tem o jogo
        item_biblioteca* buscar_item(int id_jogo)
        {
            for(size_t i = 0; i < biblioteca.size(); i++)
            {
                if(biblioteca[i].id_jogo == id_jogo)
                {
                    return &biblioteca[i];
                };
            };
            return nullptr;
        };

        // coloca o jogo na biblioteca depois do pagamento aprovado
        void registrar_item(int id_jogo)
        {
            item_biblioteca novo_item;
            novo_item.id_jogo = id_jogo;
            novo_item.data_compra = time(nullptr);
            biblioteca.push_back(novo_item);
        };


    // operações com o usuario
    public:
        // construtor do usuario
        usuario()
        {
            this->nome_conta = "usuario";
            this->senha_conta = "";
        };

        // construtor da conta
        usuario(string novo_nick, string nova_password)
        {
            this->nome_conta = novo_nick;
            this->senha_conta = nova_password;
        };


        // a cada bloco temos funções que trabalham em conjunto
        // id da conta
        void SetId(int novo_id)
        {
            this->id = novo_id;
        };
        int GetId() const
        {
            return id;
        };


        // nick da conta
        void SetNome(string novo_nick)
        {
            this->nome_conta = novo_nick;
            cout << "Seu nome foi alterado para: " << novo_nick << endl;
        };
        string Getnome() const
        {
            return nome_conta;
        };


        // senha da conta
        void SetSenha(string nova_senha)
        {
            this->senha_conta = nova_senha;
            cout << "Sua senha foi alterada com sucesso." << endl;
        };
        bool verificar_senha(string tentativa) const
        {
            return tentativa == senha_conta;
        };


        // deposito via pix
        void depositar_pix(double valor, string codigo)
        {
            if(valor <= 0)
            {
                cout << "Valor de depósito inválido." << endl;
                return;
            };

            if(codigo != pix_da_biblioteca.GetChave())
            {
                cout << "Chave pix inválida." << endl;
                return;
            };

            double taxa = valor * pix_da_biblioteca.GetTaxa();
            double liquido = valor - taxa;
            saldo += liquido;
            cout << "Depósito de " << valor << " reais via pix. Taxa: " << taxa << ". Creditado: " << liquido << " reais." << endl;
        };

        // gift card recebido por referencia, para marcar o gift card original como usado
        void Cadastrar_GiftCard(gift_card& novo_card, string codigo)
        {
            if(novo_card.EstaValido() == false)
            {
                cout << "Esse gift card já foi utilizado." << endl;
                return;
            };

            if(novo_card.GetCodigo() != codigo)
            {
                cout << "Código de gift card inválido." << endl;
                return;
            };

            saldo += novo_card.GetValor();
            novo_card.GiftCardRegistrado();
            cout << "Gift card registrado. Valor creditado: " << novo_card.GetValor() << " reais." << endl;
        };

        double GetSaldo() const
        {
            return saldo;
        };


        // cartões cadastrados
        void adicionar_cartao(const cartao_de_credito& novo_cartao)
        {
            cartoes_cadastrados.push_back(novo_cartao);
            cout << "Cartão cadastrado com sucesso." << endl;
        };


        // verifica se o usuario tem o jogo na biblioteca
        bool possui(int id_jogo) const
        {
            for(size_t i = 0; i < biblioteca.size(); i++)
            {
                if(biblioteca[i].id_jogo == id_jogo)
                {
                    return true;
                };
            };
            return false;
        };


        // adquirir o jogo
        bool adquirir(const jogos_gratuitos& jogo)
        {
            if(possui(jogo.GetId()) == true)
            {
                cout << "Você já possui o jogo: " << jogo.GetTitulo() << endl;
                return false;
            };

            if(jogo.EhPago() == true)
            {
                if(saldo < jogo.GetPreco())
                {
                    cout << "Saldo insuficiente para comprar " << jogo.GetTitulo() << ". Preço: " << jogo.GetPreco() << " | Saldo: " << saldo << endl;
                    return false;
                };

                saldo -= jogo.GetPreco();
            };

            registrar_item(jogo.GetId());
            cout << "O jogo: " << jogo.GetTitulo() << " foi adicionado à sua biblioteca." << endl;
            return true;
        };

        // adquirir o jogo pago com um cartão cadastrado
        bool adquirir_com_cartao(const jogos_gratuitos& jogo, int indice_cartao)
        {
            if(jogo.EhPago() == false)
            {
                return adquirir(jogo);
            };

            if(indice_cartao < 0 || indice_cartao >= (int)cartoes_cadastrados.size())
            {
                cout << "Cartão não encontrado." << endl;
                return false;
            };

            if(possui(jogo.GetId()) == true)
            {
                cout << "Você já possui o jogo: " << jogo.GetTitulo() << endl;
                return false;
            };

            if(cartoes_cadastrados[indice_cartao].gastar(jogo.GetPreco()) == false)
            {
                return false;
            };

            registrar_item(jogo.GetId());
            cout << "O jogo: " << jogo.GetTitulo() << " foi adicionado à sua biblioteca." << endl;
            return true;
        };


        // instalar e desinstalar
        bool instalar(int id_jogo)
        {
            item_biblioteca* item = buscar_item(id_jogo);

            if(item == nullptr)
            {
                cout << "Esse jogo não está na sua biblioteca." << endl;
                return false;
            };

            if(item->instalado == true)
            {
                cout << "Esse jogo já está instalado." << endl;
                return false;
            };

            item->instalado = true;
            cout << "Jogo instalado com sucesso." << endl;
            return true;
        };

        bool desinstalar(int id_jogo)
        {
            item_biblioteca* item = buscar_item(id_jogo);

            if(item == nullptr || item->instalado == false)
            {
                cout << "Esse jogo não está instalado." << endl;
                return false;
            };

            item->instalado = false;
            cout << "Jogo desinstalado. Ele continua na sua biblioteca." << endl;
            return true;
        };


        // reembolso
        bool reembolsar(const jogos_gratuitos& jogo)
        {
            if(jogo.EhPago() == false)
            {
                cout << "Jogo gratuito não tem reembolso." << endl;
                return false;
            };

            for(size_t i = 0; i < biblioteca.size(); i++)
            {
                if(biblioteca[i].id_jogo == jogo.GetId())
                {
                    biblioteca.erase(biblioteca.begin() + i);
                    saldo += jogo.GetPreco();
                    cout << "O jogo: " << jogo.GetTitulo() << " foi reembolsado e " << jogo.GetPreco() << " reais voltaram ao seu saldo." << endl;
                    return true;
                };
            };

            cout << "Você não possui esse jogo: " << jogo.GetTitulo() << endl;
            return false;
        };


        // lista os jogos da conta
        void mostrar_biblioteca(const catalogo& loja) const
        {
            cout << "\n--- Biblioteca de " << nome_conta << " (id " << id << ") ---" << endl;

            for(size_t i = 0; i < biblioteca.size(); i++)
            {
                jogos_gratuitos* jogo = loja.buscar_por_id(biblioteca[i].id_jogo);

                cout << "  - [" << biblioteca[i].id_jogo << "] ";

                if(jogo != nullptr)
                {
                    cout << jogo->GetTitulo();
                }
                else
                {
                    cout << "(jogo removido)";
                };

                if(biblioteca[i].instalado == true)
                {
                    cout << " | instalado";
                }
                else
                {
                    cout << " | não instalado";
                };

                cout << " | " << biblioteca[i].horas_jogadas << " horas" << endl;
            };

            cout << "Saldo: " << saldo << " reais\n" << endl;
        };
};


// Classe do repositório
class repositorio_usuarios
{
    // Lista das contas que guarda PONTEIROS para os objetos criados com new
    private:
        vector <usuario*> contas;
        int proximo_id = 1;


    // operações com o repositorio
    public:
        // destrutor
        ~repositorio_usuarios()
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                delete contas[i];
            };
        };


        // cria a conta e devolve o endereço dela
        usuario* criar(string nome, string senha)
        {
            if(buscar_por_nome(nome) != nullptr)
            {
                cout << "Já existe uma conta com o nome: " << nome << endl;
                return nullptr;
            };

            usuario* nova_conta = new usuario(nome, senha);
            nova_conta->SetId(proximo_id);
            proximo_id++;
            contas.push_back(nova_conta);
            return nova_conta;
        };


        // busca, devolvem nullptr se não encontrar
        usuario* buscar_por_id(int id_procurado) const
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                if(contas[i]->GetId() == id_procurado)
                {
                    return contas[i];
                };
            };
            return nullptr;
        };

        usuario* buscar_por_nome(string nome_procurado) const
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                if(contas[i]->Getnome() == nome_procurado)
                {
                    return contas[i];
                };
            };
            return nullptr;
        };


        // altera o nome da conta, recusa se outra conta já usa o nome
        bool renomear(int id_conta, string novo_nome)
        {
            usuario* conta = buscar_por_id(id_conta);

            if(conta == nullptr)
            {
                cout << "Não existe conta com o id: " << id_conta << endl;
                return false;
            };

            usuario* outra_conta = buscar_por_nome(novo_nome);

            if(outra_conta != nullptr && outra_conta != conta)
            {
                cout << "Já existe uma conta com o nome: " << novo_nome << endl;
                return false;
            };

            conta->SetNome(novo_nome);
            return true;
        };


        // verifica se alguma conta tem o jogo na biblioteca
        bool alguem_possui(int id_jogo) const
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                if(contas[i]->possui(id_jogo) == true)
                {
                    return true;
                };
            };
            return false;
        };


        // delete da conta
        bool remover(int id_conta)
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                if(contas[i]->GetId() == id_conta)
                {
                    cout << "A conta: " << contas[i]->Getnome() << " foi removida." << endl;
                    delete contas[i];
                    contas.erase(contas.begin() + i);
                    return true;
                };
            };

            cout << "Não existe conta com o id: " << id_conta << endl;
            return false;
        };


        // delete do jogo da loja
        bool excluir_jogo(catalogo& loja, int id_jogo) const
        {
            jogos_gratuitos* jogo = loja.buscar_por_id(id_jogo);

            if(jogo == nullptr)
            {
                cout << "Não existe jogo com o id: " << id_jogo << endl;
                return false;
            };

            if(alguem_possui(id_jogo) == true)
            {
                cout << "Não foi possível remover " << jogo->GetTitulo() << ": existe conta que possui o jogo." << endl;
                return false;
            };

            return loja.remover(id_jogo);
        };


        // Lista as contas
        void listar() const
        {
            cout << "\n--- Usuários (" << contas.size() << ") ---" << endl;
            for(size_t i = 0; i < contas.size(); i++)
            {
                cout << "[" << contas[i]->GetId() << "] " << contas[i]->Getnome() << endl;
            };
            cout << endl;
        };
};