#pragma once

#include <stdint.h>
#include "player.hpp"
#include "mark.hpp"
#include "turn.hpp"

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
        0b001010100, // Diagonal: Bottom L -> Top R
        0b100010001  // Diagonal: Top L -> Bottom R
    };
    char board_char[9] = {NONE, NONE, NONE,
                          NONE, NONE, NONE,
                          NONE, NONE, NONE};
    uint16_t board = 0b000000000;
    player p1 = {-1, 0b000000000, NONE};
    player p2 = {-1, 0b000000000, NONE};
    Turn turn = P1;
    // ^ This Turn enum seems a little useless ngl, but ill keep it for now

    bool check_for_win(uint16_t player_moves);
    bool check_for_draw();
    bool validate_move(int position);
};
