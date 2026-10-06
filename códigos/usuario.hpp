#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
#include "catalogo.hpp"
#include "banco_de_dados.hpp"
#include "seguranca.hpp"
using namespace std;


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
        string senha_conta; // guarda o hash da senha, nunca a senha em texto puro
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
        // guarda como e quanto foi pago, para o reembolso devolver certo
        void registrar_item(int id_jogo, int forma, double valor, int indice_cartao)
        {
            item_biblioteca novo_item;
            novo_item.id_jogo = id_jogo;
            novo_item.data_compra = time(nullptr);
            novo_item.forma_pagamento = forma;
            novo_item.valor_pago = valor;
            novo_item.cartao_pago = indice_cartao;
            biblioteca.push_back(novo_item);
        };


    // operações com o usuario
    public:
        // construtor do usuario
        usuario()
        {
            this->nome_conta = "usuario";
            this->senha_conta = gerar_hash("");
        };

        // construtor da conta, recebe a senha em texto e guarda só o hash
        usuario(string novo_nick, string nova_password)
        {
            this->nome_conta = novo_nick;
            this->senha_conta = gerar_hash(nova_password);
        };


        // A cada bloco temos funções que trabalham em conjunto
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
            this->senha_conta = gerar_hash(nova_senha);
            cout << "Sua senha foi alterada com sucesso." << endl;
        };
        // devolve o hash da senha (é ele que vai para o banco)
        string GetSenha() const
        {
            return senha_conta;
        };
        bool verificar_senha(string tentativa) const
        {
            return gerar_hash(tentativa) == senha_conta;
        };


        // leitura da biblioteca e dos cartões, usada para salvar no banco
        const vector <item_biblioteca>& GetBiblioteca() const
        {
            return biblioteca;
        };
        const vector <cartao_de_credito>& GetCartoes() const
        {
            return cartoes_cadastrados;
        };


        // restauram os dados carregados do banco, sem mensagens na tela
        // recebe a senha como veio do banco, devolve true se era uma senha antiga em texto puro
        // (nesse caso ela é convertida para hash e a conta precisa ser salva de novo)
        bool RestaurarSenha(const string& senha_salva)
        {
            if(eh_hash(senha_salva) == true)
            {
                this->senha_conta = senha_salva;
                return false;
            };

            this->senha_conta = gerar_hash(senha_salva);
            return true;
        };
        void RestaurarSaldo(double saldo_salvo)
        {
            this->saldo = saldo_salvo;
        };
        void RestaurarItem(const item_biblioteca& item_salvo)
        {
            biblioteca.push_back(item_salvo);
        };
        void RestaurarCartao(const cartao_de_credito& cartao_salvo)
        {
            cartoes_cadastrados.push_back(cartao_salvo);
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
        bool adquirir(const jogo_base& jogo)
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

            // forma 1 = saldo, forma 0 = jogo gratuito (não pagou nada)
            if(jogo.EhPago() == true)
            {
                registrar_item(jogo.GetId(), 1, jogo.GetPreco(), -1);
            }
            else
            {
                registrar_item(jogo.GetId(), 0, 0, -1);
            };

            cout << "O jogo: " << jogo.GetTitulo() << " foi adicionado à sua biblioteca." << endl;
            return true;
        };

        // adquirir o jogo pago com um cartão cadastrado
        bool adquirir_com_cartao(const jogo_base& jogo, int indice_cartao)
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

            // forma 2 = cartão, guarda qual cartão foi usado
            registrar_item(jogo.GetId(), 2, jogo.GetPreco(), indice_cartao);
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


        // registra horas jogadas, o jogo precisa estar instalado
        bool jogar(int id_jogo, int horas)
        {
            item_biblioteca* item = buscar_item(id_jogo);

            if(item == nullptr)
            {
                cout << "Esse jogo não está na sua biblioteca." << endl;
                return false;
            };

            if(item->instalado == false)
            {
                cout << "Instale o jogo antes de jogar." << endl;
                return false;
            };

            if(horas <= 0)
            {
                cout << "Quantidade de horas inválida." << endl;
                return false;
            };

            item->horas_jogadas += horas;
            cout << "Você jogou " << horas << " hora(s). Total no jogo: " << item->horas_jogadas << " horas." << endl;
            return true;
        };


        // reembolso: devolve o valor que foi pago, no saldo ou no cartão usado na compra
        bool reembolsar(const jogo_base& jogo)
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
                    item_biblioteca item = biblioteca[i];

                    // compras antigas não guardaram o valor pago, então usa o preço atual
                    double valor = item.valor_pago;
                    if(valor <= 0)
                    {
                        valor = jogo.GetPreco();
                    };

                    biblioteca.erase(biblioteca.begin() + i);

                    bool cartao_valido = (item.cartao_pago >= 0 && item.cartao_pago < (int)cartoes_cadastrados.size());

                    if(item.forma_pagamento == 2 && cartao_valido == true)
                    {
                        // a compra foi no cartão: o dinheiro volta para o cartão, não para o saldo
                        cartoes_cadastrados[item.cartao_pago].estornar(valor);
                        cout << "O jogo: " << jogo.GetTitulo() << " foi reembolsado e " << valor << " reais voltaram ao cartão." << endl;
                    }
                    else
                    {
                        saldo += valor;
                        cout << "O jogo: " << jogo.GetTitulo() << " foi reembolsado e " << valor << " reais voltaram ao seu saldo." << endl;
                    };
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
                jogo_base* jogo = loja.buscar_por_id(biblioteca[i].id_jogo);

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

                cout << " | " << biblioteca[i].horas_jogadas << " horas";

                // data da compra no formato dd/mm/aaaa
                if(biblioteca[i].data_compra > 0)
                {
                    time_t momento = (time_t)biblioteca[i].data_compra;
                    struct tm* data = localtime(&momento);

                    if(data != nullptr)
                    {
                        char texto_data[20];
                        strftime(texto_data, sizeof(texto_data), "%d/%m/%Y", data);
                        cout << " | adquirido em " << texto_data;
                    };
                };

                cout << endl;
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
        banco_de_dados* banco = nullptr;


    // operações com o repositorio
    public:
        // construtor
        repositorio_usuarios(banco_de_dados* banco_usado = nullptr)
        {
            this->banco = banco_usado;
        };

        // destrutor
        ~repositorio_usuarios()
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                delete contas[i];
            };
        };

        // não deixa copiar o repositório, senão as mesmas contas seriam apagadas duas vezes
        repositorio_usuarios(const repositorio_usuarios&) = delete;
        repositorio_usuarios& operator=(const repositorio_usuarios&) = delete;

        // salva a conta, a biblioteca e os cartões no banco
        // tudo em uma transação: se alguma parte falhar, nada é alterado no banco
        bool salvar(usuario* conta)
        {
            if(banco == nullptr || conta == nullptr)
            {
                return false;
            };

            if(banco->iniciar_transacao() == false)
            {
                return false;
            };

            bool sucesso = banco->salvar_conta(conta->GetId(), conta->Getnome(), conta->GetSenha(), conta->GetSaldo())
                        && banco->salvar_biblioteca(conta->GetId(), conta->GetBiblioteca())
                        && banco->salvar_cartoes(conta->GetId(), conta->GetCartoes());

            if(sucesso == true)
            {
                return banco->confirmar_transacao();
            };

            banco->cancelar_transacao();
            cout << "Os dados da conta " << conta->Getnome() << " não foram salvos." << endl;
            return false;
        };


        // carrega as contas salvas no banco e devolve quantas foram carregadas
        int carregar_do_banco()
        {
            if(banco == nullptr || contas.size() > 0)
            {
                return 0;
            };

            vector <dados_conta> contas_salvas = banco->carregar_contas();

            for(size_t i = 0; i < contas_salvas.size(); i++)
            {
                usuario* conta = new usuario(contas_salvas[i].nome, "");
                conta->SetId(contas_salvas[i].id);
                conta->RestaurarSaldo(contas_salvas[i].saldo);

                // senha antiga em texto puro vira hash e a conta é salva de novo
                bool regravar = conta->RestaurarSenha(contas_salvas[i].senha);

                vector <item_biblioteca> itens = banco->carregar_biblioteca(contas_salvas[i].id);
                for(size_t j = 0; j < itens.size(); j++)
                {
                    conta->RestaurarItem(itens[j]);
                };

                vector <cartao_de_credito> cartoes = banco->carregar_cartoes(contas_salvas[i].id);
                for(size_t j = 0; j < cartoes.size(); j++)
                {
                    conta->RestaurarCartao(cartoes[j]);

                    // cartão antigo com o número completo: ao salvar de novo ficam só os 4 últimos dígitos
                    if(cartoes[j].GetNumero().size() > 4)
                    {
                        regravar = true;
                    };
                };

                contas.push_back(conta);

                if(regravar == true)
                {
                    salvar(conta);
                };

                if(conta->GetId() >= proximo_id)
                {
                    proximo_id = conta->GetId() + 1;
                };
            };

            return (int)contas_salvas.size();
        };


        // cria a conta, salva no banco e devolve o endereço dela
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
            salvar(nova_conta);
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
            salvar(conta);
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


        // delete da conta, na lista e no banco
        bool remover(int id_conta)
        {
            for(size_t i = 0; i < contas.size(); i++)
            {
                if(contas[i]->GetId() == id_conta)
                {
                    cout << "A conta: " << contas[i]->Getnome() << " foi removida." << endl;
                    delete contas[i];
                    contas.erase(contas.begin() + i);

                    if(banco != nullptr)
                    {
                        banco->remover_conta(id_conta);
                    };
                    return true;
                };
            };

            cout << "Não existe conta com o id: " << id_conta << endl;
            return false;
        };


        // delete do jogo da loja
        bool excluir_jogo(catalogo& loja, int id_jogo) const
        {
            jogo_base* jogo = loja.buscar_por_id(id_jogo);

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

        // cria um gift card novo, recusa se o código já existir
        bool criar_gift_card(const string& codigo, double valor)
        {
            if(banco == nullptr)
            {
                return false;
            };

            gift_card existente;
            if(banco->buscar_gift_card(codigo, existente) == true)
            {
                cout << "Já existe um gift card com o código: " << codigo << endl;
                return false;
            };

            // inserir_gift_card falha se o código já existir, então nunca substitui um gift card
            return banco->inserir_gift_card(gift_card(codigo, valor));
        };


        // resgata o gift card na conta e salva os dois no banco
        void resgatar_gift_card(usuario* conta, const string& codigo)
        {
            if(banco == nullptr || conta == nullptr)
            {
                return;
            };

            gift_card card;
            if(banco->buscar_gift_card(codigo, card) == false)
            {
                cout << "Código de gift card inválido." << endl;
                return;
            };

            bool estava_valido = card.EstaValido();
            conta->Cadastrar_GiftCard(card, codigo);

            // se o cartão foi usado agora, marca como usado no banco e salva o novo saldo
            if(estava_valido == true && card.EstaValido() == false)
            {
                banco->salvar_gift_card(card);
                salvar(conta);
            };
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