// C++ Program for Message Queue (Writer Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "../../shared/include/msg_t.hpp"

using namespace std;

int main()
{
    msg_t message;
    int client_pid = (int) getpid();

    // ftok to generate unique key
    key_t key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    int msgid = msgget(key, 0666 | IPC_CREAT);
    message.msg_type = 1;

    // Envia PID
    snprintf(message.msg_text, sizeof(message.msg_text), "%d", client_pid);

    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
    cout << "Client connected " << client_pid << endl;
    memset(&message, 0, sizeof(message));

    message.msg_type = client_pid;

    // Get game session PID
    msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), client_pid, 0);
    
    int session_id = message.session_id;
    cout << "Session ID: " << session_id << endl;
    cout << "Game session established: " << message.msg_text << endl;

    // todo: Game loop
    while(true){ // im just testing if i can correnctly send messages between clients and session threads :)
        cout << "Waiting for server message..." << endl;

        msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), 0, 0) == -1;
        memset(&message, 0, sizeof(message));

        cout << "DEBUG - Received: " << message.msg_text << endl;

        if (strstr(message.msg_text, "MOVE")) {
            cout << "\nYour turn! Enter position (1-9):" << endl;
            int position;
            cin >> position;
            cin.ignore();
            message.msg_type = session_id;
            snprintf(message.msg_text, sizeof(message.msg_text), "%d", position);
            if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
                perror("Failed to send move");
            }
        } else if (strstr(message.msg_text, "BOARD")) {
            char* board = strchr(message.msg_text, '\n');
            if (board) cout << "Board:\n" << board + 1 << endl;
        } else if (strstr(message.msg_text, "WAIT")) {
            cout << "Waiting for the other player..." << endl;
        } else if (strstr(message.msg_text, "WIN")) {
            char* winner = strchr(message.msg_text, '\n');
            cout << "\nGame over! " << (winner ? winner + 1 : "You") << " won!\n";
            break;
        } else if (strstr(message.msg_text, "DRAW")) {
            cout << "\nGame over! It's a draw!" << endl;
            break; 
        } else if (strstr(message.msg_text, "INVALID")) {
            cout << "\nInvalid move! Please try again!" << endl;
        }
    }
    cout << "Game ended. Press enter to exit ..." << endl;
    return 0;
}
