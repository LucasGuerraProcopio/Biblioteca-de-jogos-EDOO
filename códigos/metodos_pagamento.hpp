
#include <iostream>
#include <string>
#include <vector>
using namespace std;




class cartao_de_credito{
    private:
        long numero_do_cartao;
        int cvc;
        string validade;
        double limite;
        double gastos = 0;
    public:

        cartao_de_credito(long numero, int vcv, string tempo, double dinheiro){
            this->numero_do_cartao = numero;
            this->cvc = vcv;
            this->validade = tempo;
            this->limite = dinheiro;
        };

        ~cartao_de_credito(){
            cout << "Esse cartão não está mais registrado na biblioteca de jogos." << endl;
        };

        void gastar(double valor){
            if((gastos + valor) > limite ){
                cout << "Você ultrapassou seu limite, compra negada" << endl;
            }
            else{
                gastos += valor;
                cout << "Compra aprovada no valor de: " << valor << "\n Gastos totais com esse cartão na biblioteca de jogos: " << valor << endl;
            };
        };

};

class pix {

};


class gift_card{

};