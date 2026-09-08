#include <iostream>
#include <string>

class jogos{
    private:
        std::string titulo;
        double valor;
        double tamanho;
        int horas_jogadas = 0;

    public:
        jogos(std::string nome,double preco, double peso){
            this->titulo = nome;
            this->valor = preco;
            this->tamanho = peso;
        };

        ~jogos(){
            std::cout << "O jogo: " << titulo << " foi desinstalado." << std::endl;
        };

        void SetTitulo(std::string nome){
            this->titulo = nome;
        };

        void SetValor(double preco){
            this->valor = preco;
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

        double GetValor(){
            return valor;
        };

        double GetTamanho(){
            return tamanho;
        };

        int GetHoras(){
            return horas_jogadas;
        };

        void Displaay(){
            std::cout << "O jogo: " << titulo <<" custa: " << valor << " reais, e pesa: " << tamanho << " Gigabytes\n" << "Suas horas jogadas: " << horas_jogadas << std::endl;
        };

    };

int main(){

    jogos Fortinite("Fortinite", 0, 90);

}








