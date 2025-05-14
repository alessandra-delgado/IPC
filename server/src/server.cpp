





// create the message queue

// main loop:
// wait for connection


// assign player to session


// game running (loop)
// validate player commands
// send back response
// repeat until game ends --> close game session (remove from map) and keep running server

// if close server: disconnect all players safely (?) and shut down (close message queue).

#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int main()
{
    key_t key;
    int msgid;
    
    vector<string> waiting_players;

    // ftok to generate unique key
    key = ftok("tictactoe", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    while(true){
        // Recebe PID do cliente
        msgrcv(msgid, &message, sizeof(message), 1, 0);

        // Adicioanr novo cliente à lista de espera
        waiting_players.push_back(message.mesg_text);

        // Connect log message
        cout << "Cliente " << message.mesg_text << " conectado." << endl;
     
        // TEMOS JOGADORES SUFICIENTES PARA UM XOGO FIXE!!!!!!
        if(waiting_players.size() >= 2)
        {
            // PARA CADA DOIS JOGAORES, CRIAR UMA SESSÃO
            cout << "DOIS CLIENTE CONECTADO VAMO COMEÇAR O XOGO" << endl;
        }
    }


    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}


