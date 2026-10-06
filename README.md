# Biblioteca de jogos - EDOO
Nosso projeto consiste em uma biblioteca de jogos, a qual tem a finalidade de usar os recursos de POO na linguagem C++, com o intuito de prática e aplicação de todos os recursos aprendidos na disciplina de Estrutura de Dados Orientadas a Objetos.

### Integrantes do Grupo:
- Eduardo Cabral Cordeiro dos Santos
- Gabriel Costa Romeiro dos Santos
- Lucas Guerra Procopio

## Funcionalidades
- **Jogos (CRUD):** cadastrar, listar, buscar (por id ou parte do título), atualizar (título, tamanho, preço, usuário e senha do jogo) e remover. Um jogo só é removido se nenhuma conta o possuir.
- **Contas (CRUD):** cadastrar, listar, atualizar nome e senha, e remover. As senhas ficam salvas como hash, nunca em texto puro.
- **Conta logada:** biblioteca, depósito via pix, cadastro de cartão de crédito, compra com saldo ou cartão, instalar, desinstalar, jogar, reembolso e resgate de gift card.
- **Reembolso:** só até 14 dias depois da compra e com menos de 2 horas jogadas. O dinheiro volta para a mesma forma de pagamento usada na compra
- **Valores decimais:** aceitam ponto ou vírgula.
- **Persistência:** tudo é salvo em um banco SQLite (`biblioteca.db`), criado na pasta do programa.
- **Duas formas de usar:** o menu no terminal (`main.cpp`) e uma interface gráfica no navegador (`servidor.cpp`). As duas usam as mesmas classes e o mesmo banco.

## Interface gráfica
A interface tem 5 telas: catálogo (com busca, filtro e ordenação), entrar/criar conta, minha conta (biblioteca), carteira (pix, cartões e gift card) e administração (jogos, gift cards e contas).

