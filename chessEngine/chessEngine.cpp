#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <bit>
#include <algorithm>

#include "openingBook.h"
#include "generalData.h"
#include "magic.h"
#include "eval.h"
#include "bitboardOperations.h"
#include "zobrist.h"
#include "debugFunctions.h"
#include "repetition.h"

//For some reason, be šios eilutės niekas neveikia
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define ULL unsigned long long
#define colourHash 595641612625877378

using namespace std;

bool amIWhite;
bool endgame = false;

struct moveInfo
{
    ULL from = 0;
    ULL to = 0;
    int who = -1;
    bool attack = false;
    int newEnPassantFile = -1;
    bool tookPassant = false;
    int score = 0;
    bool hasScore = false;
    int dead = -1;
};

struct tempS
{
    bool endgame = 0;
    int castleRights = 0;
    int enPassantFile = 0;
    int halfClock = 0;
};

struct TT
{
    int score;
    int depth;
    int age;
};

unordered_map<ULL, TT> transpositionTable;


ULL initializeBoardHash(position& board)
{
    ULL whitePawns = board.pawns & board.whitePieces;
    ULL whiteBishops = board.bishops & board.whitePieces;
    ULL whiteKnights = board.knights & board.whitePieces;
    ULL whiteRooks = board.rooks & board.whitePieces;
    ULL whiteQueens = board.queens & board.whitePieces;
    ULL whiteKing = board.kings & board.whitePieces;
    ULL blackPawns = board.pawns & board.blackPieces;
    ULL blackBishops = board.bishops & board.blackPieces;
    ULL blackKnights = board.knights & board.blackPieces;
    ULL blackRooks = board.rooks & board.blackPieces;
    ULL blackQueens = board.queens & board.blackPieces;
    ULL blackKing = board.kings & board.blackPieces;

    ULL boardHash = 0;

    for (ULL& whitePawn : singleOut2(whitePawns))
    {
        boardHash ^= zobristTable[ULLToIndex.at(whitePawn)][PAWN];
    }
    for (ULL& whiteKnight : singleOut2(whiteKnights))
    {
        boardHash ^= zobristTable[ULLToIndex.at(whiteKnight)][KNIGHT];
    }
    for (ULL& whiteBishop : singleOut2(whiteBishops))
    {
        boardHash ^= zobristTable[ULLToIndex.at(whiteBishop)][BISHOP];
    }
    for (ULL& whiteRook : singleOut2(whiteRooks))
    {
        boardHash ^= zobristTable[ULLToIndex.at(whiteRook)][ROOK];
    }
    for (ULL& whiteQueen : singleOut2(whiteQueens))
    {
        boardHash ^= zobristTable[ULLToIndex.at(whiteQueen)][QUEEN];
    }
    boardHash ^= zobristTable[ULLToIndex.at(whiteKing)][KING];

    for (ULL& blackPawn : singleOut2(blackPawns))
    {
        boardHash ^= zobristTable[ULLToIndex.at(blackPawn)][PAWN + 6];
    }
    for (ULL& blackKnight : singleOut2(blackKnights))
    {
        boardHash ^= zobristTable[ULLToIndex.at(blackKnight)][KNIGHT + 6];
    }
    for (ULL& blackBishop : singleOut2(blackBishops))
    {
        boardHash ^= zobristTable[ULLToIndex.at(blackBishop)][BISHOP + 6];
    }
    for (ULL& blackRook : singleOut2(blackRooks))
    {
        boardHash ^= zobristTable[ULLToIndex.at(blackRook)][ROOK + 6];
    }
    for (ULL& blackQueen : singleOut2(blackQueens))
    {
        boardHash ^= zobristTable[ULLToIndex.at(blackQueen)][QUEEN + 6];
    }
    boardHash ^= zobristTable[ULLToIndex.at(blackKing)][KING + 6];

    if (!board.whiteTurn)
        boardHash ^= colourHash;

    boardHash ^= board.castleRights;

    if (board.oldEnPassantFile != -1)
        boardHash ^= zobristPassants[board.oldEnPassantFile];

    return boardHash;
}

bool promotion(moveInfo& move)
{
    if (move.who == PAWN && ((move.to & rank1Mask) != 0 || (move.to & rank8Mask) != 0))
        return true;
    else
        return false;
}

void updateBoardHash(ULL& boardHash, moveInfo& made, bool whiteMoved, int& castleRights1, int& castleRights2, int& oldEnPassant)
{
    if (made.who == CASTLEKING || made.who == CASTLEQUEEN)
    {

        if (whiteMoved)
        {
            boardHash ^= zobristTable[ULLToIndex.at(made.from)][KING] ^ zobristTable[ULLToIndex.at(made.to)][KING];
            if (made.who == CASTLEKING)
                boardHash ^= zobristTable[7][ROOK] ^ zobristTable[5][ROOK];
            else
                boardHash ^= zobristTable[3][ROOK] ^ zobristTable[0][ROOK];
        }
        else
        {
            boardHash ^= zobristTable[ULLToIndex.at(made.from)][KING + 6] ^ zobristTable[ULLToIndex.at(made.to)][KING + 6];
            if (made.who == CASTLEKING)
                boardHash ^= zobristTable[63][ROOK + 6] ^ zobristTable[61][ROOK + 6];
            else
                boardHash ^= zobristTable[56][ROOK + 6] ^ zobristTable[59][ROOK + 6];
        }

    }
    else
    {
        int pieceType = made.who;
        int diedPiece = made.dead + 6;
        if (!whiteMoved)
        {
            pieceType += 6;
            diedPiece -= 6;
        }
        if (promotion(made))
            boardHash ^= zobristTable[ULLToIndex.at(made.from)][pieceType] ^ zobristTable[ULLToIndex.at(made.to)][pieceType + 4];
        else
            boardHash ^= zobristTable[ULLToIndex.at(made.from)][pieceType] ^ zobristTable[ULLToIndex.at(made.to)][pieceType];
        if (made.newEnPassantFile != -1)
            boardHash ^= zobristPassants[made.newEnPassantFile];
        if (oldEnPassant != -1)
            boardHash ^= zobristPassants[oldEnPassant];
        if (made.attack)
        {
            if (!made.tookPassant)
                boardHash ^= zobristTable[ULLToIndex.at(made.to)][diedPiece];
            else if (whiteMoved)
                boardHash ^= zobristTable[ULLToIndex.at(made.to) - 8][diedPiece];
            else
                boardHash ^= zobristTable[ULLToIndex.at(made.to) + 8][diedPiece];
        }
    }
    boardHash ^= castleRights1 ^ castleRights2;
    boardHash ^= colourHash;
}

