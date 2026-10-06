#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "entrada.hpp"
#include "catalogo.hpp"
#include "usuario.hpp"
using namespace std;


// pede os dados e cadastra o jogo no catálogo e no banco
inline void cadastrar_jogo(catalogo& loja)
{
    cout << "\n--- Cadastrar jogo ---" << endl;

    string titulo = ler_texto("Título: ");
    if(titulo.size() == 0)
    {
        return;
    };

    if(loja.buscar_por_titulo(titulo) != nullptr)
    {
        cout << "Já existe um jogo com o título: " << titulo << endl;
        return;
    };

    double tamanho = ler_positivo("Tamanho em Gigabytes: ");
    if(tamanho == 0)
    {
        return;
    };

    int tipo = ler_inteiro("Tipo (1 - gratuito, 2 - pago): ");
    while(tipo != 1 && tipo != 2)
    {
        if(cin.eof())
        {
            return;
        };
        tipo = ler_inteiro("Digite 1 para gratuito ou 2 para pago: ");
    };

    // usuário e senha usados para entrar no jogo
    string usuario_jogo = ler_texto("Usuário do jogo: ");
    if(usuario_jogo.size() == 0)
    {
        return;
    };

    string senha_jogo = ler_texto("Senha do jogo: ");
    if(senha_jogo.size() == 0)
    {
        return;
    };

    int id_novo = -1;

    if(tipo == 1)
    {
        id_novo = loja.adicionar(new jogos_gratuitos(titulo, tamanho, senha_jogo, usuario_jogo));
    }
    else
    {
        double preco = ler_positivo("Preço em reais: ");
        if(preco == 0)
        {
            return;
        };
        id_novo = loja.adicionar(new jogos_pagos(titulo, tamanho, senha_jogo, preco, usuario_jogo));
    };

    if(id_novo != -1)
    {
        cout << "Jogo cadastrado com o id " << id_novo << "." << endl;
    };
}

// altera título, tamanho, preço, usuário ou senha de um jogo
inline void atualizar_jogo(catalogo& loja)
{
    cout << "\n--- Atualizar jogo ---" << endl;

    int id_jogo = ler_inteiro("Id do jogo: ");
    jogo_base* jogo = loja.buscar_por_id(id_jogo);

    if(jogo == nullptr)
    {
        cout << "Não existe jogo com o id: " << id_jogo << endl;
        return;
    };

    jogo->mostrar_informacoes();

    int opcao = ler_inteiro("O que alterar (1 - título, 2 - tamanho, 3 - preço, 4 - usuário, 5 - senha): ");
    while(opcao < 1 || opcao > 5)
    {
        if(cin.eof())
        {
            return;
        };
        opcao = ler_inteiro("Digite um número de 1 a 5: ");
    };

    if(opcao == 1)
    {
        string novo_titulo = ler_texto("Novo título: ");
        if(novo_titulo.size() == 0)
        {
            return;
        };

        if(loja.buscar_por_titulo(novo_titulo) != nullptr)
        {
            cout << "Já existe um jogo com o título: " << novo_titulo << endl;
            return;
        };
        jogo->SetTitulo(novo_titulo);
    }
    else if(opcao == 2)
    {
        double novo_tamanho = ler_positivo("Novo tamanho em Gigabytes: ");
        if(novo_tamanho == 0)
        {
            return;
        };
        jogo->SetTamanho(novo_tamanho);
    }
    else if(opcao == 4)
    {
        string novo_usuario = ler_texto("Novo usuário do jogo: ");
        if(novo_usuario.size() == 0)
        {
            return;
        };
        jogo->SetConta(novo_usuario);
    }
    else if(opcao == 5)
    {
        string nova_senha = ler_texto("Nova senha do jogo: ");
        if(nova_senha.size() == 0)
        {
            return;
        };
        jogo->SetSenha(nova_senha);
    }
    else
    {
        // só jogos pagos têm preço
        jogos_pagos* jogo_pago = dynamic_cast<jogos_pagos*>(jogo);
        if(jogo_pago == nullptr)
        {
            cout << "Jogos gratuitos não têm preço." << endl;
            return;
        };

        double novo_preco = ler_positivo("Novo preço em reais: ");
        if(novo_preco == 0)
        {
            return;
        };
        jogo_pago->SetValor(novo_preco);
    };

    loja.salvar(jogo);
    cout << "Jogo atualizado." << endl;
}

