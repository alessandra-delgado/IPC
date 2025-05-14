#pragma once

#include <stdint.h>

typedef struct{
    int pid;
    uint16_t moves : 9;
    char mark;
} player;