void nedoToBitboard(string& info, position& idk)
{
    int rank = 7;
    int file = 0;
    int spaces = 0;
    for (int i = 0; i < info.size(); i++)
    {
        if (info[i] == '/')
        {
            rank--;
            file = 0;
        }
        else if (info[i] == ' ')
        {
            spaces++;
            if (spaces == 1)
            {
                if (info[i + 1] == 'b')
                    idk.whiteTurn = false;
                else
                    idk.whiteTurn = true;
                i++;
                amIWhite = idk.whiteTurn;
            }
            else if (spaces == 4)
            {
                idk.halfClock = info[i + 1] - '0';
                i++;
                while (info[i + 1] != ' ')
                {
                    idk.halfClock = idk.halfClock * 10 + info[i + 1] - '0';
                    i++;
                }
            }
            else if (spaces == 5)
            {
                idk.moveCounter = info[i + 1] - '0';
                i++;
                while (info[i + 1] != ' ')
                {
                    idk.moveCounter = idk.moveCounter * 10 + info[i + 1] - '0';
                    i++;
                }
                while (info[i + 1] != ' ')
                {
                    idk.moveCounter = idk.moveCounter * 10 + info[i + 1] - '0';
                    i++;
                }
            }
            else if (spaces == 6)
            {
                idk.whiteTime = info[i + 1] - '0';
                i++;
                while (info[i + 1] != ' ')
                {
                    idk.whiteTime = idk.whiteTime * 10 + info[i + 1] - '0';
                    i++;
                }
            }
            else if (spaces == 7)
            {
                idk.blackTime = info[i + 1] - '0';
                i++;
                while (info[i + 1] != ' ')
                {
                    idk.blackTime = idk.blackTime * 10 + info[i + 1] - '0';
                    i++;
                    if (i + 1 == info.size())
                        break;
                }
            }
        }
        else if (spaces == 2)
        {
            if (info[i] == 'K')
                idk.castleRights += 8;
            else if (info[i] == 'Q')
                idk.castleRights += 4;
            else if (info[i] == 'k')
                idk.castleRights += 2;
            else if (info[i] == 'q')
                idk.castleRights += 1;
            else
                continue;
        }
        else if (spaces == 3)
        {
            if (info[i] != '-')
            {
                idk.oldEnPassantFile = (info[i] - 'a');
                i++;
            }
            else
                continue;
        }
        else if (info[i] < '9')
        {
            file += info[i] - '0';
        }
        else if (info[i] < 'Z')
        {
            if (info[i] == 'P')
            {
                idk.pawns += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'N')
            {
                idk.knights += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'B')
            {
                idk.bishops += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'R')
            {
                idk.rooks += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'Q')
            {
                idk.queens += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'K')
            {
                idk.kings += indexToULL.at(rank * 8 + file);
                idk.whitePieces += indexToULL.at(rank * 8 + file);
            }
            file++;
        }
        else
        {
            if (info[i] == 'p')
            {
                idk.pawns += indexToULL.at(rank * 8 + file);
                idk.blackPieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'n')
            {
                idk.knights += indexToULL.at(rank * 8 + file);
                idk.blackPieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'b')
            {
                idk.bishops += indexToULL.at(rank * 8 + file);
                idk.blackPieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'r')
            {
                idk.rooks += indexToULL.at(rank * 8 + file);
                idk.blackPieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'q')
            {
                idk.queens += indexToULL.at(rank * 8 + file);
                idk.blackPieces += indexToULL.at(rank * 8 + file);
            }
            else if (info[i] == 'k')
            {
                idk.kings ^= indexToULL.at(rank * 8 + file);
                idk.blackPieces ^= indexToULL.at(rank * 8 + file);
            }
            file++;
        }
    }

}

bool canPawnMoveForward(bool& whiteTurn, ULL& allPieces, ULL& isolatedPawn)
{
    ULL pushedPawn = 0;
    if (whiteTurn) // white turn
    {
        pushedPawn = isolatedPawn << 8;
        if ((pushedPawn & ~allPieces) != 0)
        {
            return true;
        }
        return false;
    }
    else // black turn
    {
        pushedPawn = isolatedPawn >> 8;
        if ((pushedPawn & ~allPieces) != 0)
        {
            return true;
        }
        return false;
    }
    return false;
}

bool canPawnMove2Forward(bool& whiteTurn, ULL& allPieces, ULL pushedPawn)
{
    if (whiteTurn) // white turn
    {
        pushedPawn = pushedPawn << 16;
        if ((pushedPawn & ~allPieces) != 0)
            return true;
    }
    else // black turn
    {
        pushedPawn = pushedPawn >> 16;
        if ((pushedPawn & ~allPieces) != 0)
            return true;
    }
    return false;
}

pair<bool, bool> canPawnAttack(bool& whiteTurn, ULL& enemyPieces, ULL& isolatedPawn, int& enPassantFile)
{
    ULL pushedPawn = 0;
    pair<bool, bool> attacks{ false, false };
    int index = ULLToIndex.at(isolatedPawn);
    if (whiteTurn) // white turn
    {
        pushedPawn = (isolatedPawn << 7) & ~fileHMask; // left
        if ((pushedPawn & enemyPieces) != 0)
            attacks.first = true;
        else if (enPassantFile != -1 && index / 8 == 4 && index % 8 == enPassantFile + 1)
            attacks.first = true;
        pushedPawn = (isolatedPawn << 9) & ~fileAMask; // right
        if ((pushedPawn & enemyPieces) != 0)
            attacks.second = true;
        else if (enPassantFile != -1 && index / 8 == 4 && index % 8 == enPassantFile - 1)
            attacks.second = true;
        return attacks;
    }
    else // black turn
    {
        pushedPawn = (isolatedPawn >> 7) & ~fileAMask; // right
        if ((pushedPawn & enemyPieces) != 0)
            attacks.second = true;
        else if (enPassantFile != -1 && index / 8 == 3 && index % 8 == enPassantFile - 1)
            attacks.second = true;
        pushedPawn = (isolatedPawn >> 9) & ~fileHMask; // left
        if ((pushedPawn & enemyPieces) != 0)
            attacks.first = true;
        else if (enPassantFile != -1 && index / 8 == 3 && index % 8 == enPassantFile + 1)
            attacks.first = true;
        return attacks;
    }
}

int whoDied(position& current, moveInfo& made)
{
    int theUnluckyOne;
    if ((current.knights & made.to) != 0)
    {
        theUnluckyOne = KNIGHT;
    }
    else if ((current.bishops & made.to) != 0)
    {
        theUnluckyOne = BISHOP;
    }
    else if ((current.rooks & made.to) != 0)
    {
        theUnluckyOne = ROOK;
    }
    else if ((current.queens & made.to) != 0)
    {
        theUnluckyOne = QUEEN;
    }
    else if ((current.kings & made.to) != 0)
    {
        theUnluckyOne = KING;
    }
    else
    {
        theUnluckyOne = PAWN;
    }
    return theUnluckyOne;
}