// pede os dados e cria a conta no repositório
inline void cadastrar_conta(repositorio_usuarios& usuarios)
{
    cout << "\n--- Cadastrar conta ---" << endl;

    string nome = ler_texto("Nome da conta: ");
    if(nome.size() == 0)
    {
        return;
    };

    if(usuarios.buscar_por_nome(nome) != nullptr)
    {
        cout << "Já existe uma conta com o nome: " << nome << endl;
        return;
    };

    string senha = ler_texto("Senha: ");
    if(senha.size() == 0)
    {
        return;
    };

    usuario* nova_conta = usuarios.criar(nome, senha);

    if(nova_conta != nullptr)
    {
        cout << "Conta criada com o id " << nova_conta->GetId() << "." << endl;
    };
}


// altera o nome ou a senha de uma conta
inline void atualizar_conta(repositorio_usuarios& usuarios)
{
    cout << "\n--- Atualizar conta ---" << endl;

    int id_conta = ler_inteiro("Id da conta: ");
    usuario* conta = usuarios.buscar_por_id(id_conta);

    if(conta == nullptr)
    {
        cout << "Não existe conta com o id: " << id_conta << endl;
        return;
    };

    string senha_atual = ler_texto("Senha atual: ");
    if(conta->verificar_senha(senha_atual) == false)
    {
        cout << "Senha incorreta." << endl;
        return;
    };

    int opcao = ler_inteiro("O que alterar (1 - nome, 2 - senha): ");
    while(opcao != 1 && opcao != 2)
    {
        if(cin.eof())
        {
            return;
        };
        opcao = ler_inteiro("Digite 1 para nome ou 2 para senha: ");
    };

    if(opcao == 1)
    {
        string novo_nome = ler_texto("Novo nome: ");
        if(novo_nome.size() == 0)
        {
            return;
        };
        usuarios.renomear(id_conta, novo_nome);
    }
    else
    {
        string nova_senha = ler_texto("Nova senha: ");
        if(nova_senha.size() == 0)
        {
            return;
        };
        conta->SetSenha(nova_senha);
        usuarios.salvar(conta);
    };
}


// remove uma conta do repositório
inline void remover_conta(repositorio_usuarios& usuarios)
{
    cout << "\n--- Remover conta ---" << endl;

    int id_conta = ler_inteiro("Id da conta: ");
    usuario* conta = usuarios.buscar_por_id(id_conta);

    if(conta == nullptr)
    {
        cout << "Não existe conta com o id: " << id_conta << endl;
        return;
    };

    string senha = ler_texto("Senha da conta: ");
    if(conta->verificar_senha(senha) == false)
    {
        cout << "Senha incorreta." << endl;
        return;
    };

    usuarios.remover(id_conta);
}


// remove um jogo da loja e do banco, só se nenhuma conta possuir o jogo
inline void remover_jogo(catalogo& loja, repositorio_usuarios& usuarios)
{
    cout << "\n--- Remover jogo ---" << endl;
    loja.listar();

    int id_jogo = ler_inteiro("Id do jogo: ");
    jogo_base* jogo = loja.buscar_por_id(id_jogo);

    if(jogo == nullptr)
    {
        cout << "Não existe jogo com o id: " << id_jogo << endl;
        return;
    };

    string confirmacao = ler_texto("Remover o jogo " + jogo->GetTitulo() + "? (s/n): ");
    if(confirmacao != "s" && confirmacao != "S")
    {
        cout << "Remoção cancelada." << endl;
        return;
    };

    usuarios.excluir_jogo(loja, id_jogo);
}


