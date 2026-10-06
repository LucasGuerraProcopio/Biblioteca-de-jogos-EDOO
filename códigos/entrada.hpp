#pragma once
#include <cstdlib>
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


// Lê número inteiro
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


// Lê número decimal maior que zero
inline double ler_positivo(string pergunta)
{
    string texto;

    while(true)
    {
        cout << pergunta;

        if(!getline(cin, texto))
        {
            return 0;
        };

        // troca a vírgula por ponto
        for(size_t i = 0; i < texto.size(); i++)
        {
            if(texto[i] == ',')
            {
                texto[i] = '.';
            };
        };

        // só aceita dígitos e no máximo um ponto
        bool valido = true;
        int pontos = 0;
        int digitos = 0;

        for(size_t i = 0; i < texto.size(); i++)
        {
            if(texto[i] == '.')
            {
                pontos++;
            }
            else if(texto[i] >= '0' && texto[i] <= '9')
            {
                digitos++;
            }
            else
            {
                valido = false;
            };
        };

        if(valido == true && pontos <= 1 && digitos > 0 && digitos <= 15)
        {
            double numero = atof(texto.c_str());

            if(numero > 0)
            {
                return numero;
            };
        };

        cout << "Digite um número maior que zero (ponto ou vírgula nos decimais). ";
    };
}