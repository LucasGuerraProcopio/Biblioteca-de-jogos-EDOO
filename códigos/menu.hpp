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

    int id_novo = -1;

    if(tipo == 1)
    {
        id_novo = loja.adicionar(new jogos_gratuitos(titulo, tamanho, ""));
    }
    else
    {
        double preco = ler_positivo("Preço em reais: ");
        if(preco == 0)
        {
            return;
        };
        id_novo = loja.adicionar(new jogos_pagos(titulo, tamanho, "", preco));
    };

    if(id_novo != -1)
    {
        cout << "Jogo cadastrado com o id " << id_novo << "." << endl;
    };
}

// altera título, tamanho ou preço de um jogo
inline void atualizar_jogo(catalogo& loja)
{
    cout << "\n--- Atualizar jogo ---" << endl;

    int id_jogo = ler_inteiro("Id do jogo: ");
    jogos_gratuitos* jogo = loja.buscar_por_id(id_jogo);

    if(jogo == nullptr)
    {
        cout << "Não existe jogo com o id: " << id_jogo << endl;
        return;
    };

    jogo->mostrar_informacoes();

    int opcao = ler_inteiro("O que alterar (1 - título, 2 - tamanho, 3 - preço): ");
    while(opcao < 1 || opcao > 3)
    {
        if(cin.eof())
        {
            return;
        };
        opcao = ler_inteiro("Digite 1, 2 ou 3: ");
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
    jogos_gratuitos* jogo = loja.buscar_por_id(id_jogo);

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
        jogos_gratuitos* jogo = loja.buscar_por_id(id_jogo);

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

        vector <jogos_gratuitos*> encontrados = loja.buscar_por_trecho(trecho);

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
            case 0:
                cout << "Saindo..." << endl;
                break;
            default:
                cout << "Opção inválida." << endl;
        };
    };
}