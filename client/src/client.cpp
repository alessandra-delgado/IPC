// C++ Program for Message Queue (Writer Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "../../shared/include/msg_t.hpp"
#include "../include/client.hpp"

using namespace std;

client::client():client_pid(getpid()), session_id(0), msg_id(-1){
    memset(board, ' ', sizeof(board));
    connectToServer();
}

void client::connectToServer(){
    // ftok to generate unique key
    key_t key = ftok("tictactoe_connect", 1);
    // msgget creates a message queue
    // and returns identifier
    msg_id = msgget(key, 0666 | IPC_CREAT);
    msg_t message;
    message.msg_type = 1;
     // send PID
    snprintf(message.msg_text, sizeof(message.msg_text), "%d", client_pid);

    // send to server
    if (msgsnd(msg_id, &message, sizeof(msg_t) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    cout << "Client connected " << client_pid << endl;

    // get from server
    if (msgrcv(msg_id, &message, sizeof(msg_t) - sizeof(long), client_pid, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    session_id = message.session_id;
    cout << "Game session started: " << session_id << endl;
}

void client::display(const char* board) const{
    cout << "\nCurrent Board:\n";
    cout << " -----------------------\n";
    for (int i = 0; i < 9; i = i + 3){
        cout << " ";
        for(int j = 0; j < 3; j ++){
            char c = board [i + j];
            if (c == 0 || c == ' ')
                
                cout << (i+j+1);
            else
                cout << c;
            if (j < 2)
                cout << " | ";
        }
        cout << endl;
        if (i < 6)
            cout << " -----------------------n";
    }
    cout << endl;
}

void client::sendMove(int position){
    if (position < 1 || position > 9){
        cout << "Posição inválida! Introduza valor entre 1 a 9.\n";
        return;
    }

    if(board[position-1] != ' ') {
        cout << "Posição já ocupada!\n";
        return;
    }
    
    msg_t message;
    message.msg_type = session_id;
    message.sender_id = client_pid;
    snprintf(message.msg_text, sizeof(message.msg_text), "%d", position);

    if (msgsnd(msg_id, &message, sizeof(msg_t) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
}

void client::messageServer(const msg_t& message){

    cout << "DEBUG - Received: " << message.msg_text << endl;
    const char* data = strstr(message.msg_text, "BOARD:");
    if (data != nullptr){
            strncpy(board, data + 6, 9);
            display(board);
            return;
        }
         
        // check waiting message
        if (strncmp(message.msg_text, protocol_to_str(Protocol::MSG_WAIT), strlen(protocol_to_str(Protocol::MSG_WAIT))) == 0) {
            cout << "Waiting for opponent's move..." << endl;
            display(board);
            return;
        }

        // check turn
        if(strncmp(message.msg_text, protocol_to_str(Protocol::MSG_GO), strlen(protocol_to_str(Protocol::MSG_GO))) == 0){
                display(board);
                cout << "Your turn! Enter position (1-9): ";
                int position;
                cin >> position;
                cin.ignore();
                sendMove(position);
                return;
        }

        // check message from other player
        if (strncmp(message.msg_text, protocol_to_str(Protocol::MSG_BOARD), strlen(protocol_to_str(Protocol::MSG_BOARD))) == 0) {
            const char* board_str = strchr(message.msg_text, '\n');
            if (board_str != nullptr){
                display(board_str + 1);
            }
            return;
        }

        // end of game messages

        if (strncmp(message.msg_text, protocol_to_str(Protocol::MSG_WIN), strlen(protocol_to_str(Protocol::MSG_WIN))) == 0) {
            cout << "Game over! You won!" << endl;
            exit(0);
        }
        if (strncmp(message.msg_text, protocol_to_str(Protocol::MSG_DRAW), strlen(protocol_to_str(Protocol::MSG_DRAW))) == 0) {
            cout << "Game over! It's a draw!" << endl;
            exit(0);
        }
}

void client::run(){
    msg_t current_message;
    while(true){
        if (msgrcv(msg_id, &current_message, sizeof(msg_t) - sizeof(long), session_id, 0) == -1){
            perror("Erro ao receber mensagem");
            break;
        }
        messageServer(current_message);
    }
}


int main()
{
    client user;
    user.run();
    cout << "Game over. Exiting..." << endl;
    return 0;
}