void makeMove(position& current, moveInfo& made, tempS& temp)
{
    temp.castleRights = current.castleRights;
    temp.enPassantFile = current.oldEnPassantFile;
    current.oldEnPassantFile = made.newEnPassantFile;
    temp.halfClock = current.halfClock;
    temp.endgame = endgame;

    if (made.who == PAWN || made.attack)
        current.halfClock = 0;
    else
        current.halfClock++;

    if (current.whiteTurn)
    {
        current.whitePieces ^= made.from ^ made.to;

        if (made.attack == true)
        {
            current.blackPieces ^= made.to;
            if (made.tookPassant)
            {
                current.pawns ^= made.to >> 8;
                current.blackPieces ^= made.to ^ (made.to >> 8);
                //deads.push(PAWN);
            }
            else if (made.dead == PAWN)
            {
                current.pawns ^= made.to;
                //deads.push(PAWN);
            }
            else if (made.dead == KNIGHT)
            {
                current.knights ^= made.to;
                //deads.push(KNIGHT);
            }
            else if (made.dead == BISHOP)
            {
                current.bishops ^= made.to;
                //deads.push(BISHOP);
            }
            else if (made.dead == ROOK)
            {
                if ((made.to & 9223372036854775808) != 0)
                {
                    current.castleRights &= 13;
                }
                if ((made.to & 72057594037927936) != 0)
                {
                    current.castleRights &= 14;
                }
                current.rooks ^= made.to;
                //deads.push(ROOK);
            }
            else if (made.dead == QUEEN)
            {
                current.queens ^= made.to;
                //deads.push(QUEEN);
                if (current.queens == 0)
                    endgame = true;
            }
            else if (made.dead == KING)
            {
                current.kings ^= made.to;
                //deads.push(KING);
                current.castleRights &= 12;
            }
            else
                abort();
        }

        if (made.who == PAWN)
        {
            current.pawns ^= made.from;
            if ((made.to & rank8Mask) != 0)
                current.queens ^= made.to;
            else
                current.pawns ^= made.to;
        }
        else if (made.who == KNIGHT)
        {
            current.knights ^= made.from ^ made.to;
        }
        else if (made.who == BISHOP)
        {
            current.bishops ^= made.from ^ made.to;
        }
        else if (made.who == ROOK)
        {
            if ((made.from & 128) != 0)
            {
                current.castleRights &= 7;
            }
            if ((made.from & 1) != 0)
            {
                current.castleRights &= 11;
            }
            current.rooks ^= made.from ^ made.to;
        }
        else if (made.who == QUEEN)
        {
            current.queens ^= made.from ^ made.to;
        }
        else if (made.who == KING)
        {
            current.castleRights &= 3;
            current.kings ^= made.from ^ made.to;
        }
        else if (made.who == CASTLEKING)
        {
            current.castleRights &= 3;
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 160;
            current.whitePieces ^= 160;
        }
        else if (made.who == CASTLEQUEEN)
        {
            current.castleRights &= 3;
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 9;
            current.whitePieces ^= 9;
        }
        else
            abort();
    }
    else
    {
        current.blackPieces ^= made.from ^ made.to;
        if (made.attack == true)
        {
            current.whitePieces ^= made.to;
            if (made.tookPassant)
            {
                current.pawns ^= made.to << 8;
                current.whitePieces ^= made.to ^ (made.to << 8);
                //deads.push(PAWN);
            }
            else if (made.dead == PAWN)
            {
                current.pawns ^= made.to;
                //deads.push(PAWN);
            }
            else if (made.dead == KNIGHT)
            {
                current.knights ^= made.to;
                //deads.push(KNIGHT);
            }
            else if (made.dead == BISHOP)
            {
                current.bishops ^= made.to;
                //deads.push(BISHOP);
            }
            else if (made.dead == ROOK)
            {
                if ((made.to & 128) != 0)
                {
                    current.castleRights &= 7;
                }
                if ((made.to & 1) != 0)
                {
                    current.castleRights &= 11;
                }
                current.rooks ^= made.to;
                //deads.push(ROOK);
            }
            else if (made.dead == QUEEN)
            {
                current.queens ^= made.to;
                //deads.push(QUEEN);
                if (current.queens == 0)
                    endgame = true;
            }
            else if (made.dead == KING)
            {
                current.kings ^= made.to;
                //deads.push(KING);
                current.castleRights &= 3;
            }
            else
                abort();
        }
        if (made.who == PAWN)
        {
            current.pawns ^= made.from;
            if ((made.to & rank1Mask) != 0)
                current.queens ^= made.to;
            else
                current.pawns ^= made.to;
        }
        else if (made.who == KNIGHT)
        {
            current.knights ^= made.from ^ made.to;
        }
        else if (made.who == BISHOP)
        {
            current.bishops ^= made.from ^ made.to;
        }
        else if (made.who == ROOK)
        {
            if ((made.from & 9223372036854775808) != 0)
            {
                current.castleRights &= 13;
            }
            if ((made.from & 72057594037927936) != 0)
            {
                current.castleRights &= 14;
            }
            current.rooks ^= made.from ^ made.to;
        }
        else if (made.who == QUEEN)
        {
            current.queens ^= made.from ^ made.to;
        }
        else if (made.who == KING)
        {
            current.castleRights &= 12;
            current.kings ^= made.from ^ made.to;
        }
        else if (made.who == CASTLEKING)
        {
            current.castleRights &= 12;
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 11529215046068469760;
            current.blackPieces ^= 11529215046068469760;
        }
        else if (made.who == CASTLEQUEEN)
        {
            current.castleRights &= 12;
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 648518346341351424;
            current.blackPieces ^= 648518346341351424;
        }
    }
    if (!current.whiteTurn)
    {
        current.moveCounter++;
        current.whiteTurn = true;
    }
    else
        current.whiteTurn = false;
}

void unMakeMove(position& current, moveInfo& made, tempS& temp)
{
    if (!current.whiteTurn)
    {
        current.whitePieces ^= made.from ^ made.to;
        if (made.attack == true)
        {
            //int ressurected = deads.top();
            //deads.pop();
            current.blackPieces ^= made.to;
            if (made.tookPassant)
            {
                current.blackPieces ^= made.to ^ (made.to >> 8);
                current.pawns ^= made.to >> 8;
            }
            else if (made.dead == PAWN)
            {
                current.pawns ^= made.to;
            }
            else if (made.dead == KNIGHT)
            {
                current.knights ^= made.to;
            }
            else if (made.dead == BISHOP)
            {
                current.bishops ^= made.to;
            }
            else if (made.dead == ROOK)
            {
                current.rooks ^= made.to;
            }
            else if (made.dead == QUEEN)
            {
                current.queens ^= made.to;
            }
            else if (made.dead == KING)
            {
                current.kings ^= made.to;
            }
        }
        if (made.who == PAWN)
        {
            current.pawns ^= made.from;
            if ((made.to & rank8Mask) != 0)
                current.queens ^= made.to;
            else
                current.pawns ^= made.to;
        }
        else if (made.who == KNIGHT)
        {
            current.knights ^= made.from ^ made.to;
        }
        else if (made.who == BISHOP)
        {
            current.bishops ^= made.from ^ made.to;
        }
        else if (made.who == ROOK)
        {
            current.rooks ^= made.from ^ made.to;
        }
        else if (made.who == QUEEN)
        {
            current.queens ^= made.from ^ made.to;
        }
        else if (made.who == KING)
        {
            current.kings ^= made.from ^ made.to;
        }
        else if (made.who == CASTLEKING)
        {
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 160;
            current.whitePieces ^= 160;
        }
        else if (made.who == CASTLEQUEEN)
        {
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 9;
            current.whitePieces ^= 9;
        }
    }
    else
    {
        current.blackPieces ^= made.from ^ made.to;
        if (made.attack)
        {
            //int ressurected = deads.top();
            //deads.pop();
            current.whitePieces ^= made.to;
            if (made.tookPassant)
            {
                current.whitePieces ^= made.to ^ (made.to << 8);
                current.pawns ^= (made.to << 8);
            }
            else if (made.dead == PAWN)
            {
                current.pawns ^= made.to;
            }
            else if (made.dead == KNIGHT)
            {
                current.knights ^= made.to;
            }
            else if (made.dead == BISHOP)
            {
                current.bishops ^= made.to;
            }
            else if (made.dead == ROOK)
            {
                current.rooks ^= made.to;
            }
            else if (made.dead == QUEEN)
            {
                current.queens ^= made.to;
            }
            else if (made.dead == KING)
            {
                current.kings ^= made.to;
            }
        }
        if (made.who == PAWN)
        {
            current.pawns ^= made.from;
            if ((made.to & rank1Mask) != 0)
                current.queens ^= made.to;
            else
                current.pawns ^= made.to;
        }
        else if (made.who == KNIGHT)
        {
            current.knights ^= made.from ^ made.to;
        }
        else if (made.who == BISHOP)
        {
            current.bishops ^= made.from ^ made.to;
        }
        else if (made.who == ROOK)
        {
            current.rooks ^= made.from ^ made.to;
        }
        else if (made.who == QUEEN)
        {
            current.queens ^= made.from ^ made.to;
        }
        else if (made.who == KING)
        {
            current.kings ^= made.from ^ made.to;
        }
        else if (made.who == CASTLEKING)
        {
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 11529215046068469760;
            current.blackPieces ^= 11529215046068469760;
        }
        else if (made.who == CASTLEQUEEN)
        {
            current.kings ^= made.from ^ made.to;
            current.rooks ^= 648518346341351424;
            current.blackPieces ^= 648518346341351424;
        }
    }
    if (current.whiteTurn)
    {
        current.moveCounter--;
        current.whiteTurn = false;
    }
    else
        current.whiteTurn = true;
    current.castleRights = temp.castleRights;
    current.halfClock = temp.halfClock;
    current.oldEnPassantFile = temp.enPassantFile;
    endgame = temp.endgame;
}

