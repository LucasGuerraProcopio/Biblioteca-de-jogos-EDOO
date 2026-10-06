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

## Conceitos de POO usados
- **Classes e encapsulamento:** atributos `private` e `protected` com getters e setters.
- **Herança:** `jogos_gratuitos` e `jogos_pagos` herdam de `jogo_base`.
- **Polimorfismo e classe abstrata:** `jogo_base` tem funções virtuais puras (`GetPreco` e `EhPago`), implementadas com `override` nas classes filhas.
- **Padrão de projeto Strategy:** `forma_de_pagamento` é uma interface (classe abstrata) com duas estratégias, `pagamento_saldo` e `pagamento_cartao`. O usuário compra e reembolsa sem saber qual delas está usando.
- **Ponteiros e referências:** o catálogo e o repositório guardam `vector` de ponteiros, e as formas de pagamento recebem o saldo e o cartão por referência.
- **Sobrecarga de operadores:** `<`, `>` e `==` em `jogo_base`.

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
| `sqlite3.c` e `sqlite3.h` | biblioteca SQLite |

## Como compilar e executar
Precisa do compilador `g++` (MinGW no Windows). Na pasta raiz do projeto:

**Windows (PowerShell)**
```
gcc -c códigos/sqlite3.c -o sqlite3.o
g++ -std=c++11 códigos/main.cpp sqlite3.o -o jogos.exe
.\jogos.exe
```

**Linux**
```
gcc -c códigos/sqlite3.c -o sqlite3.o
g++ -std=c++11 códigos/main.cpp sqlite3.o -ldl -lpthread -o jogos
./jogos
```
O `sqlite3.c` precisa ser compilado com `gcc` (não `g++`), e só uma vez. O arquivo `biblioteca.db` é criado ao lado do programa na primeira execução.

## Como usar
1. No menu principal, cadastre uma conta (opção 4) e entre nela (opção 10).
2. Deposite via pix (opção 2) usando a chave `gcromeiro@gmail.com`, ou cadastre um cartão (opção 3).
3. Compre jogos (opção 4), instale (opção 5) e jogue (opção 9).
4. Reembolse (opção 7) dentro das regras acima.
5. Para testar gift cards, crie um no menu principal (opção 11) e resgate na conta (opção 8).