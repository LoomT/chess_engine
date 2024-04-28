#pragma once

vector<ULL> singleOut2(ULL bitboard);
ULL getRookMoves(int index, ULL blockers);
ULL getBishopMoves(int index, ULL blockers);
ULL makeAttackBitboard(position& current, bool white);
ULL makeSlidingAttackBitboard(position& current, bool white);