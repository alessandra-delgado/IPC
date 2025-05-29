#pragma once

enum Protocol
{
    // * Client to server
    MSG_MOVE, // client's move

    // * Server to client
    MSG_GO,       // this player's turn
    MSG_WAIT,     // not this player's turn
    MSG_INVALID,  // might not be needed, but its to signal the player should reenter his move
    MSG_SHUTDOWN, // to shut down gracefully

    // * Game status
    MSG_BOARD,
    MSG_WIN,
    MSG_LOSE,
    MSG_DRAW

};

const char *protocol_to_str(Protocol p)
{
    switch (p)
    {
    case MSG_MOVE:
        return "MOVE";
    case MSG_GO:
        return "GO";
    case MSG_WAIT:
        return "WAIT";
    case MSG_INVALID:
        return "INVALID";
    case MSG_BOARD:
        return "BOARD";
    case MSG_WIN:
        return "WIN";
    case MSG_LOSE:
        return "LOSE";
    case MSG_DRAW:
        return "DRAW";
    case MSG_SHUTDOWN:
        return "SHUTDOWN";
    default:
        return "ERROR";
    }
}