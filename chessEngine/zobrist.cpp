

#include "zobrist.h"
#define ULL unsigned long long

unsigned int seed = 183325984;

unsigned int randomSmall()
{
    unsigned int n = seed;
    n ^= n << 8;
    n ^= n >> 18;
    n ^= n << 13;
    n ^= n >> 4;
    seed = n;
    return n;
}

ULL randomBig()
{
    ULL part1, part2, part3, part4;
    part1 = (ULL)(randomSmall()) & 65535;
    part2 = (ULL)(randomSmall()) & 65535;
    part3 = (ULL)(randomSmall()) & 65535;
    part4 = (ULL)(randomSmall()) & 65535;
    ULL allParts = part1 | (part2 << 16) | (part3 << 32) | (part4 << 48);
    return allParts;
}

ULL rnd()
{
    return randomBig() & randomBig();
}

void initializeZobristTable()
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            zobristTable[i][j] = rnd();
        }
    }
    for (int i = 0; i < 8; i++)
    {
        zobristPassants[i] = rnd();
    }
}