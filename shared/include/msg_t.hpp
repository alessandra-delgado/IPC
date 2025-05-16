#pragma once

// Server and client will use this struct for the message queue
typedef struct
{
    char *msg_type;      // For client routing
    int session_id;      // Which game session this came from/is for <- might have to remove
    int sender_id;       // Who sent this
    char msg_text[1000]; // Might have to change message buffer later?
} msg_t;
// todo: ^^^