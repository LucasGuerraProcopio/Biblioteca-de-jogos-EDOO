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
            cartao_de_credito(string numero_credito_novo, int cvc_credito_novo, string validade_credito_novo, double limite_credito_novo)
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
        private:
            string chave_pix;
            double taxa_da_biblioteca = 0.02;
        public:

            pix(string chave = "46302855752", double taxa = 0.02){
                this->chave_pix = chave;
                this->taxa_da_biblioteca = taxa;
            };

            string GetChave(){
            return chave_pix;
            };

            void SetTaxa(double nova_taxa){
                this->taxa_da_biblioteca = nova_taxa;
            };

            double GetTaxa(){
                return taxa_da_biblioteca;
            };
    };


    // Classe do gift card
    class gift_card
    {
        private:
            string codigo;
            double valor;
            bool valido = true;
        public:

            gift_card(){
                this->codigo = "1234 5678 9000";
                this->valor = 100.00;
            };

            gift_card(string numero, double dinheiro){
                this->codigo = numero;
                this->valor = dinheiro;
            };

            string GetCodigo(){
                return codigo;
            };

            double GetValor(){
                return valor;
            };

            void GiftCardRegistrado(){
                this->valido = false;
            };
    };