#pragma once
#include <string>
using namespace std;


// Transforma a senha em um código para que ela não fique salva em texto puro no banco
// Usa o algoritmo FNV-1a com um texto fixo na frente da senha
inline string gerar_hash(const string& senha)
{
    string texto = "biblioteca-jogos:" + senha;
    unsigned long long codigo = 14695981039346656037ULL;

    for(size_t i = 0; i < texto.size(); i++)
    {
        codigo = codigo ^ (unsigned char)texto[i];
        codigo = codigo * 1099511628211ULL;
    };

    // converte o número para texto em hexadecimal
    const char* digitos = "0123456789abcdef";
    string resultado = "";

    for(int i = 15; i >= 0; i--)
    {
        resultado += digitos[(codigo >> (i * 4)) & 15];
    };

    // descobre se o texto já é um hash
    return "h1:" + resultado;
}


// verifica se o texto está no formato hash
inline bool eh_hash(const string& texto)
{
    return texto.size() == 19 && texto.substr(0, 3) == "h1:";
}