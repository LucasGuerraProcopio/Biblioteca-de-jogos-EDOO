// Servidor local da interface gráfica
// O C++ continua com toda a lógica; a interface web (pasta web/) só chama a API
#include "httplib.h" // precisa vir antes dos outros includes (no Windows ele inclui o winsock)
#include <iostream>
#include <string>
#include "catalogo.hpp"
#include "usuario.hpp"
#include "caminho.hpp"
using namespace std;


int main(int argc, char* argv[])
{
    // deixa o terminal em UTF-8 para mostrar os acentos certos
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    string pasta = "";
    if(argc > 0)
    {
        pasta = pasta_do_programa(argv[0]);
    };

    // usa o mesmo banco do menu no terminal
    banco_de_dados banco(pasta + "biblioteca.db");

    if(banco.aberto() == false)
    {
        cout << "Sem o banco de dados o servidor não pode continuar." << endl;
        return 1;
    };

    catalogo loja(&banco);
    repositorio_usuarios usuarios(&banco);
    loja.carregar_do_banco();
    usuarios.carregar_do_banco();

    httplib::Server servidor;

    // entrega os arquivos da interface (HTML, CSS e JavaScript)
    // procura a pasta web ao lado do programa ou dentro de códigos/
    if(servidor.set_mount_point("/", pasta + "web") == false &&
       servidor.set_mount_point("/", pasta + "códigos/web") == false)
    {
        cout << "Pasta web não encontrada ao lado do programa." << endl;
        return 1;
    };

    // primeiro caminho da API: confirma que o servidor está vivo
    servidor.Get("/api/status", [&](const httplib::Request&, httplib::Response& resposta)
    {
        string json = "{\"ok\": true, \"jogos\": " + to_string(loja.buscar_por_trecho("").size()) + "}";
        resposta.set_content(json, "application/json");
    });

    cout << "Interface disponível em http://localhost:8080" << endl;
    cout << "Para encerrar, aperte Ctrl + C." << endl;

    servidor.listen("localhost", 8080);

    return 0;
}