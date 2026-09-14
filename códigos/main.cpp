#include <iostream>
#include <string>
#include <vector>
#include "tipos_jogos.hpp"
#include "metodos_pagamento.hpp"
using namespace std;


class usuario{
    private:
        string senha;
        long numero_de_usuário;
        double saldo = 0;
        vector <cartao_de_credito> cartoes_cadastrados;
    protected:
        string nome;
        int jogos_instalados = 0;
        int jogos_comprados = 0;
        vector <jogos_gratuitos*> jogos_gratis;
        vector <jogos_pagos*> jogos_comprados;
    public:

        usuario(string nick, long numero, string password){
            this->nome = nick;
            this->numero_de_usuário = numero;
            this->senha = password;
            cout << "Seja bem vindo a nossa biblioteca de jogos." << endl;
        };

        ~usuario(){
            cout << "Usuário deletado da biblioteca de jogos." <<  endl;
        };

        void SetNome(string nick){
            this->nome = nick;
            cout << "Seu nome foi alterado para: " << nick << endl;
        };

        string Getnome(){
            return nome;
        };

        void installar(jogos_gratuitos& jogo){
            jogos_gratis.emplace_back(&jogo);
            cout << "O jogo: " << jogo.GetTitulo() << " Foi instalado com sucesso." << endl;
        };

        void desintallar(string titulo){
            bool existe = false;
            for(int i = 0; i < jogos_instalados; i++){
                if((*jogos_gratis[i]).GetTitulo() == titulo){
                    jogos_gratis.erase(jogos_gratis.begin() + i);
                    cout << "O jogo: " << titulo << " foi desinstalado com sucesso." << endl;
                    existe = true;
                    break;
                };
            };
            if(existe == false){
                cout << "O jogo: " << titulo << " não está instalado ou não existe na biblioteca." << endl;
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








