// C++ Program for Message Queue (Reader Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <csignal>
#include <map>

#include "../include/server.hpp"
#include "../include/game_session.hpp"
#include "../../shared/include/msg_t.hpp"

using namespace std;

void signal_handler(int signum)
{
    cout << "Interrupt signal received (" << signum << ")" << endl;

    exit(signum);
}

void dispatcher(int msgid)
{
    msg_t message;
    signal(SIGINT, signal_handler);
    vector<int> waiting_players;
    int session_id = 2;
    map<int, thread> game_sessions;

    while (true)
    {
        cout << "A ESPERAR CONEXÃO" << endl;

        // Recebe PID do cliente
        msgrcv(msgid, &message, sizeof(message), 1, 0);

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
    game.session_id = sess_id;
    
    msg_t message;
    message.sender_id = sess_id;
    message.session_id = sess_id;

    sprintf(message.msg_text,"In thread %d, with session id %d, players are %d and %d", sess_id, sess_id, p1, p2);

    // send message to player 1
    message.msg_type = p1;
    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);

    // send message to player 2
    message.msg_type = p2;
    msgsnd(msgid, &message, sizeof(msg_t) - sizeof(long), 0);


    // todo: loop...Read client message...<
}