#pragma once

void signal_handler(int signum);
void dispatcher(int msgid);
void session_worker(int msgid, int p1, int p2, int sess_id);

