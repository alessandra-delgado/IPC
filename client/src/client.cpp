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

    // ftok to generate unique key
    key = ftok("tictactoe_connect", 1);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

    // Envia PID
    snprintf(message.mesg_text, 6, "%d", (int) getpid());

    msgsnd(msgid, &message, sizeof(message), 0);

    while(true){
        cout << "WAITING FOR GAME" << endl;
    }

    return 0;
}