- **Arquitetura:** o `servidor.cpp` transforma o programa em um pequeno servidor local (biblioteca [cpp-httplib](https://github.com/yhirose/cpp-httplib), um único arquivo `httplib.h`). A tela, feita em HTML, JavaScript e [Tailwind CSS](https://tailwindcss.com), só exibe os dados e chama a API; **toda a lógica (regras, buscas, ordenação, pagamentos) continua nas classes em C++**.
- **API:** caminhos como `/api/jogos`, `/api/entrar`, `/api/comprar` e `/api/reembolsar` chamam os mesmos métodos usados pelo menu e respondem em JSON.
- **Mensagens:** o que as classes escrevem no `cout` aparece no painel "Mensagens" da tela, sem mudar nenhuma classe. A classe `captura_cout` desvia o `cout` no construtor e o devolve no destrutor (RAII).

## Conceitos de POO usados
- **Classes e encapsulamento:** atributos `private` e `protected` com getters e setters.
- **Herança:** `jogos_gratuitos` e `jogos_pagos` herdam de `jogo_base`.
- **Polimorfismo e classe abstrata:** `jogo_base` tem funções virtuais puras (`GetPreco` e `EhPago`), implementadas com `override` nas classes filhas.
- **Padrão de projeto Strategy:** `forma_de_pagamento` é uma interface (classe abstrata) com duas estratégias, `pagamento_saldo` e `pagamento_cartao`. O usuário compra e reembolsa sem saber qual delas está usando.
- **Ponteiros e referências:** o catálogo e o repositório guardam `vector` de ponteiros, e as formas de pagamento recebem o saldo e o cartão por referência.
- **Sobrecarga de operadores:** `<`, `>` e `==` em `jogo_base`. O `<` é usado para ordenar o catálogo por tamanho na interface.
- **Padrão Repository:** `catalogo` e `repositorio_usuarios` concentram o acesso aos jogos e às contas, guardando e buscando os objetos e falando com o banco.
- **Gerenciamento de memória:** classes que apagam ponteiros no destrutor (`catalogo`, `repositorio_usuarios`, `banco_de_dados`) proíbem cópia com `= delete`, evitando *double free*.
- **RAII:** `captura_cout` (na interface) desvia o `cout` no construtor e restaura no destrutor.

## Estrutura dos arquivos
| Arquivo | O que tem |
|---|---|
| `main.cpp` | abre o banco, cadastra os jogos iniciais e chama o menu |
| `menu.hpp` | menus e telas do programa |
| `entrada.hpp` | leitura segura do que o usuário digita |
| `tipos_jogos.hpp` | classes `jogo_base`, `jogos_gratuitos` e `jogos_pagos` |
| `catalogo.hpp` | catálogo de jogos da loja |
| `usuario.hpp` | classes `usuario` e `repositorio_usuarios` |
| `metodos_pagamento.hpp` | cartão, pix, gift card e o padrão Strategy dos pagamentos |
| `banco_de_dados.hpp` | todas as operações com o SQLite |
| `seguranca.hpp` | hash das senhas |
| `jogos_iniciais.hpp` | os jogos cadastrados na primeira execução (usado pelo menu e pelo servidor) |
| `caminho.hpp` | encontra a pasta do programa, inclusive em pastas com acento no Windows |
| `servidor.cpp` | servidor da interface gráfica e os caminhos da API |
| `ferramentas_api.hpp` | montagem de JSON e a classe `captura_cout` |
| `web/index.html` e `web/app.js` | a interface gráfica (HTML, Tailwind e JavaScript) |
| `sqlite3.c` e `sqlite3.h` | biblioteca SQLite |
| `httplib.h` | biblioteca cpp-httplib (servidor local) |

## Como compilar e executar
Precisa do compilador `g++` (MinGW no Windows; no macOS, as ferramentas do Xcode: `xcode-select --install`). Todos os comandos são rodados na **pasta raiz do projeto**.

O `sqlite3.c` precisa ser compilado com `gcc` (não `g++`), e só uma vez:
```
gcc -c códigos/sqlite3.c -o sqlite3.o
```

### Menu no terminal
| Sistema | Compilar | Executar |
|---|---|---|
| Windows (PowerShell) | `g++ -std=c++11 códigos/main.cpp sqlite3.o -o jogos.exe` | `.\jogos.exe` |
| Linux | `g++ -std=c++11 códigos/main.cpp sqlite3.o -ldl -lpthread -o jogos` | `./jogos` |
| macOS | `g++ -std=c++11 códigos/main.cpp sqlite3.o -o jogos` | `./jogos` |

### Interface gráfica
| Sistema | Compilar | Executar |
|---|---|---|
| Windows (PowerShell) | `g++ -std=c++11 -D_WIN32_WINNT=0x0A00 códigos/servidor.cpp sqlite3.o -lws2_32 -o servidor.exe` | `.\servidor.exe` |
| Linux | `g++ -std=c++11 códigos/servidor.cpp sqlite3.o -ldl -lpthread -o servidor` | `./servidor` |
| macOS | `g++ -std=c++11 códigos/servidor.cpp sqlite3.o -o servidor` | `./servidor` |

Depois de executar, abra **http://localhost:8080** no navegador. Para encerrar o servidor, aperte `Ctrl + C` no terminal.

### Observações
- O arquivo `biblioteca.db` é criado ao lado do programa na primeira execução, e os jogos iniciais são cadastrados automaticamente.
- **Não use o menu e a interface ao mesmo tempo:** os dois leem o banco ao abrir, e um não fica sabendo das mudanças do outro.
- A interface carrega o Tailwind e as fontes pela internet, então o navegador precisa de conexão.
- Se aparecer "Não foi possível usar a porta 8080", já existe outro servidor aberto: feche-o antes.
- No Windows 11, o **Controle Inteligente de Aplicativos** pode bloquear o executável recém-compilado ("Uma política de Controle de Aplicativo bloqueou este arquivo"). Compilar de novo, ou com outro nome (`-o servidor2.exe`), costuma resolver.

## Como usar

### Interface gráfica
1. Em **Minha conta**, crie uma conta na aba "Criar conta" e entre com ela.
2. Na aba **Carteira**, deposite via pix (a chave aparece na tela) ou cadastre um cartão.
3. No **Catálogo**, selecione um jogo e compre com saldo ou cartão.
4. Na aba **Biblioteca**, instale, jogue e reembolse os jogos.
5. Em **Administração**, cadastre, edite e remova jogos, crie gift cards e remova contas.

### Menu no terminal
1. No menu principal, cadastre uma conta (opção 4) e entre nela (opção 10).
2. Deposite via pix (opção 2) usando a chave `gcromeiro@gmail.com`, ou cadastre um cartão (opção 3).
3. Compre jogos (opção 4), instale (opção 5) e jogue (opção 9).
4. Reembolse (opção 7) dentro das regras acima.
5. Para testar gift cards, crie um no menu principal (opção 11) e resgate na conta (opção 8).