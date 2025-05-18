#pragma once
#include "../../shared/include/msg_t.hpp"

void signal_handler(int signum);
void broadcast(int msgid, msg_t *msg, long p1, long p2);
void dispatcher(int msgid);
void session_worker(int msgid, int p1, int p2, int sess_id);