// busca jogos pelo id ou por parte do título
inline void buscar_jogo(catalogo& loja)
{
    cout << "\n--- Buscar jogo ---" << endl;

    int opcao = ler_inteiro("Buscar por (1 - id, 2 - título): ");
    while(opcao != 1 && opcao != 2)
    {
        if(cin.eof())
        {
            return;
        };
        opcao = ler_inteiro("Digite 1 para id ou 2 para título: ");
    };

    if(opcao == 1)
    {
        int id_jogo = ler_inteiro("Id do jogo: ");
        jogo_base* jogo = loja.buscar_por_id(id_jogo);

        if(jogo == nullptr)
        {
            cout << "Não existe jogo com o id: " << id_jogo << endl;
            return;
        };

        jogo->mostrar_informacoes();
    }
    else
    {
        string trecho = ler_texto("Parte do título: ");
        if(trecho.size() == 0)
        {
            return;
        };

        vector <jogo_base*> encontrados = loja.buscar_por_trecho(trecho);

        if(encontrados.size() == 0)
        {
            cout << "Nenhum jogo encontrado com: " << trecho << endl;
            return;
        };

        cout << encontrados.size() << " jogo(s) encontrado(s):" << endl;
        for(size_t i = 0; i < encontrados.size(); i++)
        {
            encontrados[i]->mostrar_informacoes();
        };
    };
}

// MENU DA CONTA
// mostra os cartões da conta, escondendo o número menos os 4 últimos dígitos
inline void listar_cartoes(const usuario& conta)
{
    const vector <cartao_de_credito>& cartoes = conta.GetCartoes();
 
    for(size_t i = 0; i < cartoes.size(); i++)
    {
        string numero = cartoes[i].GetNumero();
        string final_numero = numero;
 
        if(numero.size() > 4)
        {
            final_numero = numero.substr(numero.size() - 4);
        };
 
        cout << "  " << i + 1 << " - cartão final " << final_numero << " | disponível: " << cartoes[i].GetLimite() - cartoes[i].GetGastos() << " reais" << endl;
    };
}
 
 
// depósito via pix
inline void conta_depositar_pix(usuario& conta)
{
    pix pix_da_loja; // a classe pix já vem com a chave e a taxa da biblioteca
 
    cout << "\n--- Depósito via pix ---" << endl;
    cout << "Chave pix da biblioteca: " << pix_da_loja.GetChave() << " (taxa de " << pix_da_loja.GetTaxa() * 100 << "%)" << endl;
 
    double valor = ler_positivo("Valor do depósito: ");
    if(valor == 0)
    {
        return;
    };
 
    string chave = ler_texto("Chave pix: ");
    if(chave.size() == 0)
    {
        return;
    };
 
    conta.depositar_pix(valor, chave);
}
 
 
// cadastra um cartão de crédito na conta
inline void conta_cadastrar_cartao(usuario& conta)
{
    cout << "\n--- Cadastrar cartão ---" << endl;
 
    cout << "Por segurança, só os 4 últimos dígitos ficam salvos no banco e o CVC não é guardado." << endl;

    string numero = ler_texto("Número do cartão: ");
    if(numero.size() == 0)
    {
        return;
    };
 
    int cvc = ler_inteiro("CVC: ");
    while(cvc < 1 || cvc > 9999)
    {
        if(cin.eof())
        {
            return;
        };
        cvc = ler_inteiro("O CVC tem 3 ou 4 dígitos. CVC: ");
    };
 
    string validade = ler_texto("Validade (MM/AA): ");
    if(validade.size() == 0)
    {
        return;
    };
 
    double limite = ler_positivo("Limite em reais: ");
    if(limite == 0)
    {
        return;
    };
 
    conta.adicionar_cartao(cartao_de_credito(numero, cvc, validade, limite));
}
 
 
// compra um jogo com saldo ou cartão (jogos gratuitos entram direto)
inline void conta_comprar_jogo(usuario& conta, catalogo& loja)
{
    cout << "\n--- Comprar jogo ---" << endl;
 
    int id_jogo = ler_inteiro("Id do jogo: ");
    jogo_base* jogo = loja.buscar_por_id(id_jogo);
 
    if(jogo == nullptr)
    {
        cout << "Não existe jogo com o id: " << id_jogo << endl;
        return;
    };
 
    if(conta.possui(id_jogo) == true)
    {
        cout << "Você já possui o jogo: " << jogo->GetTitulo() << endl;
        return;
    };
 
    if(jogo->EhPago() == false)
    {
        conta.adquirir(*jogo);
        return;
    };
 
    cout << jogo->GetTitulo() << " custa " << jogo->GetPreco() << " reais. Seu saldo: " << conta.GetSaldo() << " reais." << endl;
 
    int forma = ler_inteiro("Pagar com (1 - saldo, 2 - cartão): ");
    while(forma != 1 && forma != 2)
    {
        if(cin.eof())
        {
            return;
        };
        forma = ler_inteiro("Digite 1 para saldo ou 2 para cartão: ");
    };
 
    if(forma == 1)
    {
        conta.adquirir(*jogo);
        return;
    };
 
    if(conta.GetCartoes().size() == 0)
    {
        cout << "Você não tem cartões cadastrados." << endl;
        return;
    };
 
    listar_cartoes(conta);
    int escolhido = ler_inteiro("Número do cartão: ");
 
    // a lista começa em 1 na tela, mas o índice do vector começa em 0
    conta.adquirir_com_cartao(*jogo, escolhido - 1);
}
 
 
// instala um jogo da biblioteca
inline void conta_instalar(usuario& conta)
{
    int id_jogo = ler_inteiro("Id do jogo para instalar: ");
    conta.instalar(id_jogo);
}
 
 
// desinstala um jogo, que continua na biblioteca
inline void conta_desinstalar(usuario& conta)
{
    int id_jogo = ler_inteiro("Id do jogo para desinstalar: ");
    conta.desinstalar(id_jogo);
}
 
 
// registra horas jogadas em um jogo instalado
inline void conta_jogar(usuario& conta)
{
    int id_jogo = ler_inteiro("Id do jogo para jogar: ");
    int horas = ler_inteiro("Quantas horas jogou: ");
    conta.jogar(id_jogo, horas);
}
 
 
// devolve um jogo pago e o dinheiro volta para o saldo ou para o cartão usado na compra
inline void conta_reembolsar(usuario& conta, catalogo& loja)
{
    int id_jogo = ler_inteiro("Id do jogo para reembolsar: ");
    jogo_base* jogo = loja.buscar_por_id(id_jogo);
 
    if(jogo == nullptr)
    {
        cout << "Não existe jogo com o id: " << id_jogo << endl;
        return;
    };
 
    string confirmacao = ler_texto("Reembolsar o jogo " + jogo->GetTitulo() + "? (s/n): ");
    if(confirmacao != "s" && confirmacao != "S")
    {
        cout << "Reembolso cancelado." << endl;
        return;
    };
 
    conta.reembolsar(*jogo);
}
 
