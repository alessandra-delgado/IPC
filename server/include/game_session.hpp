#pragma once
#include <stdint.h>

class GameSession
{
public:
    const uint16_t bit_masks[8] = {
        0b000000111, // Bottom row
        0b000111000, // Middle row
        0b111000000, // Top row
        0b001001001, // Right column
        0b010010010, // Middle column
        0b100100100, // Left column
        0b001010100, // Diagonal Bottom L -> Top R
        0b100010001  // Diagonal Top L -> Bottom R
    };
    static inline int last_session = 0; // Sequential session IDs
    int session_id;
    int pid_1 = -1;
    int pid_2 = -1;
    int turn;
    uint16_t x_moves = 0b000000000; // No moves
    uint16_t o_moves = 0b000000000; // ? wait how do i relate the moves to its player? should i create a struct? -> less repeated code this way!
    // todo: assign moves to player (perhaps by using a structure)

    bool game_start();
    bool check_for_win(uint16_t player_moves);
    bool validate_move(int position);
    char *board_to_message();
};