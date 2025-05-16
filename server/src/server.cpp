// C++ Program for Message Queue (Reader Process)
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <csignal>

#include "../include/server.hpp"
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
    int games = 2;
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

            // Send game session
            snprintf(message.msg_text, 100, "%d", games);

            // Envia game session id ao jogador 1
            message.msg_type = player1;
            msgsnd(msgid, &message, sizeof(message), 0);

            // Envia game session id ao jogador 2
            message.msg_type = player2;
            msgsnd(msgid, &message, sizeof(message), 0);

            cout << "A iniciar o jogo dos clientes: " << player1 << " : " << player2 << " GAME: " << games << endl;
            // todo: Criar nova thread thread e executar o jogo.

            // Increment game count
            games++;
        }
    }
}
