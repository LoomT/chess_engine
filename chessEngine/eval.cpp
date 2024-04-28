
#include "generalData.h"
#include "bitboardOperations.h"
#include "magic.h"
#include "eval.h"

using namespace std;

#define ULL unsigned long long

int valuePositionPawn[64]{
 0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50,
10, 10, 20, 30, 30, 20, 10, 10,
 5,  5, 10, 25, 25, 10,  5,  5,
 0,  0,  0, 20, 20,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5,
 5, 10, 10,-20,-20, 10, 10,  5,
 0,  0,  0,  0,  0,  0,  0,  0
};

int valueEndgamePositionPawn[64]{
 0,  0,  0,  0,  0,  0,  0,  0,
100,100,100,100,100,100,100,100,
50, 50, 50, 50, 50, 50, 50, 50,
30, 30, 30, 30, 30, 30, 30, 30,
20, 20, 20, 20, 20, 20, 20, 20,
10, 10, 10, 10, 10, 10, 10, 10,
 0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0
};

int valuePositionKnight[64]{
-50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50
};

int valuePositionBishop[64]{
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20
};

int valuePositionRook[64]{
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  3,  5,  5,  3,  0,  0
};

int valuePositionQueen[64]{
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
 -5,  0,  5,  5,  5,  5,  0, -5,
-10,  0,  5,  5,  5,  5,  0,-10,
-10,  0,  0,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20
};

int valuePositionKing[64]{
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20
};

int valueEndgamePositionKing[64]{
-50,-40,-30,-20,-20,-30,-40,-50,
-30,-20,-10,  0,  0,-10,-20,-30,
-30,-10, 20, 30, 30, 20,-10,-30,
-30,-10, 30, 40, 40, 30,-10,-30,
-30,-10, 30, 40, 40, 30,-10,-30,
-30,-10, 20, 30, 30, 20,-10,-30,
-30,-30,  0,  0,  0,  0,-30,-30,
-50,-30,-30,-30,-30,-30,-30,-50
};

int evaluate(position& current, bool endgame)
{
    ULL whiteKing = current.kings & current.whitePieces;
    ULL blackKing = current.kings & current.blackPieces;
    int pawnValue = pValue[PAWN];
    int knightValue = pValue[KNIGHT];
    int bishopValue = pValue[BISHOP];
    int rookValue = pValue[ROOK];
    int queenValue = pValue[QUEEN];
    int score = 0;

    if (!endgame || current.moveCounter < 40)
        for (ULL& pawn : singleOut2(current.pawns))
        {
            if ((pawn & current.whitePieces) != 0)
            {
                score += pawnValue;
                score += valuePositionPawn[63 - ULLToIndex.at(pawn)];
            }
            else
            {
                score -= pawnValue;
                score -= valuePositionPawn[ULLToIndex.at(pawn)];
            }
        }
    else
        for (ULL& pawn : singleOut2(current.pawns))
        {
            if ((pawn & current.whitePieces) != 0)
            {
                score += pawnValue;
                score += valueEndgamePositionPawn[63 - ULLToIndex.at(pawn)];
            }
            else
            {
                score -= pawnValue;
                score -= valueEndgamePositionPawn[ULLToIndex.at(pawn)];
            }
        }
    for (ULL& knight : singleOut2(current.knights))
    {
        if ((knight & current.whitePieces) != 0)
        {
            score += knightValue;
            score += valuePositionKnight[63 - ULLToIndex.at(knight)];
        }
        else
        {
            score -= knightValue;
            score -= valuePositionKnight[ULLToIndex.at(knight)];
        }
    }
    for (ULL& bishop : singleOut2(current.bishops))
    {
        if ((bishop & current.whitePieces) != 0)
        {
            score += bishopValue;
            score += valuePositionBishop[63 - ULLToIndex.at(bishop)];
        }
        else
        {
            score -= bishopValue;
            score -= valuePositionBishop[ULLToIndex.at(bishop)];
        }
    }
    vector<ULL> rooks = singleOut2(current.rooks);
    //vector<ULL> whiteRooks;
    //vector<ULL> blackRooks;
    for (ULL& rook : rooks)
    {
        if ((rook & current.whitePieces) != 0)
        {
            //whiteRooks.push_back(rook);
            score += rookValue;
            score += valuePositionRook[63 - ULLToIndex.at(rook)];
        }
        else
        {
            //blackRooks.push_back(rook);
            score -= rookValue;
            score -= valuePositionRook[ULLToIndex.at(rook)];
        }
    }
    /*if (whiteRooks.size() >= 2) {
        bool end = false;
        auto cmp = whiteRooks.begin();
        cmp++;
        while (cmp != whiteRooks.end()) {
            auto itr = whiteRooks.begin();
            ULL rays = getRookMoves(ULLToIndex.at(*cmp), current.whitePieces ^ current.blackPieces);
            while (itr != cmp) {
                if ((rays & *itr) != 0) {
                    score += 10;
                    end = true;
                    break;
                }
                itr++;
            }
            if (end) break;
            cmp++;
        }
    }
    if (blackRooks.size() >= 2) {
        bool end = false;
        auto cmp = blackRooks.begin();
        cmp++;
        while (cmp != blackRooks.end()) {
            auto itr = blackRooks.begin();
            ULL rays = getRookMoves(ULLToIndex.at(*cmp), current.whitePieces ^ current.blackPieces);
            while (itr != cmp) {
                if ((rays & *itr) != 0) {
                    score -= 10;
                    end = true;
                    break;
                }
                itr++;
            }
            if (end) break;
            cmp++;
        }
    }*/
    for (ULL& queen : singleOut2(current.queens))
    {
        if ((queen & current.whitePieces) != 0)
        {
            score += queenValue;
            score += valuePositionQueen[63 - ULLToIndex.at(queen)];
        }
        else
        {
            score -= queenValue;
            score -= valuePositionQueen[ULLToIndex.at(queen)];
        }
    }

    if (!endgame)
    {
        score += valuePositionKing[63 - ULLToIndex.at(whiteKing)];
        score -= valuePositionKing[ULLToIndex.at(blackKing)];
    }
    else
    {
        score += valueEndgamePositionKing[63 - ULLToIndex.at(whiteKing)];
        score -= valueEndgamePositionKing[ULLToIndex.at(blackKing)];
    }

    return score;
}