// resgata um gift card e o valor vai para o saldo
inline void conta_resgatar_gift_card(usuario& conta, repositorio_usuarios& usuarios)
{
    string codigo = ler_texto("Código do gift card: ");
    if(codigo.size() == 0)
    {
        return;
    };

    usuarios.resgatar_gift_card(&conta, codigo);
}

// menu da conta logada
inline void menu_da_conta(usuario* conta, catalogo& loja, repositorio_usuarios& usuarios)
{
    int opcao = -1;
 
    while(opcao != 0)
    {
        cout << "\n===== Conta de " << conta->Getnome() << " | Saldo: " << conta->GetSaldo() << " reais =====" << endl;
        cout << "1 - Ver biblioteca" << endl;
        cout << "2 - Depositar via pix" << endl;
        cout << "3 - Cadastrar cartão" << endl;
        cout << "4 - Comprar jogo" << endl;
        cout << "5 - Instalar jogo" << endl;
        cout << "6 - Desinstalar jogo" << endl;
        cout << "7 - Reembolsar jogo" << endl;
        cout << "8 - Resgatar gift card" << endl;
        cout << "9 - Jogar" << endl;
        cout << "0 - Sair da conta" << endl;
 
        opcao = ler_inteiro("Escolha: ");
 
        switch(opcao)
        {
            case 1:
                conta->mostrar_biblioteca(loja);
                break;
            case 2:
                conta_depositar_pix(*conta);
                break;
            case 3:
                conta_cadastrar_cartao(*conta);
                break;
            case 4:
                conta_comprar_jogo(*conta, loja);
                break;
            case 5:
                conta_instalar(*conta);
                break;
            case 6:
                conta_desinstalar(*conta);
                break;
            case 7:
                conta_reembolsar(*conta, loja);
                break;
            case 8:
                conta_resgatar_gift_card(*conta, usuarios);
                break;
            case 9:
                conta_jogar(*conta);
                break;
            case 0:
                cout << "Saindo da conta..." << endl;
                break;
            default:
                cout << "Opção inválida." << endl;
        };
 
        // as opções 2 a 9 podem mudar a conta, então ela é salva no banco
        if(opcao >= 2 && opcao <= 9)
        {
            usuarios.salvar(conta);
        };
    };
}
 
 
// pede nome e senha e abre o menu da conta
inline void entrar_na_conta(catalogo& loja, repositorio_usuarios& usuarios)
{
    cout << "\n--- Entrar na conta ---" << endl;
 
    string nome = ler_texto("Nome da conta: ");
    if(nome.size() == 0)
    {
        return;
    };
 
    string senha = ler_texto("Senha: ");
    if(senha.size() == 0)
    {
        return;
    };
 
    usuario* conta = usuarios.buscar_por_nome(nome);
 
    // a mesma mensagem nos dois casos, para não revelar quais contas existem
    if(conta == nullptr || conta->verificar_senha(senha) == false)
    {
        cout << "Nome ou senha incorretos." << endl;
        return;
    };
 
    cout << "Bem-vindo, " << conta->Getnome() << "!" << endl;
    menu_da_conta(conta, loja, usuarios);
}

