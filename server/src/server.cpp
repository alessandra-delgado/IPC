// C++ Program for Message Queue (Reader Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <thread>
#include <csignal>
#include <map>

#include "../include/server.hpp"
#include "../include/game_session.hpp"
#include "../../shared/include/msg_t.hpp"
#include "../include/protocol.hpp"

using namespace std;

bool stop_dispatcher = false;
void signal_handler(int signum)
{
    cout << "Interrupt signal received (" << signum << ")" << endl;

    stop_dispatcher = true;
}

void dispatcher(int msgid)
{
    msg_t message;
    signal(SIGINT, signal_handler);
    vector<int> waiting_players;
    int session_id = 2;
    map<int, thread> game_sessions;

    // todo: send message for server shut down instead of using stop_dispatcher var, since msgrcv is blocking
    while (!stop_dispatcher)
    {
        cout << "A ESPERAR CONEXÃO" << endl;

        // Recebe PID do cliente
        msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), 1, 0);

        // Adicioanr novo cliente à lista de espera
        waiting_players.push_back(stoi(message.msg_text));

        // Connect log message
        cout << "Cliente " << message.msg_text << " conectado." << endl;

        // Clear msg
        snprintf(message.msg_text, 6, "%s", "00000");

        // Para cada dois jogadores criar uma sessão
        while (waiting_players.size() >= 2 && !stop_dispatcher)
        {
            cout << "A INICIAR JOGO" << endl;

            // Remove player 1 from waiting list
            int player1 = waiting_players.back();
            waiting_players.pop_back();
            // Remove player 2 from waiting list
            int player2 = waiting_players.back();
            waiting_players.pop_back();

            cout << "A iniciar o jogo dos clientes: " << player1 << " : " << player2 << " GAME: " << session_id << endl;
            thread t(session_worker, msgid, player1, player2, session_id);
            t.detach();
            cout << "thread created successfully" << endl;
            // Increment game count
            session_id++;
        }
    }
}

void session_worker(int msgid, int p1, int p2, int sess_id)
{
    // 1 - Init the game in this thread
    GameSession game;
    game.p1.pid = p1;
    game.p2.pid = p2;
    game.p1.mark = X;
    game.p2.mark = O;

    msg_t message;
    message.session_id = sess_id;

    sprintf(message.msg_text, "In thread %d, with session id %d, players are %d and %d", sess_id, sess_id, p1, p2);

    // send message (session id) to player 1
    message.msg_type = p1;
    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

    // send message (session id) to player 2
    message.msg_type = p2;
    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

    // Game loop ----------------------------------------------------------------------------
    long this_turn, to_wait; // will be the main alternating msg_type
    do
    {
        // 1 - Check whose turn it is
        this_turn = game.turn == P1 ? p1 : p2;
        to_wait = game.turn == P1 ? p2 : p1;

        // 2 - Send message to both clients
        // Send to the player that waits this turn
        message.msg_type = to_wait;
        sprintf(message.msg_text, protocol_to_str(Protocol::MSG_WAIT));
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

        // Send to the player that plays this turn
        message.msg_type = this_turn; // Update message receiver
        sprintf(message.msg_text, protocol_to_str(Protocol::MSG_MOVE));
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);


        // 3 - Communicate with this turn's player
        bool invalid_move = true;
        sprintf(message.msg_text, protocol_to_str(Protocol::MSG_INVALID)); // Default
        do
        {
            // - Wait for player's move
            msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), sess_id, 0);

            // - Parse the text from the message
            int position;
            char buf[1000];
            strncpy(buf, message.msg_text, sizeof(buf));
            char *line = strtok(buf, "\n");
            if (line == NULL)
            {
                // retry
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
            }
            line = strtok(NULL, "\n");
            if (line == NULL)
            {
                // retry
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
            }

            // -- Convert the line to integer, if it fails tell client to retry
            try
            {
                position = stoi(message.msg_text);
                // todo: validate move...
                // invalid_move = false;
            }
            catch (exception)
            {
                // retry
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
            };

        } while (invalid_move);
    
        // 4 - Switch turns
        game.turn = game.turn == P1 ? P2 : P1;

        // 5 - Send message to both clients with the game status at the end of turn
        // todo add the actual board to payload
        sprintf(message.msg_text, protocol_to_str(Protocol::MSG_BOARD));

        // Send to current player
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
        // Send to the waiting player
        message.msg_type = to_wait;
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

        // check for win and send message...

    } while (true);
}