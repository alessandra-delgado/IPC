#pragma once

#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include "../../shared/include/msg_t.hpp"
#include "../../server/include/protocol.hpp"

class client{
    public:
        client();
        void run();

    private:
        void connectToServer();
        void display(const char* board) const;
        void sendMove(int position);
        void gameLoop();
        void messageServer(const msg_t& message);

        int msg_id;
        int client_pid;
        long session_id;

        char board[9] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
        };
};