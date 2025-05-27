// C++ Program for Message Queue (Writer Process)
#include <sys/msg.h>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstring>
#include <sys/ipc.h>
#include <unistd.h>
#include <cstdlib>
#include "../../shared/include/msg_t.hpp"
#include "../../server/include/protocol.hpp"

using namespace std;

int main()
{
    msg_t message;
    int client_pid = (int)getpid();

    // ftok to generate unique key
    key_t key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue and returns identifier
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    message.msg_type = 1;
    message.sender_id = client_pid;
    // Envia PID
    snprintf(message.msg_text, sizeof(message.msg_text), "%d", client_pid);

    if (msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        return 1;
    }

    cout << "Client connected " << client_pid << endl;

    // Get game session PID
    if (msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), client_pid, 0) == -1)
    {
        perror("msgrcv failed");
        return 1;
    }

    int session_id = message.session_id;

    cout << "You: " << message.msg_text << endl;

    // todo: Game loop
    while (true)
    { // im just testing if i can correnctly send messages between clients and session threads :)

        string msg_text(message.msg_text);

        if (msg_text.find(protocol_to_str(Protocol::MSG_MOVE)) != string::npos)
        {
            cout << "\nYour turn! Enter position (1-9):" << endl;
            int position;
            while (!(cin >> position) || position < 1 || position > 9)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 1 and 9: ";
            }

            // message to server
            memset(&message, 0, sizeof(message));
            message.msg_type = session_id;
            message.sender_id = client_pid;
            snprintf(message.msg_text, sizeof(message.msg_text), "%d", position);

            if (msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0) == -1)
            {
                perror("msgsnd failed");
                break;
            }
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_BOARD)) != string::npos)
        {
            char *board = strchr(message.msg_text, '\n');
            if (board)
            {
                board++; // jump '\n'
                std::string display = board;
                display.erase(std::remove(display.begin(), display.end(), '\n'), display.end());
                display.resize(9, ' ');

                cout << "\nBOARD:\n";
                for (int j = 0; j < 3; j++)
                {
                    cout << " ";
                    for (int i = 0; i < 3; i++)
                    {
                        int pos = j * 3 + i;
                        char c = display[pos];
                        cout << c;
                        if (i < 2)
                            cout << " | ";
                    }
                    cout << "\n";
                    if (j < 2)
                        cout << "---+---+---\n";
                }
                cout << endl;
            }
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_WAIT)) != string::npos)
        {
            cout << "Waiting for the other player..." << endl;
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_WIN)) != string::npos)
        {
            cout << "Game over!" << "You win!" << endl;
            break;
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_LOSE)) != string::npos)
        {
            cout << "Game over!" << "You lose." << endl;
            break;
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_DRAW)) != string::npos)
        {
            cout << "Game over! It's a draw!" << endl;
            break;
        }
        else if (msg_text.find(protocol_to_str(Protocol::MSG_INVALID)) != string::npos)
        {
            cout << "Invalid move! Please try again!" << endl;
        }

        if (msg_text.find(protocol_to_str(Protocol::MSG_SHUTDOWN)) != string::npos)
        {
            cout << "Server shutdown. Thanks for playing!" << endl;

        }
        if (msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), client_pid, 0) == -1)
        {
            perror("msgrcv failed --- loop");
            return 1;
        }
    }
    cout << "Press enter to exit ..." << endl;
    cin.ignore();
    cin.get();
    return 0;
}
