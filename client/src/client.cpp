// C++ Program for Message Queue (Writer Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <unistd.h>
#include "../../shared/include/msg_t.hpp"

using namespace std;

int main()
{
    msg_t message;
    key_t key;
    int msgid;
    string game_id;
    int client_pid = (int) getpid();

    // ftok to generate unique key
    key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.msg_type = 1;

    // Envia PID
    snprintf(message.msg_text, 6, "%d", client_pid);

    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
    cout << "Client connected " << client_pid << endl;

    message.msg_type = client_pid;

    // Get game session PID
    msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), client_pid, 0);
    
    cout << message.msg_text << endl;
    message.msg_type = message.session_id;
    message.sender_id = (int) getpid();
    // todo: Game loop
    string txt;
    while(true){ // im just testing if i can correnctly send messages between clients and session threads :)
        cout << "Mensagem a enviar: ";
        getline(cin, txt);

        snprintf(message.msg_text, 1000, "%s", txt.c_str());
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
    }
        

    return 0;
}
