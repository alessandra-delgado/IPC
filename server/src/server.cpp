// C++ Program for Message Queue (Reader Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[6];
} message;

int main()
{
    key_t key;
    int msgid;
    
    int games = 2;
    
    vector<int> waiting_players;

    // ftok to generate unique key
    key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Dispatcher -> the thread that takes care of routing clients to a session
    
    // Each game session is taken care of by it's own thread

    // So for the server, we'll have 1 + n/2 threads, where n is the number of clients
    while(true){
        
        cout << "A ESPERAR CONEXÃO" << endl;

        // Recebe PID do cliente
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        
        // Adicioanr novo cliente à lista de espera
        waiting_players.push_back(stoi(message.mesg_text));
        
        // Connect log message
        cout << "Cliente " << message.mesg_text << " conectado." << endl;

        // Clear msg
        snprintf(message.mesg_text, 6, "%s", "00000");
     
        // Para cada dois jogadores criar uma sessão
        while(waiting_players.size() >= 2) {
            cout << "A INICIAR JOGO" << endl;

            // Remove player 1 from waiting list
            int player1 = waiting_players.back();
            waiting_players.pop_back();
            // Remove player 2 from waiting list
            int player2 = waiting_players.back();
            waiting_players.pop_back();

                
            // Send game session
            snprintf(message.mesg_text, 100, "%d", games);
                
            // Envia PID ao jogador 1
            message.mesg_type = player1;
            msgsnd(msgid, &message, sizeof(message), 0);
                
            // Envia PID ao jogador 2
            message.mesg_type = player2;
            msgsnd(msgid, &message, sizeof(message), 0);
                
            cout << "A iniciar o jogo dos clientes: " << player1 << " : " << player2 << " GAME: " << games <<endl;
            // todo: Criar nova thread thread e executar o jogo.
            
            // Increment game count
            games++;
        }
    }

    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
