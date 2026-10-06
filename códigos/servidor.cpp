// Servidor local da interface gráfica
// O C++ continua com toda a lógica; a interface web (pasta web/) só chama a API
#include "httplib.h" // precisa vir antes dos outros includes (no Windows ele inclui o winsock)
#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include "catalogo.hpp"
#include "usuario.hpp"
#include "caminho.hpp"
#include "ferramentas_api.hpp"
#include "jogos_iniciais.hpp"
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

    // o que for escrito no cout durante a carga vira a primeira mensagem da tela
    string mensagens_iniciais;
    {
        captura_cout captura;

        int carregados = loja.carregar_do_banco();

        // na primeira vez, o próprio servidor cadastra os jogos iniciais
        cadastrar_jogos_iniciais(banco, loja, carregados);

        if(carregados > 0)
        {
            cout << carregados << " jogos carregados do banco." << endl;
        };

        int contas_carregadas = usuarios.carregar_do_banco();
        if(contas_carregadas > 0)
        {
            cout << contas_carregadas << " contas carregadas do banco." << endl;
        };

        mensagens_iniciais = captura.mensagens_json();
    }

    httplib::Server servidor;

    // o servidor atende vários pedidos ao mesmo tempo, mas as classes e o cout não foram
    // feitos para isso; esta trava garante que um pedido da API rode por vez
    mutex trava_api;

    // entrega os arquivos da interface (HTML, CSS e JavaScript)
    // procura a pasta web ao lado do programa ou dentro de códigos/
    if(servidor.set_mount_point("/", pasta + "web") == false &&
       servidor.set_mount_point("/", pasta + "códigos/web") == false)
    {
        cout << "Pasta web não encontrada ao lado do programa." << endl;
        return 1;
    };

    // confirma que o servidor está vivo e manda as mensagens da carga
    servidor.Get("/api/status", [&](const httplib::Request&, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);

        string json = "{\"ok\": true, \"jogos\": " + to_string(loja.buscar_por_trecho("").size()) +
                      ", \"mensagens\": " + mensagens_iniciais + "}";
        resposta.set_content(json, "application/json");
    });

    // lista os jogos do catálogo, com busca, filtro e ordenação feitos aqui no C++
    // exemplo: /api/jogos?busca=mine&tipo=pagos&ordem=preco&direcao=desc
    servidor.Get("/api/jogos", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        string busca = pedido.get_param_value("busca");
        string tipo = pedido.get_param_value("tipo");
        string ordem = pedido.get_param_value("ordem");
        bool decrescente = pedido.get_param_value("direcao") == "desc";

        // busca por trecho do título (a mesma função do menu)
        vector <jogo_base*> encontrados = loja.buscar_por_trecho(busca);

        // filtro por tipo: EhPago() é virtual, cada classe responde do seu jeito
        vector <jogo_base*> lista;
        for(size_t i = 0; i < encontrados.size(); i++)
        {
            bool pago = encontrados[i]->EhPago();

            if(tipo == "pagos" && pago == false)
            {
                continue;
            };
            if(tipo == "gratuitos" && pago == true)
            {
                continue;
            };
            lista.push_back(encontrados[i]);
        };

        // ordena pelo id primeiro; o stable_sort depois mantém essa ordem nos empates
        sort(lista.begin(), lista.end(), [](jogo_base* a, jogo_base* b)
        {
            return a->GetId() < b->GetId();
        });

        if(ordem == "titulo")
        {
            stable_sort(lista.begin(), lista.end(), [decrescente](jogo_base* a, jogo_base* b)
            {
                return decrescente ? b->GetTitulo() < a->GetTitulo() : a->GetTitulo() < b->GetTitulo();
            });
        }
        else if(ordem == "tamanho")
        {
            // usa o operator< que o jogo_base já tem (compara pelo tamanho)
            stable_sort(lista.begin(), lista.end(), [decrescente](jogo_base* a, jogo_base* b)
            {
                return decrescente ? *b < *a : *a < *b;
            });
        }
        else if(ordem == "preco")
        {
            stable_sort(lista.begin(), lista.end(), [decrescente](jogo_base* a, jogo_base* b)
            {
                return decrescente ? b->GetPreco() < a->GetPreco() : a->GetPreco() < b->GetPreco();
            });
        }
        else if(decrescente == true)
        {
            reverse(lista.begin(), lista.end());
        };

        // monta a resposta em JSON
        string jogos_json = "[";
        for(size_t i = 0; i < lista.size(); i++)
        {
            if(i > 0)
            {
                jogos_json += ", ";
            };

            jogos_json += "{\"id\": " + to_string(lista[i]->GetId()) +
                          ", \"titulo\": " + json_texto(lista[i]->GetTitulo()) +
                          ", \"tamanho\": " + json_numero(lista[i]->GetTamanho()) +
                          ", \"preco\": " + json_numero(lista[i]->GetPreco()) +
                          ", \"pago\": " + (lista[i]->EhPago() ? "true" : "false") + "}";
        };
        jogos_json += "]";

        resposta.set_content("{\"ok\": true, \"jogos\": " + jogos_json +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    cout << "Interface disponível em http://localhost:8080" << endl;
    cout << "Para encerrar, aperte Ctrl + C." << endl;

    servidor.listen("localhost", 8080);

    return 0;
}
