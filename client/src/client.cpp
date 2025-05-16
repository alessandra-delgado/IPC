// C++ Program for Message Queue (Writer Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <unistd.h>

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
    string game_id;
    int client_pid = (int) getpid();

    // ftok to generate unique key
    key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

    // Envia PID
    snprintf(message.mesg_text, 6, "%d", client_pid);

    msgsnd(msgid, &message, sizeof(message), 0);
    cout << "Client connected " << client_pid << endl;

    
    message.mesg_type = client_pid;

    // Get game session PID
    msgrcv(msgid, &message, sizeof(message), client_pid, 0);
    game_id = message.mesg_text;

    // Game loop
        cout << "GAME STARTED ID: " << game_id << endl;
        

    return 0;
}
