    #include <iostream>
    #include <string>
    #include <vector>
    using namespace std;


    // Classe do cartão de credito
    class cartao_de_credito
    {
        // Informações do cartão
        private:
            string numero_credito; 
            int cvc_credito;
            string validade_credito;
            double limite_credito;
            double gastos_credito = 0;


        // Operaçoes com o cartão de credito
        public:
            // Construtor do novo cartão de credito
            cartao_de_credito(long numero_credito_novo, int cvc_credito_novo, string validade_credito_novo, double limite_credito_novo)
            {
                this->numero_credito = numero_credito_novo;
                this->cvc_credito = cvc_credito_novo;
                this->validade_credito = validade_credito_novo;
                this->limite_credito = limite_credito_novo;
            };


            // Destrutor para apagar um cartão de credito
            ~cartao_de_credito() 
            {
                cout << "Esse cartão não está mais registrado na biblioteca de jogos." << endl;
            };


            // Função ou Metodo que verifica se é possivel realizar a compra
            void gastar(double valor_compra)
            {
                if((gastos_credito + valor_compra) > limite_credito)
                {
                    cout << "Você ultrapassou seu limite, compra negada" << endl;
                }
                else
                {
                    gastos_credito += valor_compra;
                    cout << "Compra aprovada no valor de: " << valor_compra << "\n Gastos totais com esse cartão na biblioteca de jogos: " << gastos_credito << endl;
                };
            };

    };


    // Classe do pix
    class pix 
    {

    };


    // Classe do gift card
    class gift_card
    {

    };