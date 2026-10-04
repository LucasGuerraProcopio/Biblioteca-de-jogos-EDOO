#pragma once
#include <iostream>
#include <string>
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
            case 0:
                cout << "Saindo..." << endl;
                break;
            default:
                cout << "Opção inválida." << endl;
        };
    };
}