vector<moveInfo> generateMoves(position& current)
{
    vector<moveInfo> possibleMoves;
    ULL whitePawns = current.pawns & current.whitePieces;
    ULL whiteBishops = current.bishops & current.whitePieces;
    ULL whiteKnights = current.knights & current.whitePieces;
    ULL whiteRooks = current.rooks & current.whitePieces;
    ULL whiteQueens = current.queens & current.whitePieces;
    ULL whiteKing = current.kings & current.whitePieces;
    ULL blackPawns = current.pawns & current.blackPieces;
    ULL blackBishops = current.bishops & current.blackPieces;
    ULL blackKnights = current.knights & current.blackPieces;
    ULL blackRooks = current.rooks & current.blackPieces;
    ULL blackQueens = current.queens & current.blackPieces;
    ULL blackKing = current.kings & current.blackPieces;
    ULL allPieces = current.whitePieces ^ current.blackPieces;

    if (current.whiteTurn)
    {
        ULL enemyAttacks = makeAttackBitboard(current, false);

        if ((current.castleRights & 8) != 0 && (allPieces & 96) == 0) // white king side 
        {
            if ((enemyAttacks & 112) == 0)
            {
                possibleMoves.push_back(moveInfo{ 16, 64, CASTLEKING });
            }
        }
        if ((current.castleRights & 4) != 0 && (allPieces & 14) == 0) // white queen side
        {
            if ((enemyAttacks & 28) == 0)
            {
                possibleMoves.push_back(moveInfo{ 16, 4, CASTLEQUEEN });
            }
        }

        for (ULL& isolatedPawn : singleOut2(whitePawns)) // pawns
        {
            ULL pushedPawn;
            int index = ULLToIndex.at(isolatedPawn);

            pushedPawn = (isolatedPawn << 7) & ~fileHMask; // left
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 4 && index % 8 == current.oldEnPassantFile + 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            pushedPawn = (isolatedPawn << 9) & ~fileAMask; // right
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 4 && index % 8 == current.oldEnPassantFile - 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            if (canPawnMoveForward(current.whiteTurn, allPieces, isolatedPawn))
            {
                possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn << 8, PAWN });
                if ((isolatedPawn & rank2Mask) != 0 && canPawnMove2Forward(current.whiteTurn, allPieces, isolatedPawn))
                {
                    possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn << 16, PAWN, false, ULLToIndex.at(isolatedPawn) % 8 });
                }
            }
        }

        for (ULL& k : singleOut2(whiteKnights)) // knights
        {
            ULL km = allKnightMoves.at(k) & ~current.whitePieces;
            ULL kma = km & current.blackPieces;
            km ^= kma;
            for (ULL& kmm : singleOut2(km))
            {
                possibleMoves.push_back(moveInfo{ k, kmm, KNIGHT });
            }
            for (ULL& kmma : singleOut2(kma))
            {
                possibleMoves.push_back(moveInfo{ k, kmma, KNIGHT, true });
            }
        }

        for (ULL& bishop : singleOut2(whiteBishops)) // bishops
        {
            int index = ULLToIndex.at(bishop);
            ULL possibleBishopMoveBitboard = getBishopMoves(index, allPieces);
            possibleBishopMoveBitboard &= ~current.whitePieces;
            for (ULL& move : singleOut2(possibleBishopMoveBitboard))
            {
                if ((move & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
                else
                    possibleMoves.push_back(moveInfo{ bishop, move, BISHOP });
            }
        }

        for (ULL& rook : singleOut2(whiteRooks))
        {
            int index = ULLToIndex.at(rook);
            ULL possibleRookMoveBitboard = getRookMoves(index, allPieces);
            possibleRookMoveBitboard &= ~current.whitePieces;
            for (ULL& move : singleOut2(possibleRookMoveBitboard))
            {
                if ((move & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
                else
                    possibleMoves.push_back(moveInfo{ rook, move, ROOK });
            }
        }

        for (ULL& queen : singleOut2(whiteQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            possibleQueenMoveBitboard &= ~current.whitePieces;
            for (ULL& move : singleOut2(possibleQueenMoveBitboard))
            {
                if ((move & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
                else
                    possibleMoves.push_back(moveInfo{ queen, move, QUEEN });
            }
        }

        ULL km = allKingMoves.at(whiteKing) & ~current.whitePieces & ~enemyAttacks;
        ULL kma = km & current.blackPieces;
        km ^= kma;
        for (ULL& kmm : singleOut2(km))
        {
            possibleMoves.push_back(moveInfo{ whiteKing, kmm, KING });
        }
        for (ULL& kmma : singleOut2(kma))
        {
            possibleMoves.push_back(moveInfo{ whiteKing, kmma, KING, true });
        }
    }
    else
    {

        ULL enemyAttacks = makeAttackBitboard(current, true);

        if ((current.castleRights & 2) != 0 && (allPieces & 6917529027641081856) == 0) // black king side 
        {
            if ((enemyAttacks & 8070450532247928832) == 0)
            {
                possibleMoves.push_back(moveInfo{ 1152921504606846976, 4611686018427387904, CASTLEKING });
            }
        }
        if ((current.castleRights & 1) != 0 && (allPieces & 1008806316530991104) == 0) //black queen side
        {
            if ((enemyAttacks & 2017612633061982208) == 0)
            {
                possibleMoves.push_back(moveInfo{ 1152921504606846976, 288230376151711744, CASTLEQUEEN });
            }
        }

        for (ULL& isolatedPawn : singleOut2(blackPawns))
        {

            ULL pushedPawn;
            int index = ULLToIndex.at(isolatedPawn);

            pushedPawn = (isolatedPawn >> 7) & ~fileAMask; // right
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 3 && index % 8 == current.oldEnPassantFile - 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            pushedPawn = (isolatedPawn >> 9) & ~fileHMask; // left
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 3 && index % 8 == current.oldEnPassantFile + 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }


            if (canPawnMoveForward(current.whiteTurn, allPieces, isolatedPawn))
            {
                if ((isolatedPawn & rank7Mask) != 0 && canPawnMove2Forward(current.whiteTurn, allPieces, isolatedPawn))
                {
                    possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn >> 16, PAWN, false, ULLToIndex.at(isolatedPawn) % 8 });
                }
                possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn >> 8, PAWN });
            }
        }

        for (ULL& k : singleOut2(blackKnights))
        {
            ULL km = allKnightMoves.at(k) & ~current.blackPieces;
            ULL kma = km & current.whitePieces;
            km ^= kma;
            for (ULL& kmm : singleOut2(km))
            {
                possibleMoves.push_back(moveInfo{ k, kmm, KNIGHT });
            }
            for (ULL& kmma : singleOut2(kma))
            {
                possibleMoves.push_back(moveInfo{ k, kmma, KNIGHT, true });
            }
        }

        for (ULL& bishop : singleOut2(blackBishops))
        {
            int index = ULLToIndex.at(bishop);
            ULL possibleBishopMoveBitboard = getBishopMoves(index, allPieces);
            possibleBishopMoveBitboard &= ~current.blackPieces;
            for (ULL& move : singleOut2(possibleBishopMoveBitboard))
            {
                if ((move & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
                else
                    possibleMoves.push_back(moveInfo{ bishop, move, BISHOP });
            }
        }

        for (ULL& rook : singleOut2(blackRooks))
        {
            int index = ULLToIndex.at(rook);
            ULL possibleRookMoveBitboard = getRookMoves(index, allPieces);
            possibleRookMoveBitboard &= ~current.blackPieces;
            for (ULL& move : singleOut2(possibleRookMoveBitboard))
            {
                if ((move & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
                else
                    possibleMoves.push_back(moveInfo{ rook, move, ROOK });
            }
        }

        for (ULL& queen : singleOut2(blackQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            possibleQueenMoveBitboard &= ~current.blackPieces;
            for (ULL& move : singleOut2(possibleQueenMoveBitboard))
            {
                if ((move & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
                else
                    possibleMoves.push_back(moveInfo{ queen, move, QUEEN });
            }
        }

        ULL km = allKingMoves.at(blackKing) & ~current.blackPieces & ~enemyAttacks;
        ULL kma = km & current.whitePieces;
        km ^= kma;
        for (ULL& kmm : singleOut2(km))
        {
            possibleMoves.push_back(moveInfo{ blackKing, kmm, KING });
        }
        for (ULL& kmma : singleOut2(kma))
        {
            possibleMoves.push_back(moveInfo{ blackKing, kmma, KING, true });
        }
    }

    return possibleMoves;
}

vector<moveInfo> generateCaps(position& current)
{
    vector<moveInfo> possibleMoves;
    ULL whitePawns = current.pawns & current.whitePieces;
    ULL whiteBishops = current.bishops & current.whitePieces;
    ULL whiteKnights = current.knights & current.whitePieces;
    ULL whiteRooks = current.rooks & current.whitePieces;
    ULL whiteQueens = current.queens & current.whitePieces;
    ULL whiteKing = current.kings & current.whitePieces;
    ULL blackPawns = current.pawns & current.blackPieces;
    ULL blackBishops = current.bishops & current.blackPieces;
    ULL blackKnights = current.knights & current.blackPieces;
    ULL blackRooks = current.rooks & current.blackPieces;
    ULL blackQueens = current.queens & current.blackPieces;
    ULL blackKing = current.kings & current.blackPieces;
    ULL allPieces = current.whitePieces ^ current.blackPieces;

    if (current.whiteTurn)
    {
        ULL enemyAttacks = makeAttackBitboard(current, false);

        for (ULL& isolatedPawn : singleOut2(whitePawns)) // pawns
        {
            ULL pushedPawn;
            int index = ULLToIndex.at(isolatedPawn);

            pushedPawn = (isolatedPawn << 7) & ~fileHMask; // left
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 4 && index % 8 == current.oldEnPassantFile + 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            pushedPawn = (isolatedPawn << 9) & ~fileAMask; // right
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.blackPieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 4 && index % 8 == current.oldEnPassantFile - 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }
            // promotion
            if ((isolatedPawn & rank7Mask) != 0 && canPawnMoveForward(current.whiteTurn, allPieces, isolatedPawn))
            {
                possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn << 8, PAWN });
            }
        }

        for (ULL& k : singleOut2(whiteKnights)) // knights
        {
            ULL km = allKnightMoves.at(k) & current.blackPieces;
            for (ULL& kmma : singleOut2(km))
            {
                possibleMoves.push_back(moveInfo{ k, kmma, KNIGHT, true });
            }
        }

        for (ULL& bishop : singleOut2(whiteBishops)) // bishops
        {
            int index = ULLToIndex.at(bishop);
            ULL possibleBishopMoveBitboard = getBishopMoves(index, allPieces);
            possibleBishopMoveBitboard &= current.blackPieces;
            for (ULL& move : singleOut2(possibleBishopMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
            }
        }

        for (ULL& rook : singleOut2(whiteRooks))
        {
            int index = ULLToIndex.at(rook);
            ULL possibleRookMoveBitboard = getRookMoves(index, allPieces);
            possibleRookMoveBitboard &= current.blackPieces;
            for (ULL& move : singleOut2(possibleRookMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
            }
        }

        for (ULL& queen : singleOut2(whiteQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            possibleQueenMoveBitboard &= current.blackPieces;
            for (ULL& move : singleOut2(possibleQueenMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
            }
        }

        ULL km = allKingMoves.at(whiteKing) & current.blackPieces & ~enemyAttacks;

        for (ULL& kmma : singleOut2(km))
        {
            possibleMoves.push_back(moveInfo{ whiteKing, kmma, KING, true });
        }
    }
    else
    {
        /*
        if ((makeAttackBitboard(current, false) & whiteKing) != 0)
        {
            moveInfo oops;
            possibleMoves.push_back(oops);
            return possibleMoves;
        }*/

        ULL enemyAttacks = makeAttackBitboard(current, true);

        for (ULL& isolatedPawn : singleOut2(blackPawns))
        {

            ULL pushedPawn;
            int index = ULLToIndex.at(isolatedPawn);

            pushedPawn = (isolatedPawn >> 7) & ~fileAMask; // right
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 3 && index % 8 == current.oldEnPassantFile - 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            pushedPawn = (isolatedPawn >> 9) & ~fileHMask; // left
            if (pushedPawn != 0)
            {
                if ((pushedPawn & current.whitePieces) != 0)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true });
                else if ((pushedPawn & allPieces) == 0 && current.oldEnPassantFile != -1 && index / 8 == 3 && index % 8 == current.oldEnPassantFile + 1)
                    possibleMoves.push_back(moveInfo{ isolatedPawn, pushedPawn, PAWN, true, -1, true });
            }

            if ((isolatedPawn & rank2Mask) != 0 && canPawnMoveForward(current.whiteTurn, allPieces, isolatedPawn))
            {
                possibleMoves.push_back(moveInfo{ isolatedPawn, isolatedPawn >> 8, PAWN });
            }
        }

        for (ULL& k : singleOut2(blackKnights))
        {
            ULL km = allKnightMoves.at(k) & current.whitePieces;
            for (ULL& kmma : singleOut2(km))
            {
                possibleMoves.push_back(moveInfo{ k, kmma, KNIGHT, true });
            }
        }

        for (ULL& bishop : singleOut2(blackBishops))
        {
            int index = ULLToIndex.at(bishop);
            ULL possibleBishopMoveBitboard = getBishopMoves(index, allPieces);
            possibleBishopMoveBitboard &= current.whitePieces;
            for (ULL& move : singleOut2(possibleBishopMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
            }
        }

        for (ULL& rook : singleOut2(blackRooks))
        {
            int index = ULLToIndex.at(rook);
            ULL possibleRookMoveBitboard = getRookMoves(index, allPieces);
            possibleRookMoveBitboard &= current.whitePieces;
            for (ULL& move : singleOut2(possibleRookMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
            }
        }

        for (ULL& queen : singleOut2(blackQueens))
        {
            int index = ULLToIndex.at(queen);
            ULL possibleQueenMoveBitboard = getRookMoves(index, allPieces);
            possibleQueenMoveBitboard ^= getBishopMoves(index, allPieces);
            possibleQueenMoveBitboard &= current.whitePieces;
            for (ULL& move : singleOut2(possibleQueenMoveBitboard))
            {
                possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
            }
        }

        ULL km = allKingMoves.at(blackKing) & current.whitePieces & ~enemyAttacks;
        for (ULL& kmma : singleOut2(km))
        {
            possibleMoves.push_back(moveInfo{ blackKing, kmma, KING, true });
        }
    }

    return possibleMoves;
}

vector<moveInfo> generateRecaptures(position& current, ULL& recapture)
{
    vector<moveInfo> possibleMoves;
    ULL whitePawns = current.pawns & current.whitePieces;
    ULL whiteBishops = current.bishops & current.whitePieces;
    ULL whiteKnights = current.knights & current.whitePieces;
    ULL whiteRooks = current.rooks & current.whitePieces;
    ULL whiteQueens = current.queens & current.whitePieces;
    ULL whiteKing = current.kings & current.whitePieces;
    ULL blackPawns = current.pawns & current.blackPieces;
    ULL blackBishops = current.bishops & current.blackPieces;
    ULL blackKnights = current.knights & current.blackPieces;
    ULL blackRooks = current.rooks & current.blackPieces;
    ULL blackQueens = current.queens & current.blackPieces;
    ULL blackKing = current.kings & current.blackPieces;
    ULL allPieces = current.whitePieces ^ current.blackPieces;
    ULL move = 0;

    if (current.whiteTurn)
    {
        move = (whitePawns << 9) & ~fileAMask & recapture;
        if (move != 0)
            possibleMoves.push_back(moveInfo{ move >> 9, move, PAWN, true });

        move = (whitePawns << 7) & ~fileHMask & recapture;
        if (move != 0)
            possibleMoves.push_back(moveInfo{ move >> 7, move, PAWN, true });

        for (ULL& k : singleOut2(whiteKnights)) // knights
        {
            move = allKnightMoves.at(k) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ k, move, KNIGHT, true });
        }

        for (ULL& bishop : singleOut2(whiteBishops)) // bishops
        {
            int index = ULLToIndex.at(bishop);
            move = getBishopMoves(index, allPieces) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
        }

        for (ULL& rook : singleOut2(whiteRooks)) // rooks
        {
            int index = ULLToIndex.at(rook);
            move = getRookMoves(index, allPieces) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
        }

        for (ULL& queen : singleOut2(whiteQueens)) // queens
        {
            int index = ULLToIndex.at(queen);
            move = getRookMoves(index, allPieces);
            move ^= getBishopMoves(index, allPieces);
            move &= recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
        }

        move = allKingMoves.at(whiteKing) & recapture & ~makeAttackBitboard(current, !current.whiteTurn); //king
        if (move != 0)
            possibleMoves.push_back(moveInfo{ whiteKing, move, KING, true });
    }
    else
    {
        move = (whitePawns >> 9) & ~fileHMask & recapture;
        if (move != 0)
            possibleMoves.push_back(moveInfo{ move << 9, move, PAWN, true });

        move = (whitePawns >> 7) & ~fileAMask & recapture;
        if (move != 0)
            possibleMoves.push_back(moveInfo{ move << 7, move, PAWN, true });

        for (ULL& k : singleOut2(blackKnights))
        {
            move = allKnightMoves.at(k) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ k, move, KNIGHT, true });
        }

        for (ULL& bishop : singleOut2(blackBishops))
        {
            int index = ULLToIndex.at(bishop);
            move = getBishopMoves(index, allPieces) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ bishop, move, BISHOP, true });
        }

        for (ULL& rook : singleOut2(blackRooks))
        {
            int index = ULLToIndex.at(rook);
            move = getRookMoves(index, allPieces) & recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ rook, move, ROOK, true });
        }

        for (ULL& queen : singleOut2(blackQueens))
        {
            int index = ULLToIndex.at(queen);
            move = getRookMoves(index, allPieces);
            move ^= getBishopMoves(index, allPieces);
            move &= recapture;
            if (move != 0)
                possibleMoves.push_back(moveInfo{ queen, move, QUEEN, true });
        }

        move = allKingMoves.at(blackKing) & recapture & ~makeAttackBitboard(current, !current.whiteTurn);
        if (move != 0)
            possibleMoves.push_back(moveInfo{ blackKing, move, KING, true });
    }
    return possibleMoves;
}

