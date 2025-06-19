#pragma once
#include <cstdint>

using Bitboard = uint64_t;

extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];

void init_knight_attacks();
void init_king_attacks();