// cria um gift card que depois pode ser resgatado em qualquer conta
inline void criar_gift_card(repositorio_usuarios& usuarios)
{
    cout << "\n--- Criar gift card ---" << endl;

    string codigo = ler_texto("Código do gift card: ");
    if(codigo.size() == 0)
    {
        return;
    };

    double valor = ler_positivo("Valor em reais: ");
    if(valor == 0)
    {
        return;
    };

    if(usuarios.criar_gift_card(codigo, valor) == true)
    {
        cout << "Gift card " << codigo << " criado com valor de " << valor << " reais." << endl;
    };
}

// menu principal
inline void executar_menu(catalogo& loja, repositorio_usuarios& usuarios)
{
    int opcao = -1;

    while(opcao != 0)
    {
        cout << "\n===== Biblioteca de jogos =====" << endl;
        cout << "1 - Listar jogos" << endl;
        cout << "2 - Cadastrar jogo" << endl;
        cout << "3 - Listar contas" << endl;
        cout << "4 - Cadastrar conta" << endl;
        cout << "5 - Atualizar conta" << endl;
        cout << "6 - Remover conta" << endl;
        cout << "7 - Remover jogo" << endl;
        cout << "8 - Atualizar jogo" << endl;
        cout << "9 - Buscar jogo" << endl;
        cout << "10 - Entrar na conta" << endl;
        cout << "11 - Criar gift card" << endl;
        cout << "0 - Sair" << endl;

        opcao = ler_inteiro("Escolha: ");

        switch(opcao)
        {
            case 1:
                loja.listar();
                break;
            case 2:
                cadastrar_jogo(loja);
                break;
            case 3:
                usuarios.listar();
                break;
            case 4:
                cadastrar_conta(usuarios);
                break;
            case 5:
                atualizar_conta(usuarios);
                break;
            case 6:
                remover_conta(usuarios);
                break;
            case 7:
                remover_jogo(loja, usuarios);
                break;
            case 8:
                atualizar_jogo(loja);
                break;
            case 9:
                buscar_jogo(loja);
                break;
            case 10:
                entrar_na_conta(loja, usuarios);
                break;
            case 11:
                criar_gift_card(usuarios);
                break;
            case 0:
                cout << "Saindo..." << endl;
                break;
            default:
                cout << "Opção inválida." << endl;
        };
    };
}