bool moveOrder(moveInfo& a, moveInfo& b)
{
    if (a.hasScore && b.hasScore && a.score != b.score)
    {
        if (a.score > b.score)
            return true;
        else if (a.score < b.score)
            return false;
    }
    else if (a.hasScore && !b.hasScore)
        return true;
    else if (!a.hasScore && b.hasScore)
        return false;
    else
    {
        if ((a.who == CASTLEKING || a.who == CASTLEQUEEN))
            return true;
        else if ((b.who == CASTLEKING || b.who == CASTLEQUEEN) && (a.who != CASTLEKING && a.who != CASTLEQUEEN))
            return false;
        //else if (a.attack == true && b.attack == false)
            //return true;
        else if (a.attack == true && b.attack == false)
            return true;
        else if (a.attack && b.attack)
        {
            if (a.dead > b.dead)
                return true;
            else if (a.dead == b.dead && a.who < b.who)
                return true;
        }
    }
    return false;
}

bool orderAttacks(moveInfo& a, moveInfo& b)
{
    if (a.attack == true && b.attack == false)
        return true;
    else if (a.attack && b.attack)
    {
        if (a.dead > b.dead)
            return true;
        else if (a.dead == b.dead && a.who < b.who)
            return true;
    }
    return false;
}

bool orderKingMoves(moveInfo& a, moveInfo& b)
{
    if (a.who > b.who)
        return true;
    return false;
}

