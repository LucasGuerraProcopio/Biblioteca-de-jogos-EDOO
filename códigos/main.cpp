#include <iostream>
#include <string>

class jogos_gratuitos{
    private:
        std::string conta;
        std::string senha_conta;

    protected:
        std::string titulo;
        double tamanho;
        int horas_jogadas = 0;

    public:
        jogos_gratuitos(std::string nome, double peso, std::string senha, std::string usuario = "Player 1"){
            this->titulo = nome;
            this->tamanho = peso;
            this->senha_conta = senha;
            this->conta = usuario;
        };

        virtual ~jogos_gratuitos(){
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

        void SetConta(std::string usuario){
            this->conta = usuario;
        };

        void SetSenha(std::string senha){
            this->senha_conta = senha;
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

        std::string GetConta(){
            return conta;
        };

        std::string GetSenha(){
            return senha_conta;
        };

        void Display(){
            std::cout << "O jogo: " << titulo << "pesa: " << tamanho << " Gigabytes\n" << "Suas horas jogadas: " << horas_jogadas << std::endl;
        };

    };

    class jogos_pagos : public jogos_gratuitos {
        private:
            int valor;
            bool comprado = false;


        public:

            jogos_pagos(std::string nome, double peso, std::string senha, int preco, std::string usuario = "Player 1") : jogos_gratuitos(nome, peso, senha, usuario){
                this->valor = preco;
            };
            
            ~jogos_pagos(){
                std::cout << "O jogo: " << titulo << " foi desisntalado, e os seus " << valor << "reais foram rembolsados." << std::endl;
            };

            void SetValor(int preco){
                this->valor = preco;
            };

            void comprar(){
                if(comprado == false){
                    std::cout << "Você comprou o jogo: " << titulo << std::endl;
                    comprado = true;
                }
                else{
                    std::cout << "Você já tem esse jogo." << std::endl;
                };
            };

            void rembolsar(){
                if(comprado == false){
                    std::cout << "Você não possui esse jogo: " << titulo << std::endl;
                }
                else{
                    std::cout << "O jogo: " << titulo << " foi reembolsado" << std::endl;  
                    comprado = true;              
                };
            };

            int Getvalor(){
                return valor;
            };

            bool GetEstado(){
                return comprado;
            };

    };

int main(){
    jogos_gratuitos Fortinite("Fortinite", 90, "Torres_tortas", "Embananado123");
    jogos_gratuitos Roblox("Roblox", 5, "29/01/2021", "Tripa Boy");
    jogos_gratuitos FNAF("Five nights at Freddy", 10, "FFCBG123456");
    jogos_gratuitos Brawl("Brawl Stars", 0.8, "HIPERCARGA67", "Piriquito Deuz");
    jogos_pagos R6("Rainbow six siege", 70, "Odeio_escudos123", 50);
    jogos_pagos Minecraft("Minecraft", 35, "I am Steve", 40, "Eduardo Cabral");
    jogos_pagos MK("Mortal Kombat", 80, "Get over here", 200);
    jogos_pagos Fifa("FC 26", 80, "Futebol123", 500);
    jogos_pagos Gta("Grand Theft Auto 6", 220, "Rockstar mercenaria", 550, "CJ");
}








