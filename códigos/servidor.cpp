// Servidor local da interface gráfica
// O C++ continua com toda a lógica; a interface web (pasta web/) só chama a API
#include "httplib.h" // precisa vir antes dos outros includes (no Windows ele inclui o winsock)
#include <algorithm>
#include <cstdlib>
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
            cout << carregados << (carregados == 1 ? " jogo carregado" : " jogos carregados") << " do banco." << endl;
        };

        int contas_carregadas = usuarios.carregar_do_banco();
        if(contas_carregadas > 0)
        {
            cout << contas_carregadas << (contas_carregadas == 1 ? " conta carregada" : " contas carregadas") << " do banco." << endl;
        };

        mensagens_iniciais = captura.mensagens_json();
    }

    httplib::Server servidor;

    // impede que dois servidores usem a porta 8080 ao mesmo tempo (por padrão o httplib deixa,
    // e aí os pedidos se dividem entre o servidor novo e um antigo esquecido aberto)
    servidor.set_socket_options([](socket_t soquete)
    {
        int ligado = 1;
        #ifdef _WIN32
        setsockopt(soquete, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&ligado, sizeof(ligado));
        #else
        setsockopt(soquete, SOL_SOCKET, SO_REUSEADDR, (const char*)&ligado, sizeof(ligado));
        #endif
    });

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
                          ", \"pago\": " + (lista[i]->EhPago() ? "true" : "false") +
                          ", \"usuario\": " + json_texto(lista[i]->GetConta()) +
                          ", \"em_uso\": " + (usuarios.alguem_possui(lista[i]->GetId()) ? "true" : "false") + "}";
        };
        jogos_json += "]";

        resposta.set_content("{\"ok\": true, \"jogos\": " + jogos_json +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // ===== Conta logada =====
    // o programa roda no computador de uma pessoa só, então o servidor guarda uma única
    // conta logada; guardamos o id (e não o ponteiro) porque a conta pode ser removida
    int id_logado = -1;

    // devolve a conta logada, ou nullptr se ninguém entrou
    auto conta_logada = [&]() -> usuario*
    {
        if(id_logado == -1)
        {
            return nullptr;
        };
        return usuarios.buscar_por_id(id_logado);
    };

    // só os 4 últimos dígitos do cartão saem do C++ (o número completo nunca vai para a tela)
    auto final_cartao = [](const string& numero) -> string
    {
        if(numero.size() > 4)
        {
            return numero.substr(numero.size() - 4);
        };
        return numero;
    };

    // monta o JSON da conta logada (dados, biblioteca e cartões), ou null se ninguém entrou
    auto conta_json = [&]() -> string
    {
        usuario* conta = conta_logada();
        if(conta == nullptr)
        {
            return "null";
        };

        const vector <cartao_de_credito>& cartoes = conta->GetCartoes();
        const vector <item_biblioteca>& biblioteca = conta->GetBiblioteca();

        string itens = "[";
        for(size_t i = 0; i < biblioteca.size(); i++)
        {
            const item_biblioteca& item = biblioteca[i];
            jogo_base* jogo = loja.buscar_por_id(item.id_jogo);

            // final do cartão usado na compra (vazio se não foi no cartão)
            string cartao_final = "";
            if(item.forma_pagamento == 2 && item.cartao_pago >= 0 && item.cartao_pago < (int)cartoes.size())
            {
                cartao_final = final_cartao(cartoes[item.cartao_pago].GetNumero());
            };

            if(i > 0)
            {
                itens += ", ";
            };

            itens += "{\"id\": " + to_string(item.id_jogo) +
                     ", \"titulo\": " + json_texto(jogo != nullptr ? jogo->GetTitulo() : "(jogo removido)") +
                     ", \"pago\": " + string(jogo != nullptr && jogo->EhPago() ? "true" : "false") +
                     ", \"instalado\": " + string(item.instalado ? "true" : "false") +
                     ", \"horas\": " + to_string(item.horas_jogadas) +
                     ", \"data_compra\": " + to_string(item.data_compra) +
                     ", \"forma\": " + to_string(item.forma_pagamento) +
                     ", \"valor_pago\": " + json_numero(item.valor_pago) +
                     ", \"cartao_final\": " + json_texto(cartao_final) + "}";
        };
        itens += "]";

        string lista_cartoes = "[";
        for(size_t i = 0; i < cartoes.size(); i++)
        {
            if(i > 0)
            {
                lista_cartoes += ", ";
            };

            lista_cartoes += "{\"indice\": " + to_string(i) +
                             ", \"final\": " + json_texto(final_cartao(cartoes[i].GetNumero())) +
                             ", \"validade\": " + json_texto(cartoes[i].GetValidade()) +
                             ", \"limite\": " + json_numero(cartoes[i].GetLimite()) +
                             ", \"gastos\": " + json_numero(cartoes[i].GetGastos()) + "}";
        };
        lista_cartoes += "]";

        return "{\"id\": " + to_string(conta->GetId()) +
               ", \"nome\": " + json_texto(conta->Getnome()) +
               ", \"saldo\": " + json_numero(conta->GetSaldo()) +
               ", \"biblioteca\": " + itens +
               ", \"cartoes\": " + lista_cartoes +
               ", \"prazo_reembolso_dias\": " + to_string(PRAZO_REEMBOLSO_DIAS) +
               ", \"limite_horas_reembolso\": " + to_string(LIMITE_HORAS_REEMBOLSO) +
               ", \"pix_chave\": " + json_texto(pix().GetChave()) +
               ", \"pix_taxa\": " + json_numero(pix().GetTaxa()) + "}";
    };

    // resposta padrão das ações da conta: se deu certo, a conta atualizada e as mensagens do cout
    auto responder_conta = [&](httplib::Response& resposta, bool sucesso, const captura_cout& captura)
    {
        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") + ", \"conta\": " + conta_json() +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    };

    // diz à tela se há alguém logado (usado quando a página é aberta ou recarregada)
    servidor.Get("/api/sessao", [&](const httplib::Request&, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        resposta.set_content("{\"ok\": true, \"conta\": " + conta_json() + ", \"mensagens\": []}", "application/json");
    });

    // entrar na conta: mesma regra do entrar_na_conta do menu
    servidor.Post("/api/entrar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        string nome = pedido.get_param_value("nome");
        string senha = pedido.get_param_value("senha");
        bool sucesso = false;

        if(nome.size() == 0 || senha.size() == 0)
        {
            cout << "Preencha o nome e a senha." << endl;
        }
        else
        {
            usuario* conta = usuarios.buscar_por_nome(nome);

            // a mesma mensagem nos dois casos, para não revelar quais contas existem
            if(conta == nullptr || conta->verificar_senha(senha) == false)
            {
                cout << "Nome ou senha incorretos." << endl;
            }
            else
            {
                id_logado = conta->GetId();
                cout << "Bem-vindo, " << conta->Getnome() << "!" << endl;
                sucesso = true;
            };
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") + ", \"conta\": " + conta_json() +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // criar conta: usa o criar() do repositório
    servidor.Post("/api/contas", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        string nome = pedido.get_param_value("nome");
        string senha = pedido.get_param_value("senha");
        bool sucesso = false;

        if(nome.size() == 0 || senha.size() == 0)
        {
            cout << "Preencha o nome e a senha." << endl;
        }
        else
        {
            usuario* nova_conta = usuarios.criar(nome, senha);

            if(nova_conta != nullptr)
            {
                cout << "Conta criada com o id " << nova_conta->GetId() << ". Agora é só entrar." << endl;
                sucesso = true;
            };
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // sair da conta
    servidor.Post("/api/sair", [&](const httplib::Request&, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        if(id_logado != -1)
        {
            id_logado = -1;
            cout << "Você saiu da conta." << endl;
        };

        resposta.set_content("{\"ok\": true, \"conta\": null, \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // ===== Ações da conta logada =====
    // cada ação chama o método do usuario e depois salva a conta no banco

    // comprar ou pegar um jogo: forma=saldo ou forma=cartao&cartao=0
    servidor.Post("/api/comprar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        jogo_base* jogo = loja.buscar_por_id(atoi(pedido.get_param_value("id").c_str()));

        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else if(jogo == nullptr)
        {
            cout << "Esse jogo não existe." << endl;
        }
        else if(pedido.get_param_value("forma") == "cartao")
        {
            sucesso = conta->adquirir_com_cartao(*jogo, atoi(pedido.get_param_value("cartao").c_str()));
        }
        else
        {
            sucesso = conta->adquirir(*jogo);
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    // instalar, desinstalar e jogar usam o id do jogo
    servidor.Post("/api/instalar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else
        {
            sucesso = conta->instalar(atoi(pedido.get_param_value("id").c_str()));
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    servidor.Post("/api/desinstalar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else
        {
            sucesso = conta->desinstalar(atoi(pedido.get_param_value("id").c_str()));
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    servidor.Post("/api/jogar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else
        {
            sucesso = conta->jogar(atoi(pedido.get_param_value("id").c_str()),
                                   atoi(pedido.get_param_value("horas").c_str()));
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    // reembolso: as regras (14 dias, menos de 2 horas, devolver no saldo ou no cartão) ficam no usuario
    servidor.Post("/api/reembolsar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        jogo_base* jogo = loja.buscar_por_id(atoi(pedido.get_param_value("id").c_str()));

        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else if(jogo == nullptr)
        {
            cout << "Esse jogo não existe." << endl;
        }
        else
        {
            sucesso = conta->reembolsar(*jogo);
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    // ===== Carteira =====

    // lê um número digitado na tela, aceitando vírgula ou ponto nos decimais
    auto ler_valor = [](string texto) -> double
    {
        for(size_t i = 0; i < texto.size(); i++)
        {
            if(texto[i] == ',')
            {
                texto[i] = '.';
            };
        };
        return atof(texto.c_str());
    };

    // depósito via pix: usa o depositar_pix do usuario (confere a chave e desconta a taxa)
    servidor.Post("/api/pix", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else
        {
            double saldo_antes = conta->GetSaldo();
            conta->depositar_pix(ler_valor(pedido.get_param_value("valor")), pedido.get_param_value("chave"));

            // o depositar_pix não devolve nada: se o saldo subiu, deu certo
            sucesso = conta->GetSaldo() > saldo_antes;
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    // cadastro de cartão: mesmas validações do menu
    servidor.Post("/api/cartoes", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        string numero = pedido.get_param_value("numero");
        string validade = pedido.get_param_value("validade");
        int cvc = atoi(pedido.get_param_value("cvc").c_str());
        double limite = ler_valor(pedido.get_param_value("limite"));

        // tira espaços do número (ex.: "1234 5678 ...")
        string so_digitos = "";
        for(size_t i = 0; i < numero.size(); i++)
        {
            if(numero[i] != ' ')
            {
                so_digitos += numero[i];
            };
        };

        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else if(so_digitos.size() < 4 || validade.size() == 0)
        {
            cout << "Preencha o número do cartão e a validade." << endl;
        }
        else if(cvc < 1 || cvc > 9999)
        {
            cout << "O CVC tem 3 ou 4 dígitos." << endl;
        }
        else if(limite <= 0)
        {
            cout << "O limite precisa ser maior que zero." << endl;
        }
        else
        {
            conta->adicionar_cartao(cartao_de_credito(so_digitos, cvc, validade, limite));
            sucesso = true;
        };

        if(sucesso == true)
        {
            usuarios.salvar(conta);
        };
        responder_conta(resposta, sucesso, captura);
    });

    // resgate de gift card: o repositório confere o código no banco, credita e salva
    servidor.Post("/api/giftcard", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        usuario* conta = conta_logada();
        string codigo = pedido.get_param_value("codigo");

        if(conta == nullptr)
        {
            cout << "Entre na conta primeiro." << endl;
        }
        else if(codigo.size() == 0)
        {
            cout << "Digite o código do gift card." << endl;
        }
        else
        {
            double saldo_antes = conta->GetSaldo();
            usuarios.resgatar_gift_card(conta, codigo);
            sucesso = conta->GetSaldo() > saldo_antes;
        };

        responder_conta(resposta, sucesso, captura);
    });

    // ===== Administração =====
    // assim como no menu do terminal, a administração não exige login

    // cadastrar jogo: tipo=gratuito ou tipo=pago (com preco)
    servidor.Post("/api/admin/jogos", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        string titulo = pedido.get_param_value("titulo");
        string usuario_jogo = pedido.get_param_value("usuario");
        string senha_jogo = pedido.get_param_value("senha");
        double tamanho = ler_valor(pedido.get_param_value("tamanho"));
        double preco = ler_valor(pedido.get_param_value("preco"));
        bool pago = pedido.get_param_value("tipo") == "pago";

        if(titulo.size() == 0 || usuario_jogo.size() == 0 || senha_jogo.size() == 0)
        {
            cout << "Preencha todos os campos obrigatórios." << endl;
        }
        else if(tamanho <= 0)
        {
            cout << "O tamanho precisa ser maior que zero." << endl;
        }
        else if(pago == true && preco <= 0)
        {
            cout << "Jogos pagos precisam de um preço maior que zero." << endl;
        }
        else
        {
            // o catálogo guarda ponteiros para a classe base; cada tipo é criado com sua classe
            jogo_base* novo = nullptr;
            if(pago == true)
            {
                novo = new jogos_pagos(titulo, tamanho, senha_jogo, preco, usuario_jogo);
            }
            else
            {
                novo = new jogos_gratuitos(titulo, tamanho, senha_jogo, usuario_jogo);
            };

            // adicionar() recusa título repetido e cuida do delete nesse caso
            int id_novo = loja.adicionar(novo);
            if(id_novo != -1)
            {
                cout << "Jogo cadastrado com o id " << id_novo << "." << endl;
                sucesso = true;
            };
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // editar jogo: mesmas regras do atualizar_jogo do menu; senha vazia mantém a atual
    servidor.Post("/api/admin/jogos/editar", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        jogo_base* jogo = loja.buscar_por_id(atoi(pedido.get_param_value("id").c_str()));
        string titulo = pedido.get_param_value("titulo");
        string usuario_jogo = pedido.get_param_value("usuario");
        string senha_jogo = pedido.get_param_value("senha");
        double tamanho = ler_valor(pedido.get_param_value("tamanho"));
        double preco = ler_valor(pedido.get_param_value("preco"));

        // só jogos pagos têm preço (dynamic_cast devolve nullptr se o jogo for gratuito)
        jogos_pagos* jogo_pago = dynamic_cast<jogos_pagos*>(jogo);
        jogo_base* outro_com_titulo = loja.buscar_por_titulo(titulo);

        if(jogo == nullptr)
        {
            cout << "Esse jogo não existe." << endl;
        }
        else if(titulo.size() == 0 || usuario_jogo.size() == 0)
        {
            cout << "Preencha todos os campos obrigatórios." << endl;
        }
        else if(outro_com_titulo != nullptr && outro_com_titulo != jogo)
        {
            cout << "Já existe um jogo com o título: " << titulo << endl;
        }
        else if(tamanho <= 0)
        {
            cout << "O tamanho precisa ser maior que zero." << endl;
        }
        else if(jogo_pago != nullptr && preco <= 0)
        {
            cout << "Jogos pagos precisam de um preço maior que zero." << endl;
        }
        else
        {
            jogo->SetTitulo(titulo);
            jogo->SetTamanho(tamanho);
            jogo->SetConta(usuario_jogo);
            if(senha_jogo.size() > 0)
            {
                jogo->SetSenha(senha_jogo);
            };
            if(jogo_pago != nullptr)
            {
                jogo_pago->SetValor(preco);
            };

            loja.salvar(jogo);
            cout << "Jogo atualizado." << endl;
            sucesso = true;
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // remover jogo: o excluir_jogo recusa se alguma conta tiver o jogo
    servidor.Post("/api/admin/jogos/remover", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        bool sucesso = usuarios.excluir_jogo(loja, atoi(pedido.get_param_value("id").c_str()));

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // lista os gift cards
    servidor.Get("/api/admin/giftcards", [&](const httplib::Request&, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;

        vector <gift_card> lista = banco.listar_gift_cards();
        string json = "[";
        for(size_t i = 0; i < lista.size(); i++)
        {
            if(i > 0)
            {
                json += ", ";
            };
            json += "{\"codigo\": " + json_texto(lista[i].GetCodigo()) +
                    ", \"valor\": " + json_numero(lista[i].GetValor()) +
                    ", \"valido\": " + (lista[i].EstaValido() ? "true" : "false") + "}";
        };
        json += "]";

        resposta.set_content("{\"ok\": true, \"giftcards\": " + json +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // cria um gift card (o repositório recusa código repetido)
    servidor.Post("/api/admin/giftcards", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        string codigo = pedido.get_param_value("codigo");
        double valor = ler_valor(pedido.get_param_value("valor"));

        if(codigo.size() == 0)
        {
            cout << "Digite o código do gift card." << endl;
        }
        else if(valor <= 0)
        {
            cout << "O valor precisa ser maior que zero." << endl;
        }
        else if(usuarios.criar_gift_card(codigo, valor) == true)
        {
            cout << "Gift card " << codigo << " criado com valor de " << valor << " reais." << endl;
            sucesso = true;
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // lista as contas (sem senha)
    servidor.Get("/api/admin/contas", [&](const httplib::Request&, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);

        const vector <usuario*>& contas = usuarios.GetContas();
        string json = "[";
        for(size_t i = 0; i < contas.size(); i++)
        {
            if(i > 0)
            {
                json += ", ";
            };
            json += "{\"id\": " + to_string(contas[i]->GetId()) +
                    ", \"nome\": " + json_texto(contas[i]->Getnome()) +
                    ", \"saldo\": " + json_numero(contas[i]->GetSaldo()) +
                    ", \"jogos\": " + to_string(contas[i]->GetBiblioteca().size()) + "}";
        };
        json += "]";

        resposta.set_content("{\"ok\": true, \"contas\": " + json + ", \"mensagens\": []}", "application/json");
    });

    // remove uma conta; precisa da senha dela, como no menu
    servidor.Post("/api/admin/contas/remover", [&](const httplib::Request& pedido, httplib::Response& resposta)
    {
        lock_guard<mutex> trava(trava_api);
        captura_cout captura;
        bool sucesso = false;

        int id_conta = atoi(pedido.get_param_value("id").c_str());
        usuario* conta = usuarios.buscar_por_id(id_conta);

        if(conta == nullptr)
        {
            cout << "Não existe conta com o id: " << id_conta << endl;
        }
        else if(conta->verificar_senha(pedido.get_param_value("senha")) == false)
        {
            cout << "Senha incorreta." << endl;
        }
        else
        {
            sucesso = usuarios.remover(id_conta);

            // se a conta removida era a logada, ninguém fica logado
            if(sucesso == true && id_conta == id_logado)
            {
                id_logado = -1;
            };
        };

        resposta.set_content("{\"ok\": " + string(sucesso ? "true" : "false") + ", \"conta\": " + conta_json() +
                             ", \"mensagens\": " + captura.mensagens_json() + "}", "application/json");
    });

    // primeiro reserva a porta; se outro servidor já estiver usando, avisa e encerra
    if(servidor.bind_to_port("localhost", 8080) == false)
    {
        cout << "Não foi possível usar a porta 8080. Já existe outro servidor rodando?" << endl;
        return 1;
    };

    cout << "Interface disponível em http://localhost:8080" << endl;
    cout << "Para encerrar, aperte Ctrl + C." << endl;

    servidor.listen_after_bind();

    return 0;
}