bool orderQuickScore(moveInfo& a, moveInfo& b)
{
    if (a.hasScore && b.hasScore && a.score != b.score)
    {
        if (a.score > b.score)
            return true;
        else if (a.score < b.score)
            return false;
    }
    else if (a.hasScore && !b.hasScore)
        return true;

    return false;
}

bool pawnRecapture(position& current, ULL& sq)
{
    if (!current.whiteTurn)
    {
        if (((sq << 7) & current.blackPieces & current.pawns) != 0 || ((sq << 9) & current.blackPieces & current.pawns) != 0)
            return true;
    }
    else
    {
        if (((sq >> 7) & current.whitePieces & current.pawns) != 0 || ((sq >> 9) & current.whitePieces & current.pawns) != 0)
            return true;
    }
    return false;
}

bool badCapture(position& current, moveInfo& move)
{
    if (move.who == PAWN)
        return false;
    if (pValue[move.dead] > pValue[move.who])
        return false;

    if (pawnRecapture(current, move.to) &&
        pValue[move.dead] + 200 - pValue[move.who] < 0)
        return true;

    return false;
}

bool inCheck(position& current, bool turn)
{
    if (turn)
    {
        if ((makeAttackBitboard(current, false) & current.kings & current.whitePieces) != 0)
            return true;
        else return false;
    }
    else
    {
        if ((makeAttackBitboard(current, true) & current.kings & current.blackPieces) != 0)
            return true;
        else return false;
    }
}

bool inCheckSliders(position& current, bool turn)
{
    if (turn)
    {
        if ((makeSlidingAttackBitboard(current, false) & current.kings & current.whitePieces) != 0)
            return true;
        else return false;
    }
    else
    {
        if ((makeSlidingAttackBitboard(current, true) & current.kings & current.blackPieces) != 0)
            return true;
        else return false;
    }
}

unordered_set<ULL> qPieces;

int quiesce(int depth, position& current, int alpha, int beta)
{
    int standPat;
    vector<moveInfo> possibleMoves;
    bool checked = false;
    if (!inCheck(current, current.whiteTurn))
    {
        standPat = evaluate(current, endgame);

        if (current.whiteTurn)
        {
            if (standPat >= beta)
                return standPat;
            int delta = 900;
            if ((current.pawns & current.whitePieces & rank7Mask) != 0)
                delta += 700;
            if (standPat + delta < alpha)
                return standPat;
            if (standPat > alpha)
                alpha = standPat;
        }
        else
        {
            if (standPat <= alpha)
                return standPat;
            int delta = 900;
            if ((current.pawns & current.blackPieces & rank2Mask) != 0)
                delta += 700;
            if (standPat - delta > beta)
                return standPat;
            if (standPat < beta)
                beta = standPat;
        }
        if (depth >= 1)
            possibleMoves = generateCaps(current);
        else
            possibleMoves = generateMoves(current);
    }
    else
    {
        if (current.whiteTurn)
            standPat = -999999 + (depth * 11111);
        else
            standPat = 999999 - (depth * 11111);
        possibleMoves = generateMoves(current);
        checked = true;
    }

    for (auto& move : possibleMoves)
    {
        if (move.attack)
            move.dead = whoDied(current, move);
    }
    if (!checked)
        sort(possibleMoves.begin(), possibleMoves.end(), orderAttacks);
    else
        sort(possibleMoves.begin(), possibleMoves.end(), orderKingMoves);

    if (current.whiteTurn)
    {
        int score;
        for (auto& move : possibleMoves)
        {

            if (!checked && move.attack && depth >= 3)
            {
                auto itr = qPieces.find(move.to);
                if (itr == qPieces.end() || *itr != move.to)
                    continue;
            }
            tempS temp;
            makeMove(current, move, temp);

            if (!checked) // is king under check
            {
                if (inCheckSliders(current, true))
                {
                    unMakeMove(current, move, temp);
                    continue;
                }
            }
            else if (inCheck(current, true))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && !move.attack && !inCheck(current, false) && !promotion(move))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && move.attack && !endgame && standPat + pValue[move.dead] + 200 < alpha)
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && move.attack && badCapture(current, move) && !promotion(move))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            bool first = false;
            //if(move.attack)
            first = qPieces.insert(move.to).second;

            score = quiesce(depth + 1, current, alpha, beta);
            unMakeMove(current, move, temp);
            if (first)
                qPieces.erase(move.to);
            standPat = max(score, standPat);
            alpha = max(alpha, standPat);
            if (beta <= alpha)
                break;
        }
        return standPat;
    }
    else
    {
        int score;
        for (auto& move : possibleMoves)
        {

            if (!checked && move.attack && depth >= 3)
            {
                auto itr = qPieces.find(move.to);
                if (itr == qPieces.end() || *itr != move.to)
                    continue;
            }
            tempS temp;
            makeMove(current, move, temp);

            if (!checked) // is king under check
            {
                if (inCheckSliders(current, false))
                {
                    unMakeMove(current, move, temp);
                    continue;
                }
            }
            else if (inCheck(current, false))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && !move.attack && !inCheck(current, true) && !promotion(move))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && move.attack && !endgame && standPat - pValue[move.dead] - 200 > beta)
            {
                unMakeMove(current, move, temp);
                continue;
            }

            if (!checked && move.attack && badCapture(current, move) && !promotion(move))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            bool first = false;
            //if (move.attack)
            first = qPieces.insert(move.to).second;

            score = quiesce(depth + 1, current, alpha, beta);

            unMakeMove(current, move, temp);
            if (first)
                qPieces.erase(move.to);
            standPat = min(score, standPat);
            beta = min(beta, standPat);
            if (beta <= alpha)
                break;
        }
        return standPat;
    }
}

