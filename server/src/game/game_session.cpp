#include "../../include/game_session.hpp"

bool GameSession::check_for_win(uint16_t player_moves)
{
    for (int i = 0; i < 8; i++)
    {
        if (this->bit_masks[i] & player_moves == bit_masks[i])
            return true;
    }
    return false;
}

// Validates move and adds it to the current player's move
bool GameSession::validate_move(int position)
{

    player *p = this->turn == P1 ? &this->p1 : &this->p2;
    int mask = 1 << (9 - position);

    if ((position > 0 && position < 10) && (this->board & mask) == 0)
    {
        p->moves |= mask;
        this->board |= mask;
        this->board_char[position-1] = p->mark;
        return true;
    }
    return false;
}
