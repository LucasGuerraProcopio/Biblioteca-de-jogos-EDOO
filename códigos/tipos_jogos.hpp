#include <iostream>
#include <string>   
using namespace std;


// Classe dos jogos gratuitos
class jogos_gratuitos
{
    // Informações sobre o jogo        
    protected:
        string titulo_jogo;
        double tamanho_jogo;
        int horas_jogadas = 0;


    // Login e senha do jogo
    private:
        string senha_jogo;
        string usuario_jogo;


    // Operações sobre os jogos gratuitos do usuario
    public:

        jogos_gratuitos(){
            this->titulo_jogo = "Jogo sem nome";
            this->tamanho_jogo = 1;
            this->usuario_jogo = "New_Player123";
            cout << "Escolha uma senha!" << endl;
            cin >> senha_jogo;
        };

        // Construtor do jogo gratuito a ser adicionado
        jogos_gratuitos(string titulo_jogo_gratuito, double tamanho_jogo_gratuito, string senha_gratuito, string usuario_gratuito = "Player 1")
        {
            this->titulo_jogo = titulo_jogo_gratuito;
            this->tamanho_jogo = tamanho_jogo_gratuito;
            this->senha_jogo = senha_gratuito;
            this->usuario_jogo = usuario_gratuito;
        };

        jogos_gratuitos(const jogos_gratuitos &jogoantigo){
            this->titulo_jogo = jogoantigo.titulo_jogo;
            this->tamanho_jogo = jogoantigo.tamanho_jogo;
            this->senha_jogo = jogoantigo.senha_jogo;
            this->usuario_jogo = jogoantigo.usuario_jogo;
            this->horas_jogadas = jogoantigo.horas_jogadas;
        };


        // Destrutor para desinstalar um jogo
        virtual ~jogos_gratuitos()
        {
            cout << "O jogo: " << titulo_jogo << " foi desinstalado." << endl;
        };

        bool operator<(const jogos_gratuitos &outro){
            return tamanho_jogo < outro.tamanho_jogo;
        };

        bool operator>(const jogos_gratuitos &outro){
            return tamanho_jogo > outro.tamanho_jogo;
        };

        bool operator==(const jogos_gratuitos &outro){
            return tamanho_jogo == outro.tamanho_jogo;
        };

        
        // A cada bloco temos funções que trabalham em conjunto
        // Titulo do jogo
        void SetTitulo(string titulo_jogo_gratuito)
        {
            this->titulo_jogo = titulo_jogo_gratuito;
        };
        string GetTitulo()
        {
            return titulo_jogo;
        };


        // Tamanho do jogo
        void SetTamanho(double tamanho_jogo_gratuito)
        {
            this->tamanho_jogo = tamanho_jogo_gratuito;
        };
        double GetTamanho()
        {
            return tamanho_jogo;
        };


        // Horas jogadas
        void SetHoras(int novas_horas_jogadas)
        {
            this-> horas_jogadas += novas_horas_jogadas;
        };
        int GetHoras()
        {
            return horas_jogadas;
        };


        // Senha do jogo
        void SetSenha(string senha_gratuito)
        {
            this->senha_jogo = senha_gratuito;
        };
        string GetSenha()
        {
            return senha_jogo;
        };


        // Usuario do jogo
        void SetConta(string usuario_gratuito)
        {
            this->usuario_jogo = usuario_gratuito;
        };
        string GetConta()
        {
            return usuario_jogo;
        };


        // Função das informações
        void mostrar_informacoes()
        {
            cout << "O jogo: " << titulo_jogo << " pesa: " << tamanho_jogo << " Gigabytes\n" << "Suas horas jogadas: " << horas_jogadas << endl;
        };
    };


    // Classe dos jogos pagos que herda da classe dos jogos gratuitos
    class jogos_pagos : public jogos_gratuitos 
    {
        // Informações da compra do jogo pago
        private:
            int valor_jogo;
            bool jogo_comprado = false;

        
        // Operações com os jogos pagos
        public:
            // Construtor do jogo pago a ser adicionado. Como essa classe herda da classe dos jogos gratuitos, não precisa reescrever o construtor todo
            jogos_pagos(string titulo_jogo_pago, double tamanho_jogo_pago, string senha_pago, int preco_jogo, string usuario_pago = "Player 1") : jogos_gratuitos(titulo_jogo_pago, tamanho_jogo_pago, senha_pago, usuario_pago)
            {
                this->valor_jogo = preco_jogo;
            };
            

            // Destrutor para desinstalar um jogo
            ~jogos_pagos()
            {
                cout << "O jogo: " << titulo_jogo << " foi desisntalado, e os seus " << valor_jogo << "reais foram rembolsados." << endl;
            };


            // A cada bloco temos funções que trabalham em conjunto
            // Valor do jogo
            void SetValor(int preco_jogo)
            {
                this->valor_jogo = preco_jogo;
            };
            int Getvalor()
            {
                return valor_jogo;
            };


            // Jogo comprado
            void comprar()
            {
                if(jogo_comprado == false)
                {
                    cout << "Você comprou o jogo: " << titulo_jogo << endl;
                    jogo_comprado = true;
                }
                else
                {
                    cout << "Você já tem esse jogo." << endl;
                };
            };
            bool GetEstado()
            {
                return jogo_comprado;
            };

            
            // Função para reembolsar o jogo
            void rembolsar()
            {
                if(jogo_comprado == false)
                {
                    cout << "Você não possui esse jogo: " << titulo_jogo << endl;
                }
                else
                {
                    cout << "O jogo: " << titulo_jogo << " foi reembolsado" << endl;  
                    jogo_comprado = false;              
                };
            };
    };