#include <iostream>
#include <string>
#include <vector>
using namespace std;

class jogos_gratuitos{
    private:
        string conta;
        string senha_conta;

    protected:
        string titulo;
        double tamanho;
        int horas_jogadas = 0;

    public:
        jogos_gratuitos(string nome, double peso, string senha, string usuario = "Player 1"){
            this->titulo = nome;
            this->tamanho = peso;
            this->senha_conta = senha;
            this->conta = usuario;
        };

        virtual ~jogos_gratuitos(){
            cout << "O jogo: " << titulo << " foi desinstalado." << endl;
        };

        void SetTitulo(string nome){
            this->titulo = nome;
        };

        void SetTamanho(double peso){
            this->tamanho = peso;
        };

        void SetHoras(int tempo){
            this-> horas_jogadas += tempo;
        };

        void SetConta(string usuario){
            this->conta = usuario;
        };

        void SetSenha(string senha){
            this->senha_conta = senha;
        };

        string GetTitulo(){
            return titulo;
        };

        double GetTamanho(){
            return tamanho;
        };

        int GetHoras(){
            return horas_jogadas;
        };

        string GetConta(){
            return conta;
        };

        string GetSenha(){
            return senha_conta;
        };

        void Display(){
            cout << "O jogo: " << titulo << "pesa: " << tamanho << " Gigabytes\n" << "Suas horas jogadas: " << horas_jogadas << endl;
        };

    };

    class jogos_pagos : public jogos_gratuitos {
        private:
            int valor;
            bool comprado = false;


        public:

            jogos_pagos(string nome, double peso, string senha, int preco, string usuario = "Player 1") : jogos_gratuitos(nome, peso, senha, usuario){
                this->valor = preco;
            };
            
            ~jogos_pagos(){
                cout << "O jogo: " << titulo << " foi desisntalado, e os seus " << valor << "reais foram rembolsados." << endl;
            };

            void SetValor(int preco){
                this->valor = preco;
            };

            void comprar(){
                if(comprado == false){
                    cout << "Você comprou o jogo: " << titulo << endl;
                    comprado = true;
                }
                else{
                    cout << "Você já tem esse jogo." << endl;
                };
            };

            void rembolsar(){
                if(comprado == false){
                    cout << "Você não possui esse jogo: " << titulo << endl;
                }
                else{
                    cout << "O jogo: " << titulo << " foi reembolsado" << endl;  
                    comprado = false;              
                };
            };

            int Getvalor(){
                return valor;
            };

            bool GetEstado(){
                return comprado;
            };


    };