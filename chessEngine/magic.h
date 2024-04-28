#pragma once

struct magicForm
{
    ULL blockerMask = 0;
    ULL magic = 0;
    int nBits = 64;
};
inline magicForm rook[64];
inline magicForm bishop[64];

inline ULL legalRookMoves[64][4096]{};
inline ULL legalBishopMoves[64][512]{};

void initializeMagicBitboards();

ULL getRookMoves(int index, ULL blockers);

ULL getBishopMoves(int index, ULL blockers);