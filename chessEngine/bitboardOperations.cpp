#include <vector>
#include <bit>
#include "generalData.h"
#include "bitboardOperations.h"

#define ULL unsigned long long

using namespace std;

/*vector<ULL> singleOut(ULL& bitboard)
{
    vector<ULL> positions;
    vector<ULL> files;
    if ((bitboard & (fileAMask ^ fileBMask ^ fileCMask ^ fileDMask)) != 0)
    {
        if ((bitboard & (fileAMask ^ fileBMask)) != 0)
        {
            if ((bitboard & fileAMask) != 0)
                files.push_back(bitboard & fileAMask);
            if ((bitboard & fileBMask) != 0)
                files.push_back(bitboard & fileBMask);
        }
        if ((bitboard & (fileCMask ^ fileDMask)) != 0)
        {
            if ((bitboard & fileCMask) != 0)
                files.push_back(bitboard & fileCMask);
            if ((bitboard & fileDMask) != 0)
                files.push_back(bitboard & fileDMask);
        }
    }
    if ((bitboard & (fileEMask ^ fileFMask ^ fileGMask ^ fileHMask)) != 0)
    {
        if ((bitboard & (fileEMask ^ fileFMask)) != 0)
        {
            if ((bitboard & fileEMask) != 0)
                files.push_back(bitboard & fileEMask);
            if ((bitboard & fileFMask) != 0)
                files.push_back(bitboard & fileFMask);
        }
        if ((bitboard & (fileGMask ^ fileHMask)) != 0)
        {
            if ((bitboard & fileGMask) != 0)
                files.push_back(bitboard & fileGMask);
            if ((bitboard & fileHMask) != 0)
                files.push_back(bitboard & fileHMask);
        }
    }
    for (ULL& file : files)
    {
        if ((file & (rank1Mask ^ rank2Mask ^ rank3Mask ^ rank4Mask)) != 0)
        {
            if ((file & (rank1Mask ^ rank2Mask)) != 0)
            {
                if ((file & rank1Mask) != 0)
                    positions.push_back(file & rank1Mask);
                if ((file & rank2Mask) != 0)
                    positions.push_back(file & rank2Mask);
            }
            if ((file & (rank3Mask ^ rank4Mask)) != 0)
            {
                if ((file & rank3Mask) != 0)
                    positions.push_back(file & rank3Mask);
                if ((file & rank4Mask) != 0)
                    positions.push_back(file & rank4Mask);
            }
        }
        if ((file & (rank5Mask ^ rank6Mask ^ rank7Mask ^ rank8Mask)) != 0)
        {
            if ((file & (rank5Mask ^ rank6Mask)) != 0)
            {
                if ((file & rank5Mask) != 0)
                    positions.push_back(file & rank5Mask);
                if ((file & rank6Mask) != 0)
                    positions.push_back(file & rank6Mask);
            }
            if ((file & (rank7Mask ^ rank8Mask)) != 0)
            {
                if ((file & rank7Mask) != 0)
                    positions.push_back(file & rank7Mask);
                if ((file & rank8Mask) != 0)
                    positions.push_back(file & rank8Mask);
            }
        }
    }
    return positions;
}*/

vector<int> PawnSingleOut(ULL& bitboard)
{
    vector<int> ranks;
    if ((bitboard & (rank2Mask ^ rank3Mask ^ rank4Mask)) != 0)
    {
        if ((bitboard & (rank2Mask)) != 0)
            ranks.push_back(1);
        if ((bitboard & (rank3Mask ^ rank4Mask)) != 0)
        {
            if ((bitboard & (rank3Mask)) != 0)
                ranks.push_back(2);
            if ((bitboard & (rank4Mask)) != 0)
                ranks.push_back(3);
        }
    }
    if ((bitboard & (rank5Mask ^ rank6Mask ^ rank7Mask)) != 0)
    {
        if ((bitboard & (rank5Mask)) != 0)
            ranks.push_back(4);
        if ((bitboard & (rank6Mask ^ rank7Mask)) != 0)
        {
            if ((bitboard & (rank6Mask)) != 0)
                ranks.push_back(5);
            if ((bitboard & (rank7Mask)) != 0)
                ranks.push_back(6);
        }
    }
    return ranks;
}

vector<ULL> singleOut2(ULL bitboard)
{
    vector<ULL> positions;
    ULL temp;
    while (bitboard != 0)
    {
        temp = bit_floor(bitboard);
        positions.push_back(temp);
        bitboard ^= temp;
    }
    return positions;
}