void quickEval(position& current, vector<moveInfo>& possibleMoves, ULL& boardHash)
{
    for (auto& made : possibleMoves)
    {
        int newCastleRights = current.castleRights;
        if (current.whiteTurn)
        {
            if (made.attack == true)
            {
                if (made.dead == ROOK)
                {
                    if ((made.to & 9223372036854775808) != 0)
                    {
                        newCastleRights &= 13;
                    }
                    if ((made.to & 72057594037927936) != 0)
                    {
                        newCastleRights &= 14;
                    }
                }
            }
            else if (made.who == ROOK)
            {
                if ((made.from & 128) != 0)
                {
                    newCastleRights &= 7;
                }
                if ((made.from & 1) != 0)
                {
                    newCastleRights &= 11;
                }
            }
            else if (made.who == KING)
            {
                newCastleRights &= 3;
            }
            else if (made.who == CASTLEKING)
            {
                newCastleRights &= 3;
            }
            else if (made.who == CASTLEQUEEN)
            {
                newCastleRights &= 3;
            }
        }
        else
        {
            if (made.attack == true)
            {

                if (made.dead == ROOK)
                {
                    if ((made.to & 128) != 0)
                    {
                        newCastleRights &= 7;
                    }
                    if ((made.to & 1) != 0)
                    {
                        newCastleRights &= 11;
                    }
                }
            }
            else if (made.who == ROOK)
            {
                if ((made.from & 9223372036854775808) != 0)
                {
                    newCastleRights &= 13;
                }
                if ((made.from & 72057594037927936) != 0)
                {
                    newCastleRights &= 14;
                }
            }
            else if (made.who == KING)
            {
                newCastleRights &= 12;
            }
            else if (made.who == CASTLEKING)
            {
                newCastleRights &= 12;
            }
            else if (made.who == CASTLEQUEEN)
            {
                newCastleRights &= 12;
            }
        }
        updateBoardHash(boardHash, made, current.whiteTurn, current.castleRights, newCastleRights, current.oldEnPassantFile);
        auto itr = transpositionTable.find(boardHash);
        if (itr != transpositionTable.end())
        {
            if (current.whiteTurn)
                made.score = transpositionTable.at(boardHash).score + transpositionTable.at(boardHash).depth * 10;
            else
                made.score = - transpositionTable.at(boardHash).score - transpositionTable.at(boardHash).depth * 10;
            made.hasScore = true;
            //cout << "found quick score: " << made.score << endl;
        }
        updateBoardHash(boardHash, made, current.whiteTurn, current.castleRights, newCastleRights, current.oldEnPassantFile);
    }
}

int minimax(int depth, position& current, int alpha, int beta, ULL& boardHash)
{
    if (depth == 0)
        //return evaluate(current);
        return quiesce(0, current, alpha, beta);
    vector<moveInfo> possibleMoves;

    possibleMoves = generateMoves(current);

    for (auto& move : possibleMoves)
    {
        if (move.attack)
            move.dead = whoDied(current, move);
    }
    bool checked = false;
    if (inCheck(current, current.whiteTurn))
        checked = true;
    quickEval(current, possibleMoves, boardHash);
    sort(possibleMoves.begin(), possibleMoves.end(), moveOrder);
    if (current.whiteTurn)
    {
        int maxScore = INT_MIN;

        for (moveInfo& move : possibleMoves)
        {
            int score;
            tempS temp;
            makeMove(current, move, temp);

            if (!checked) // is king under check
            {
                if (inCheckSliders(current, true))
                {
                    unMakeMove(current, move, temp);
                    continue;
                }
            }
            else if (inCheck(current, true))
            {
                unMakeMove(current, move, temp);
                continue;
            }

            updateBoardHash(boardHash, move, !current.whiteTurn, current.castleRights, temp.castleRights, temp.enPassantFile);
            auto itr = transpositionTable.find(boardHash);
            if (itr != transpositionTable.end() && itr->second.depth >= depth)
            {
                score = itr->second.score;
                itr->second.age = current.moveCounter;
                if (score > 666666)
                    score = score - itr->second.depth * 11111 + depth * 11111;
                else if (score < -666666)
                    score = score + itr->second.depth * 11111 - depth * 11111;
            }
            else
            {
                if (pushPosition(current, boardHash)) {
                    score = 0;
                }
                else score = minimax(depth - 1, current, alpha, beta, boardHash);
                popPosition();
                itr = transpositionTable.find(boardHash);
                if (itr != transpositionTable.end())
                {
                    if (depth > itr->second.depth)
                        itr->second = { score, depth, current.moveCounter };
                }
                else
                    transpositionTable.insert({ boardHash, {score, depth, current.moveCounter} });
            }
            updateBoardHash(boardHash, move, !current.whiteTurn, current.castleRights, temp.castleRights, temp.enPassantFile);

            unMakeMove(current, move, temp);
            maxScore = max(score, maxScore);
            alpha = max(alpha, maxScore);
            if (beta <= alpha)
                break;
        }
        if (maxScore == INT_MIN && checked)
            return -999999 - (depth * 11111);
        else if (maxScore == INT_MIN)
            return 0;
        else
            return maxScore;
    }
    else
    {
        int minScore = INT_MAX;
        for (moveInfo& move : possibleMoves)
        {
            int score;
            tempS temp;
            makeMove(current, move, temp);

            if (!checked) // is king under check
            {
                if (inCheckSliders(current, false))
                {
                    unMakeMove(current, move, temp);
                    continue;
                }
            }
            else if (inCheck(current, false))
            {
                unMakeMove(current, move, temp);
                continue;
            }
            updateBoardHash(boardHash, move, !current.whiteTurn, current.castleRights, temp.castleRights, temp.enPassantFile);
            auto itr = transpositionTable.find(boardHash);
            if (itr != transpositionTable.end() && itr->second.depth >= depth)
            {
                score = itr->second.score;
                itr->second.age = current.moveCounter;
                if (score > 666666)
                    score = score - itr->second.depth * 11111 + depth * 11111;
                else if (score < -666666)
                    score = score + itr->second.depth * 11111 - depth * 11111;
            }
            else
            {
                if (pushPosition(current, boardHash)) {
                    score = 0;
                }
                else score = minimax(depth - 1, current, alpha, beta, boardHash);
                popPosition();
                itr = transpositionTable.find(boardHash);
                if (itr != transpositionTable.end())
                {
                    if (depth > itr->second.depth)
                        itr->second = { score, depth, current.moveCounter };
                }
                else
                    transpositionTable.insert({ boardHash, { score, depth, current.moveCounter } });
            }
            updateBoardHash(boardHash, move, !current.whiteTurn, current.castleRights, temp.castleRights, temp.enPassantFile);
            unMakeMove(current, move, temp);
            minScore = min(score, minScore);
            beta = min(beta, minScore);
            if (beta <= alpha)
                break;
        }
        if (minScore == INT_MAX && checked)
            return 999999 + (depth * 11111);
        else if (minScore == INT_MAX)
            return 0;
        else
            return minScore;
    }
}

