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
#include <atomic>
#include <set>
#include <mutex>
#include <semaphore.h>

#include "../include/server.hpp"
#include "../include/game_session.hpp"
#include "../../shared/include/msg_t.hpp"
#include "../../shared/include/protocol.hpp"

using namespace std;

int id_msg;
std::set<int> active_sessions;
std::mutex active_sessions_mutex;
std::atomic<bool> shutdown = false;
sem_t sem;

void signal_handler(int signum)
{
    cout << "Interrupt signal received (" << signum << ")" << endl;

    msg_t msg;
    msg.msg_type = 1;
    msg.sender_id = 1;

    msgsnd(id_msg, &msg, sizeof(msg_t) - sizeof(long), 0);
}

void broadcast(int msgid, msg_t msg, long p1, long p2)
{
    // send message to player 1
    msg.msg_type = p1;
    msgsnd(msgid, &msg, sizeof(msg_t) - sizeof(long), 0);

    // send message to player 2
    msg.msg_type = p2;
    msgsnd(msgid, &msg, sizeof(msg_t) - sizeof(long), 0);
}

void dispatcher(int msgid)
{
    id_msg = msgid;
    msg_t message;
    signal(SIGINT, signal_handler);
    vector<int> waiting_players;
    int session_id = 2;
    sem_init(&sem, 0, 0);

    cout << "IPC - SO 2024/2025 == Servidor de \033[92mTIC TAC TOE\033[0m" << endl;
    cout << "\033[96mAlessandra\033[0m - 51713 e \033[93mMatilde\033[0m - 51752" << endl;

    while (true)
    {
        cout << "A ESPERAR CONEXÃO" << endl;
        // Recebe PID do cliente ou shutdown message
        msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), 1, 0);
        if (message.sender_id == 1) // Shut down
        {
            cout << "Server was prompted to shutdown." << endl;
            while (true)
            {
                cout << "Logging out waiting players" << endl;
                for (int i = 0; i < (int)waiting_players.size(); i++)
                {
                    sprintf(message.msg_text, "%s", protocol_to_str(Protocol::MSG_SHUTDOWN));
                    message.msg_type = waiting_players[i];
                    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
                }
                waiting_players.clear();

                if (active_sessions.empty())
                    return;

                // Wait for the last thread to signal.
                cout << "Waiting for open connections to close." << endl;
                cout << "\033[91mIf sessions don't take action in 5 seconds, server will force shutdown.\033[0m" << endl;
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += 5; // timeout in seconds
                shutdown = true;
                if (sem_timedwait(&sem, &ts) == -1 && errno == ETIMEDOUT)
                {
                    std::cerr << "Timer ran out. Server forced to shutdown" << endl;
                    return;
                }
            }
        }

        // Adicioanr novo cliente à lista de espera
        waiting_players.push_back(stoi(message.msg_text));
        // Connect log message
        cout << "Cliente " << message.msg_text << " conectado." << endl;

        // Clear msg
        snprintf(message.msg_text, 6, "%s", "00000");

        // Para cada dois jogadores criar uma sessão
        while (waiting_players.size() >= 2)
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

            // ! Critical section
            active_sessions_mutex.lock();
            active_sessions.insert(session_id);
            active_sessions_mutex.unlock();
            // * -- end of critical section

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
    game.p1.mark = X;
    game.p2.pid = p2;
    game.p2.mark = O;

    msg_t message;
    message.session_id = sess_id;

    // Game loop ----------------------------------------------------------------------------
    long this_turn, to_wait; // will be the main alternating msg_type
    do
    {
        if (shutdown)
        {
            sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_SHUTDOWN));
            broadcast(msgid, message, p1, p2);
            // ! Critical section --------------------------------------------------------------
            active_sessions_mutex.lock();
            active_sessions.erase(sess_id);
            if (active_sessions.empty()) // This was the last session thread
            {
                sem_post(&sem);
            }
            active_sessions_mutex.unlock();
            // * -- end of critical section

            cout << "Session " << sess_id << " closed." << endl;
            return;
        }

        // * 0 - check for win on previous turn -- things are initialized so this should be fine
        if (game.check_for_win(game.turn == P1 ? game.p2.moves : game.p1.moves))
        {
            message.msg_type = this_turn;
            sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_WIN));
            msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

            message.msg_type = to_wait;
            sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_LOSE));
            msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

            break;
        }
        else if (game.check_for_draw())
        {
            sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_DRAW));
            broadcast(msgid, message, p1, p2);
            break;
        }

        // * 1 - Check whose turn it is
        this_turn = game.turn == P1 ? p1 : p2;
        to_wait = game.turn == P1 ? p2 : p1;

        // * 2 - Send message to both clients
        // Send to the player that waits this turn
        message.msg_type = to_wait;
        sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_WAIT));
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

        // Send to the player that plays this turn
        message.msg_type = this_turn; // Update message receiver
        sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_MOVE));
        msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

        // * 3 - Communicate with this turn's player
        bool invalid_move = true;
        sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_INVALID)); // Default
        do
        {
            // - Wait for player's move
            cout << "thread is waiting for client msg" << endl;
            msgrcv(msgid, &message, sizeof(msg_t) - sizeof(long), sess_id, 0);

            // - Parse the text from the message
            int position;
            char buf[1000];
            strncpy(buf, message.msg_text, sizeof(buf));
            char *line = strtok(buf, "\n");

            if (line == NULL) // retry
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

            // Read the next line
            line = strtok(NULL, "\n");
            // -- Convert the line to integer, if it fails tell client to retry
            try
            {
                position = stoi(message.msg_text);
                invalid_move = game.validate_move(position);
            }
            catch (const std::exception &e)
            {
               invalid_move = true;
            }

            if (invalid_move)
            {
                sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_INVALID));
                message.msg_type = this_turn;
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

                sprintf(message.msg_text, "%s\n", protocol_to_str(Protocol::MSG_MOVE));
                message.msg_type = this_turn;
                msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);
            }

        } while (invalid_move);

        // * 4 - Switch turns
        game.turn = game.turn == P1 ? P2 : P1;

        // * 5 - Send message to both clients with the game status at the end of turn
        sprintf(message.msg_text, "%s\n%s\n", protocol_to_str(Protocol::MSG_BOARD), game.board_char);

        // Broadcast game board to players
        broadcast(msgid, message, p1, p2);
    } while (true);

    // ! Critical
    active_sessions_mutex.lock();
    active_sessions.erase(sess_id);
    active_sessions_mutex.unlock();
}