#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random> 

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() {
        // Inicializar o tabuleiro e as variáveis do jogo
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                board[i][j] = ' ';
            }
        }
        winner = ' ';
        game_over = 0;
        current_player = 'O';

    }

    void display_board() {
        // Exibir o tabuleiro no console
        std::system("clear");
        for(int i = 0; i < 3; i++){
            //std::cout<<board[i][0]<< "|" <<board[i][1]<< "|" <<board[i][2]<<"\n";
            printf("%c|%c|%c \n", board[i][0], board[i][1],board[i][2]);
            if(i != 2){
                printf("-----\n");
            }
        }
    }

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos

        std::unique_lock<std::mutex>  lock(board_mutex);
        while(player != current_player){
            turn_cv.wait(lock);
        }

        if(!is_game_over()){
            if(board[row][col] != 'X' && board[row][col] != 'O'){
                board[row][col] = player;
                display_board();
                if(player == 'O'){
                    current_player = 'X';
                }else{
                    current_player = 'O';
                }
                turn_cv.notify_one();
                board_mutex.unlock();
                return 1;
            }else{
                return 0;
            }
        }else{
            turn_cv.notify_one();
            board_mutex.unlock();
            return 1;
        }

        
    }

 /*    bool check_win(char player) {
        // Verificar se o jogador atual venceu o jogo
        
        
    } */

    bool check_draw() {
        // Verificar se houve um empate
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(board[i][j] == ' '){
                    return 0;
                }
            }
        }
        return 1;
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        if(check_draw()){
            return 1;
        }else{
            return 0;
        }
    }

    /* char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        // linhas
        for(int i = 0; i < 3; i++){
            if(board[i][0] == board[i][1] && board[i][1] == board[i][2]){
                return board[i][0];
            }
        }
        // colunas
        for(int i = 0; i < 3; i++){
            if(board[0][i] == board[1][i] && board[i][1] == board[2][i]){
                return board[i][0];
            }
        }
        // diagonal
        if(board[0][0] == board[1][1] && board[1][1] == board[2][2]){
                return board[1][1];
        }
        if(board[0][2] == board[1][1] && board[1][1] == board[2][0]){
                return board[1][1];
        }
        if (check_draw()){
            return 'D';
        }
        return 0;
        
    }   */
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida
        while(!game.is_game_over()){
            if(strategy == "sequential"){
                play_sequential();
            }else{
                play_random();
            }
        }
    }

private:
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(game.make_move(symbol, i, j)){
                    return;
                }
            }
        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distr(0, 2);
        int l = distr(gen);
        int c = distr(gen);
        game.make_move(symbol, l, c);
    } 
};  

// Função principal

int main() {
    // Inicializar o jogo e os jogadores

    TicTacToe tabuleiro;
    tabuleiro.display_board();
    Player X(tabuleiro, 'X', "random");
    Player O(tabuleiro, 'O', "sequential");

    // Criar as threads para os jogadores

    std::thread Jogador1(&Player::play, &O);
    std::thread Jogador2(&Player::play, &X);

    // Aguardar o término das threads

    Jogador1.join();
    Jogador2.join();

    // Exibir o resultado final do jogo

    std::cout<<"\n Fim de Jogo!\n";

    return 0;
}
