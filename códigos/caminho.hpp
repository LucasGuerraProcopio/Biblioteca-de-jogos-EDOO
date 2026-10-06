#pragma once
#include <string>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
using namespace std;


// devolve a pasta onde o programa está, em UTF-8
// no Windows o argv[0] não vem em UTF-8, então pastas com acento (como "códigos")
// quebravam o caminho do banco; por isso lá o caminho é pedido direto ao sistema
inline string pasta_do_programa(const char* executavel)
{
    string caminho = executavel;

    #ifdef _WIN32
    wchar_t caminho_largo[MAX_PATH];
    DWORD tamanho = GetModuleFileNameW(NULL, caminho_largo, MAX_PATH);

    if(tamanho > 0 && tamanho < MAX_PATH)
    {
        int bytes = WideCharToMultiByte(CP_UTF8, 0, caminho_largo, (int)tamanho, NULL, 0, NULL, NULL);
        caminho = string(bytes, '\0');
        WideCharToMultiByte(CP_UTF8, 0, caminho_largo, (int)tamanho, &caminho[0], bytes, NULL, NULL);
    };
    #endif

    size_t posicao = caminho.find_last_of("/\\");

    if(posicao == string::npos)
    {
        return "";
    };

    return caminho.substr(0, posicao + 1);
}