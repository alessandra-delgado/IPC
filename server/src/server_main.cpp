#include <iostream>
#include <sys/msg.h>
#include <vector>
#include <thread>

using namespace std;
#include "../include/server.hpp"

int main()
{
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Dispatcher -> the thread that takes care of routing clients to a session

    // Each game session is taken care of by it's own thread
    // So for the server, we'll have 1 + n/2 threads, where n is the number of clients~
    std::thread session_manager(dispatcher, msgid); 
    session_manager.join();
    
    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);
    cout << "\033[93mMessage queue destroyed\033[m" << endl;
    
    return 0;
}
    