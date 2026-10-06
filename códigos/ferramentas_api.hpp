#pragma once
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;


// transforma um texto em texto JSON entre aspas, escapando os caracteres especiais
inline string json_texto(const string& texto)
{
    string saida = "\"";

    for(size_t i = 0; i < texto.size(); i++)
    {
        char c = texto[i];

        if(c == '"')
        {
            saida += "\\\"";
        }
        else if(c == '\\')
        {
            saida += "\\\\";
        }
        else if(c == '\n')
        {
            saida += "\\n";
        }
        else if(c == '\t')
        {
            saida += "\\t";
        }
        else if((unsigned char)c < 0x20)
        {
            // outros caracteres de controle viram \u00XX
            char codigo[8];
            snprintf(codigo, sizeof(codigo), "\\u%04x", (unsigned char)c);
            saida += codigo;
        }
        else
        {
            saida += c;
        };
    };

    return saida + "\"";
}


// transforma um número em texto JSON (sempre com ponto nos decimais, ex.: 0.8 e 550)
inline string json_numero(double numero)
{
    ostringstream texto;
    texto << setprecision(15) << numero;
    return texto.str();
}


// enquanto um objeto desta classe existir, tudo que as classes escrevem no cout
// fica guardado aqui em vez de ir para o terminal (assim as mensagens vão para a tela)
class captura_cout
{
    private:
        ostringstream guardado;
        streambuf* original;

    public:
        captura_cout()
        {
            original = cout.rdbuf(guardado.rdbuf());
        };

        // devolve o cout ao terminal quando o objeto é destruído
        ~captura_cout()
        {
            cout.rdbuf(original);
        };

        captura_cout(const captura_cout&) = delete;
        captura_cout& operator=(const captura_cout&) = delete;

        // devolve as linhas escritas como uma lista JSON: ["linha 1", "linha 2"]
        string mensagens_json() const
        {
            istringstream linhas(guardado.str());
            string linha;
            string lista = "[";
            bool primeira = true;

            while(getline(linhas, linha))
            {
                if(linha.size() > 0 && linha[linha.size() - 1] == '\r')
                {
                    linha.erase(linha.size() - 1);
                };

                if(linha.size() == 0)
                {
                    continue;
                };

                if(primeira == false)
                {
                    lista += ", ";
                };
                lista += json_texto(linha);
                primeira = false;
            };

            return lista + "]";
        };
};
