#include <iostream>
#include <string>

class jogos_gratuitos{
    private:
        std::string titulo;
        double tamanho;
        int horas_jogadas = 0;

    public:
        jogos_gratuitos(std::string nome, double peso){
            this->titulo = nome;
            this->tamanho = peso;
        };

        ~jogos_gratuitos(){
            std::cout << "O jogo: " << titulo << " foi desinstalado." << std::endl;
        };

        void SetTitulo(std::string nome){
            this->titulo = nome;
        };

        void SetTamanho(double peso){
            this->tamanho = peso;
        };

        void SetHoras(int tempo){
            this-> horas_jogadas += tempo;
        };

        std::string GetTitulo(){
            return titulo;
        };

        double GetTamanho(){
            return tamanho;
        };

        int GetHoras(){
            return horas_jogadas;
        };

        void Display(){
            std::cout << "O jogo: " << titulo << "pesa: " << tamanho << " Gigabytes\n" << "Suas horas jogadas: " << horas_jogadas << std::endl;
        };

    };

    class jogo_pagos : public jogos_gratuitos {
        private:
            int valor;
            bool comprado;


        public:

            jogo_pagos(std::string nome, double peso, int preco, bool adiquirido) : jogos_gratuitos(nome, peso){
                this->valor = preco;
                this->comprado = adiquirido;
            };



    };

int main(){

    jogos_gratuitos Fortinite("Fortinite", 90);
    jogos_gratuitos R6("Rainbow six siege", 75);
    jogos_gratuitos Minecraft("Minecraft", 35);
    jogos_gratuitos MK("Mortal Kombat", 80);
    jogos_gratuitos Fifa("FC 26", 80);
    jogos_gratuitos Roblox("Roblox", 5);
    jogos_gratuitos FNAF("Five nights at Freddy", 10);
    jogos_gratuitos Gta("Grand Theft Auto 6", 220);
}








