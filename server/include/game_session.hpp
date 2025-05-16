#pragma once

#include <stdint.h>
#include "player.hpp"
#include "mark.hpp"

class GameSession
{
public:
    static inline int last_session = 0; // Sequential session IDs
    const uint16_t bit_masks[8] = {
        0b000000111, // Bottom row
        0b000111000, // Middle row
        0b111000000, // Top row
        0b001001001, // Right column
        0b010010010, // Middle column
        0b100100100, // Left column
        0b001010100, // Diagonal: Bottom L -> Top R
        0b100010001  // Diagonal: Top L -> Bottom R
    };
    int session_id;
    uint16_t board = 0b000000000;
    player p1 = {-1, 0b000000000, NONE};
    player p2 = {-1, 0b000000000, NONE};
    int turn;

    bool game_start();
    bool check_for_win(uint16_t player_moves);
    bool validate_move(int position);
    char *board_to_message();
};