moveInfo findBestMove(position& current, int depth, int& time)
{
    ULL boardHash = initializeBoardHash(current);

    // save the opponent's move
    flushIfIrreversibleMove(current);
    pushPosition(current, boardHash);

    moveInfo bestMove;
    vector<moveInfo> possibleMoves = generateMoves(current);
    for (auto& move : possibleMoves)
    {
        if (move.attack)
            move.dead = whoDied(current, move);
    }
    int score = 0;
    int bestScore = 0;
    if (amIWhite)
        bestScore = INT_MIN;
    else
        bestScore = INT_MAX;

    bool checked = false;

    if (inCheck(current, amIWhite)) // is king under check
    {
        if (time > 60000)
        {
            depth++;
        }
        checked = true;
    }
    quickEval(current, possibleMoves, boardHash);
    sort(possibleMoves.begin(), possibleMoves.end(), moveOrder);

    for (moveInfo& move : possibleMoves)
    {
        int score;
        tempS temp;
        makeMove(current, move, temp);

        if (!checked) // is king under check
        {
            if (inCheckSliders(current, amIWhite))
            {
                unMakeMove(current, move, temp);
                continue;
            }
        }
        else if (inCheck(current, amIWhite))
        {
            unMakeMove(current, move, temp);
            continue;
        }
        updateBoardHash(boardHash, move, amIWhite, current.castleRights, temp.castleRights, temp.enPassantFile);
        auto itr = transpositionTable.find(boardHash);
        if (itr != transpositionTable.end() && itr->second.depth >= depth)
        {
            score = itr->second.score;
            itr->second.age = current.moveCounter;
            if (score > 666666)
                score = score - itr->second.depth * 11111 + (depth * 11111);
            else if (score < -666666)
                score = score + itr->second.depth * 11111 - (depth * 11111);
        }
        else
        {
            if (pushPosition(current, boardHash)) {
                score = 0;
            }
            else score = minimax(depth - 1, current, INT_MIN, INT_MAX, boardHash);
            popPosition();
            itr = transpositionTable.find(boardHash);
            if (itr != transpositionTable.end())
            {
                if (depth > itr->second.depth)
                    itr->second = { score, depth, current.moveCounter };
            }
            else
                transpositionTable.insert({ boardHash, {score, depth, current.moveCounter} });
        }
        cout << depth << " " << indexToCoord[ULLToIndex.at(move.from)] + indexToCoord[ULLToIndex.at(move.to)] << " quick score: " << move.score << " real score: " << score << endl;
        if (amIWhite && score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
        else if (!amIWhite && score < bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
        updateBoardHash(boardHash, move, amIWhite, current.castleRights, temp.castleRights, temp.enPassantFile);
        unMakeMove(current, move, temp);
    }
    if (checked && time > 60000)
        depth--;

    // save your move
    tempS temp;
    makeMove(current, bestMove, temp);
    updateBoardHash(boardHash, bestMove, amIWhite, current.castleRights, temp.castleRights, temp.enPassantFile);
    flushIfIrreversibleMove(current);
    pushPosition(current, boardHash);

    return bestMove;
}

int stability = 0;

void timeControl(int& oldTime, int& newTime, int& depth)
{
    double time = double(oldTime) - double(newTime);
    time /= double(oldTime);

    if (oldTime < 60000 && time > 0.1 || oldTime - newTime > 10000)
    {
        depth--;
        stability = 0;
    }
    else if (time < 0 && stability > 3 && newTime > 45000)
    {
        depth++;
    }
    stability++;
}

void TTclear(int ageCut)
{
    auto itr = transpositionTable.begin();
    auto last = transpositionTable.end();
    while (itr != last)
    {
        if ((itr->second.age + itr->second.depth) < ageCut)
            itr = transpositionTable.erase(itr);
        else
            itr++;
    }
}

int main()
{
    char clr;
    do
    {
        cout << "w/b?\n";
        cin >> clr;
        cout << endl;
    } while (clr != 'w' && clr != 'b');

    /// WINSOCKET JUODOJI MAGIJA, JEI VEIKIA, SIULAU NEKEIST
    WSADATA wsa_data;
    SOCKADDR_IN addr;

    WSAStartup(MAKEWORD(2, 0), &wsa_data);
    const auto server = socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    addr.sin_family = AF_INET;
    if (clr == 'w')
        addr.sin_port = htons(6969); // Pakeisti į 6970, jeigu jungiates prie juodųjų
    else if (clr == 'b')
        addr.sin_port = htons(6970);

    connect(server, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
    cout << "Connected to server!" << endl;
    /// WINSOCKET JUODOJI MAGIJA, JEI VEIKIA, SIULAU NEKEIST

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int result;

    initializeMagicBitboards();
    initializeZobristTable();

    int myTimeOld = 307000;
    int myTimeNew = 0;
    int depth = 5;

    do {
        // Receive message
        result = recv(server, recvbuf, recvbuflen, 0); // Jūsų programa lauks čia, kol serveris atsiųs naują lentos informaciją.
        string message(recvbuf);
        message = message.substr(0, result);
        cout << "Message received: " << message << endl;

        // Čia kodas išrinkti geriausią ėjimą

        position current;
        nedoToBitboard(message, current);

        string bestMoveFEN = "x";

        if (current.moveCounter < 4)
        {
            string pos;
            int i = 0;
            while (message[i] != ' ')
            {
                pos += message[i];
                i++;
            }
            bestMoveFEN = openingBook(current.moveCounter, pos, amIWhite);
        }
        if (bestMoveFEN == "x")
        {
            if (amIWhite)
                myTimeNew = current.whiteTime;
            else
                myTimeNew = current.blackTime;

            timeControl(myTimeOld, myTimeNew, depth);

            moveInfo bestMove = findBestMove(current, depth, myTimeNew);

            TTclear(current.moveCounter + depth - 5);

            bestMoveFEN = indexToCoord[ULLToIndex.at(bestMove.from)] + indexToCoord[ULLToIndex.at(bestMove.to)];

            myTimeOld = myTimeNew;
        }
        // Send message (išsiųsti atrinktą ėjima serveriui) // neaiskink
        send(server, bestMoveFEN.c_str(), bestMoveFEN.length(), 0);
        cout << "Message sent: " << bestMoveFEN << " at depth " << depth << endl;
    } while (result > 0);
    return 0;
}

//
//int main2() {
//    initializeMagicBitboards();
//    initializeZobristTable();
//    string message = "8/8/8/8/6Q1/3K4/5k1p/8 w - - 6 81 292801 288150";
//    cout << message << endl;
//    position current;
//    nedoToBitboard(message, current);
//    visualizeBoard(current);
//    cout << endl;
//    vector<moveInfo> initialMoves = generateMoves(current);
//    moveInfo bestMove = findBestMove(current, 10, initialMoves);
//    string bestMoveFEN = indexToCoord[ULLToIndex.at(bestMove.from)] + indexToCoord[ULLToIndex.at(bestMove.to)];
//
//    cout << "best move: " << bestMoveFEN;
//    cout << endl;
//
//    return 0;
//}
//
//int main()
//{
//    return main2();
//}