ULL makeAttackBitboard(position& current, bool white)
{
    ULL attackTable = 0;
    ULL allPieces = current.whitePieces ^ current.blackPieces;
    if (white)
    {
        ULL whitePawns = current.pawns & current.whitePieces;
        ULL whiteBishops = current.bishops & current.whitePieces;
        ULL whiteKnights = current.knights & current.whitePieces;
        ULL whiteRooks = current.rooks & current.whitePieces;
        ULL whiteQueens = current.queens & current.whitePieces;
        ULL whiteKing = current.kings & current.whitePieces;


        attackTable |= (whitePawns << 7) & ~fileHMask;
        attackTable |= (whitePawns << 9) & ~fileAMask;

        for (ULL& k : singleOut2(whiteKnights))
        {
            attackTable |= allKnightMoves.at(k);
        }

        for (ULL& bishop : singleOut2(whiteBishops)) //bishops
        {
            int index = ULLToIndex.at(bishop);
            attackTable |= getBishopMoves(index, allPieces);
        }

        for (ULL& rook : singleOut2(whiteRooks)) //rooks
        {
            int index = ULLToIndex.at(rook);
            attackTable |= getRookMoves(index, allPieces);
        }

        for (ULL& queen : singleOut2(whiteQueens)) //queens
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            attackTable |= possibleQueenMoveBitboard;
        }

        //king
        attackTable |= allKingMoves.at(whiteKing);
    }
    else
    {
        ULL blackPawns = current.pawns & current.blackPieces;
        ULL blackBishops = current.bishops & current.blackPieces;
        ULL blackKnights = current.knights & current.blackPieces;
        ULL blackRooks = current.rooks & current.blackPieces;
        ULL blackQueens = current.queens & current.blackPieces;
        ULL blackKing = current.kings & current.blackPieces;

        attackTable |= (blackPawns >> 9) & ~fileHMask;
        attackTable |= (blackPawns >> 7) & ~fileAMask;

        for (ULL& k : singleOut2(blackKnights))
        {
            attackTable |= allKnightMoves.at(k);
        }

        for (ULL& bishop : singleOut2(blackBishops))
        {
            int index = ULLToIndex.at(bishop);
            attackTable |= getBishopMoves(index, allPieces);
        }

        for (ULL& rook : singleOut2(blackRooks))
        {
            int index = ULLToIndex.at(rook);
            attackTable |= getRookMoves(index, allPieces);
        }

        for (ULL& queen : singleOut2(blackQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            attackTable |= possibleQueenMoveBitboard;
        }

        //king
        attackTable |= allKingMoves.at(blackKing);
    }
    return attackTable;
}

ULL makeSlidingAttackBitboard(position& current, bool white)
{
    ULL attackTable = 0;
    ULL allPieces = current.whitePieces ^ current.blackPieces;
    if (white)
    {
        ULL whiteBishops = current.bishops & current.whitePieces;
        ULL whiteRooks = current.rooks & current.whitePieces;
        ULL whiteQueens = current.queens & current.whitePieces;

        for (ULL& bishop : singleOut2(whiteBishops)) //bishops
        {
            int index = ULLToIndex.at(bishop);
            attackTable |= getBishopMoves(index, allPieces);
        }

        for (ULL& rook : singleOut2(whiteRooks)) //rooks
        {
            int index = ULLToIndex.at(rook);
            attackTable |= getRookMoves(index, allPieces);
        }

        for (ULL& queen : singleOut2(whiteQueens)) //queens
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            attackTable |= possibleQueenMoveBitboard;
        }
    }
    else
    {
        ULL blackBishops = current.bishops & current.blackPieces;
        ULL blackRooks = current.rooks & current.blackPieces;
        ULL blackQueens = current.queens & current.blackPieces;

        for (ULL& bishop : singleOut2(blackBishops))
        {
            int index = ULLToIndex.at(bishop);
            attackTable |= getBishopMoves(index, allPieces);
        }

        for (ULL& rook : singleOut2(blackRooks))
        {
            int index = ULLToIndex.at(rook);
            attackTable |= getRookMoves(index, allPieces);
        }

        for (ULL& queen : singleOut2(blackQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            attackTable |= possibleQueenMoveBitboard;
        }
    }
    return attackTable;
}