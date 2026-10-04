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
        vector <jogos_gratuitos*> jogos_gratis; // Listas dos jogos que guardam PONTEIROS para os objetos
        vector <jogos_pagos*> jogos_comprados;
        pix pix_da_biblioteca;


    // Login, saldo e métodos auxiliares
    private:
        string senha_conta;
        long long numero_usuario;
        double saldo = 0;
        vector <cartao_de_credito> cartoes_cadastrados;

        // Retorna a posição do jogo na lista ou -1 se não existir
        int buscar_gratis(const string& titulo) const
        {
            for(size_t i = 0; i < jogos_gratis.size(); i++)
            {
                if(jogos_gratis[i]->GetTitulo() == titulo)
                {
                    return static_cast<int>(i);
                };
            };
            return -1;
        };

        int buscar_comprado(const string& titulo) const
        {
            for(size_t i = 0; i < jogos_comprados.size(); i++)
            {
                if(jogos_comprados[i]->GetTitulo() == titulo)
                {
                    return static_cast<int>(i);
                };
            };
            return -1;
        };

        // Registra o jogo pago na biblioteca depois do pagamento aprovado
        void registrar_compra(jogos_pagos& jogo)
        {
            jogo.comprar();
            jogos_comprados.emplace_back(&jogo);
            total_jogos_comprados++;
        };


    // Operações com o usuario
    public:
        // Construtor do usuario
        usuario()
        {
            this->nome_conta = "usuario";
            this->numero_usuario = 1;
            this->senha_conta = "";
        };

        // Construtor da conta
        usuario(string novo_nick, long long novo_numero, string nova_password)
        {
            this->nome_conta = novo_nick;
            this->numero_usuario = novo_numero;
            this->senha_conta = nova_password;
            cout << "Seja bem vindo a nossa biblioteca de jogos." << endl;
        };

        // Construtor que copia outra conta
        usuario(const usuario &contavalida)
        {
            this->nome_conta = contavalida.nome_conta;
            this->numero_usuario = contavalida.numero_usuario;
            this->senha_conta = contavalida.senha_conta;
            this->saldo = contavalida.saldo;
            this->total_jogos_instalados = contavalida.total_jogos_instalados;
            this->total_jogos_comprados = contavalida.total_jogos_comprados;
            this->jogos_gratis = contavalida.jogos_gratis;
            this->jogos_comprados = contavalida.jogos_comprados;
            this->cartoes_cadastrados = contavalida.cartoes_cadastrados;
            this->pix_da_biblioteca = contavalida.pix_da_biblioteca;
        };


        // A cada bloco temos funções que trabalham em conjunto
        // Nick da conta
        void SetNome(string novo_nick)
        {
            this->nome_conta = novo_nick;
            cout << "Seu nome foi alterado para: " << novo_nick << endl;
        };
        string Getnome() const
        {
            return nome_conta;
        };


        // Senha da conta
        void SetSenha(string nova_senha)
        {
            this->senha_conta = nova_senha;
            cout << "Sua senha foi alterada com sucesso." << endl;
        };
        bool verificar_senha(string tentativa) const
        {
            return tentativa == senha_conta;
        };


        // Depósito via pix
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

        // Gift card recebido por referência
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


        // Cartões cadastrados
        void adicionar_cartao(const cartao_de_credito& novo_cartao)
        {
            cartoes_cadastrados.push_back(novo_cartao);
            cout << "Cartão cadastrado com sucesso." << endl;
        };


        // Compra com o saldo da conta
        bool comprar_jogo(jogos_pagos& jogo)
        {
            if(buscar_comprado(jogo.GetTitulo()) != -1)
            {
                cout << "Você já possui o jogo: " << jogo.GetTitulo() << endl;
                return false;
            };

            if(saldo < jogo.Getvalor())
            {
                cout << "Saldo insuficiente para comprar " << jogo.GetTitulo() << ". Preço: " << jogo.Getvalor() << " | Saldo: " << saldo << endl;
                return false;
            };

            saldo -= jogo.Getvalor();
            registrar_compra(jogo);
            return true;
        };

        // Compra com um cartão cadastrado (indice começa em 0)
        bool comprar_jogo_cartao(jogos_pagos& jogo, int indice_cartao)
        {
            if(indice_cartao < 0 || indice_cartao >= static_cast<int>(cartoes_cadastrados.size()))
            {
                cout << "Cartão não encontrado." << endl;
                return false;
            };

            if(buscar_comprado(jogo.GetTitulo()) != -1)
            {
                cout << "Você já possui o jogo: " << jogo.GetTitulo() << endl;
                return false;
            };

            if(cartoes_cadastrados[indice_cartao].gastar(jogo.Getvalor()) == false)
            {
                return false;
            };

            registrar_compra(jogo);
            return true;
        };


        // Instalar
        void instalar(jogos_gratuitos& jogo)
        {
            jogos_pagos* jogo_pago = dynamic_cast<jogos_pagos*>(&jogo);

            if(jogo_pago != nullptr)
            {
                if(buscar_comprado(jogo.GetTitulo()) != -1)
                {
                    cout << "O jogo: " << jogo.GetTitulo() << " já está na sua biblioteca." << endl;
                    return;
                };

                if(comprar_jogo(*jogo_pago) == false)
                {
                    return;
                };
            }
            else
            {
                if(buscar_gratis(jogo.GetTitulo()) != -1)
                {
                    cout << "O jogo: " << jogo.GetTitulo() << " já está instalado." << endl;
                    return;
                };

                jogos_gratis.emplace_back(&jogo);
                total_jogos_instalados++;
            };

            cout << "O jogo: " << jogo.GetTitulo() << " foi instalado com sucesso." << endl;
        };

        // Desinstalar
        void desinstalar(string titulo_jogo)
        {
            int posicao = buscar_gratis(titulo_jogo);
            if(posicao != -1)
            {
                jogos_gratis.erase(jogos_gratis.begin() + posicao);
                total_jogos_instalados--;
                cout << "O jogo: " << titulo_jogo << " foi desinstalado com sucesso." << endl;
                return;
            };

            posicao = buscar_comprado(titulo_jogo);
            if(posicao != -1)
            {
                jogos_pagos* jogo = jogos_comprados[posicao];
                saldo += jogo->Getvalor();
                jogo->rembolsar();
                jogos_comprados.erase(jogos_comprados.begin() + posicao);
                total_jogos_comprados--;
                cout << "O jogo: " << titulo_jogo << " foi desinstalado e " << jogo->Getvalor() << " reais voltaram ao seu saldo." << endl;
                return;
            };

            cout << "O jogo: " << titulo_jogo << " não está instalado ou não existe na biblioteca." << endl;
        };


        // Lista os jogos da conta
        void mostrar_biblioteca() const
        {
            cout << "\n--- Biblioteca de " << nome_conta << " ---" << endl;
            cout << "Jogos gratuitos (" << total_jogos_instalados << "):" << endl;
            for(size_t i = 0; i < jogos_gratis.size(); i++)
            {
                cout << "  - " << jogos_gratis[i]->GetTitulo() << endl;
            };
            cout << "Jogos comprados (" << total_jogos_comprados << "):" << endl;
            for(size_t i = 0; i < jogos_comprados.size(); i++)
            {
                cout << "  - " << jogos_comprados[i]->GetTitulo() << endl;
            };
            cout << "Saldo: " << saldo << " reais\n" << endl;
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

    // Teste do fluxo
    usuario conta("Fernando", 1, "senha123");
    gift_card card("GIFT-1111", 50);

    conta.depositar_pix(100, "biblioteca-jogos@exemplo.com");
    conta.Cadastrar_GiftCard(card, "GIFT-1111");
    conta.Cadastrar_GiftCard(card, "GIFT-1111"); 

    conta.instalar(Roblox);
    conta.instalar(Roblox);     
    conta.instalar(Gta);       
    conta.instalar(Minecraft); 

    conta.mostrar_biblioteca();

    conta.desinstalar("Minecraft"); 
    conta.mostrar_biblioteca();

    return 0;
}