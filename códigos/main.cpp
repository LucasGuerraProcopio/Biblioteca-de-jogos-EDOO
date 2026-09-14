#include <iostream>
#include <string>
#include <vector>

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

class cartao_de_credito{
    private:
        long numero_do_cartao;
        int cvc;
        std::string validade;
        double limite;
        double gastos = 0;
    public:

        cartao_de_credito(long numero, int vcv, std::string tempo, double dinheiro){
            this->numero_do_cartao = numero;
            this->cvc = vcv;
            this->validade = tempo;
            this->limite = dinheiro;
        };

        ~cartao_de_credito(){
            std::cout << "Esse cartão não está mais registrado na biblioteca de jogos." << std::endl;
        };

        void gastar(double valor){
            if((gastos + valor) > limite ){
                std::cout << "Você ultrapassou seu limite, compra negada" << std::endl;
            }
            else{
                gastos += valor;
                std::cout << "Compra aprovada no valor de: " << valor << "\n Gastos totais com esse cartão na biblioteca de jogos: " << valor << std::endl;
            };
        };

};


class usuario{
    private:
        std::string senha;
        long numero_de_usuário;
        double saldo = 0;
        std::vector <cartao_de_credito> cartoes_cadastrados;
    protected:
        std::string nome;
        int jogos_instalados = 0;
        int jogos_comprados = 0;
        std::vector <jogos_gratuitos*> jogos_gratis;
        std::vector <jogos_pagos*> jogos_comprados;
    public:

        usuario(std::string nick, long numero, std::string password){
            this->nome = nick;
            this->numero_de_usuário = numero;
            this->senha = password;
            std::cout << "Seja bem vindo a nossa biblioteca de jogos." << std::endl;
        };

        ~usuario(){
            std::cout << "Usuário deletado da biblioteca de jogos." <<  std::endl;
        };

        void SetNome(std::string nick){
            this->nome = nick;
            std::cout << "Seu nome foi alterado para: " << nick << std::endl;
        };

        std::string Getnome(){
            return nome;
        };

        void ColocarSaldo(int valor, std::string forma, std::string codigo = 0){
            if(forma == "pix"){
                saldo += valor;
            }
            else if(forma == "gift card"){
                if(codigo.length() == 12){
                    saldo += valor;
                    std::cout << "Um gift card no valor de: " << valor << "reais foi adicionado ao seu saldo" << std::endl;
                }
                else{
                    std::cout << "Código de gift card inválido" << std::endl;
                };
            }
            else{
                std::cout << "Formato de pagamento invalido" << std::endl;
            };

        };

        void installar(jogos_gratuitos& jogo){
            jogos_gratis.emplace_back(&jogo);
            std::cout << "O jogo: " << jogo.GetTitulo() << " Foi instalado com sucesso." << std::endl;
        };

        void desintallar(std::string titulo){
            bool existe = false;
            for(int i = 0; i < jogos_instalados; i++){
                if((*jogos_gratis[i]).GetTitulo() == titulo){
                    jogos_gratis.erase(jogos_gratis.begin() + i);
                    std::cout << "O jogo: " << titulo << " foi desinstalado com sucesso." << std::endl;
                    existe = true;
                    break;
                };
            };
            if(existe == false){
                std::cout << "O jogo: " << titulo << " não está instalado ou não existe na biblioteca." << std::endl;
            };
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
};








