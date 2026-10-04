#pragma once
#include <iostream>
#include <string>
using namespace std;


// Lê linha de texto e repete se vier vazia
inline string ler_texto(string pergunta)
{
    string texto;

    cout << pergunta;
    getline(cin, texto);

    while(texto.size() == 0 && cin.good())
    {
        cout << "Não pode ficar vazio. " << pergunta;
        getline(cin, texto);
    };

    return texto;
}


// Lê número inteiro e repete se o que foi digitado não for um número
inline int ler_inteiro(string pergunta)
{
    int numero = 0;

    cout << pergunta;
    while(!(cin >> numero) || cin.peek() != '\n')
    {
        if(cin.eof())
        {
            return 0;
        };

        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Digite um número inteiro. " << pergunta;
    };
    cin.ignore(10000, '\n');

    return numero;
}


// Lê número com ponto decimal e repete se não for maior que zero
inline double ler_positivo(string pergunta)
{
    double numero = 0;

    cout << pergunta;
    while(!(cin >> numero) || numero <= 0 || cin.peek() != '\n')
    {
        if(cin.eof())
        {
            return 0;
        };

        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Digite um número maior que zero, com ponto nos decimais. " << pergunta;
    };
    cin.ignore(10000, '\n');

    return numero;
}