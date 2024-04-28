#include <iostream>
#include <bit>

#include "generalData.h"
#include "debugFunctions.h"

#define ULL unsigned long long

using namespace std;

void visualizeBoardOld(ULL position)
{
    ULL temp;
    for (int i = 7; i >= 0; i--)
    {
        temp = position & rankMasks[i];
        for (int j = 0; j < 8; j++)
        {
            if ((temp & fileMasks[j]) != 0)
            {
                cout << 1 << " ";
            }
            else
                cout << 0 << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void visualizeBoard(position& current)
{
    ULL temp;
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

    for (int i = 7; i >= 0; i--)
    {
        temp = whitePawns & rankMasks[i];
        for (int j = 0; j < 8; j++)
        {
            bool yra = false;
            if ((whitePawns & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'P';
                yra = true;
            }

            if ((blackPawns & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'p';
                yra = true;
            }

            if ((whiteKnights & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'N';
                yra = true;
            }

            if ((blackKnights & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'n';
                yra = true;
            }

            if ((whiteBishops & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'B';
                yra = true;
            }

            if ((blackBishops & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'b';
                yra = true;
            }

            if ((whiteRooks & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'R';
                yra = true;
            }

            if ((blackRooks & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'r';
                yra = true;
            }

            if ((whiteQueens & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'Q';
                yra = true;
            }

            if ((blackQueens & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'q';
                yra = true;
            }

            if ((whiteKing & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'K';
                yra = true;
            }

            if ((blackKing & indexToULL.at(i * 8 + j)) != 0)
            {
                cout << 'k';
                yra = true;
            }

            if (yra == false)
                cout << 0;
            cout << " ";
        }
        cout << endl;
    }
}

bool ok(position& current)
{
    bool a = true;
    if ((current.whitePieces & current.blackPieces) != 0)
    {
        cout << "\nwhite and black pieces intersect\n";
        visualizeBoardOld(current.whitePieces & current.blackPieces);
        cout << endl;
        a = false;
    }
    if ((current.pawns & current.bishops) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.pawns & current.bishops);
        cout << endl;
        a = false;
    }
    if ((current.pawns & current.knights) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.pawns & current.knights);
        cout << endl;
        a = false;
    }
    if ((current.pawns & current.rooks) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.pawns & current.rooks);
        cout << endl;
        a = false;
    }
    if ((current.pawns & current.queens) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.pawns & current.queens);
        cout << endl;
        a = false;
    }
    if ((current.pawns & current.kings) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.pawns & current.kings);
        cout << endl;
        a = false;
    }
    if ((current.bishops & current.knights) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.bishops & current.knights);
        cout << endl;
        a = false;
    }
    if ((current.bishops & current.rooks) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.bishops & current.rooks);
        cout << endl;
        a = false;
    }
    if ((current.bishops & current.queens) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.bishops & current.queens);
        cout << endl;
        a = false;
    }
    if ((current.bishops & current.kings) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.bishops & current.kings);
        cout << endl;
        a = false;
    }
    if ((current.knights & current.rooks) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.knights & current.rooks);
        cout << endl;
        a = false;
    }
    if ((current.knights & current.queens) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.knights & current.queens);
        cout << endl;
        a = false;
    }
    if ((current.knights & current.kings) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.knights & current.kings);
        cout << endl;
        a = false;
    }
    if ((current.rooks & current.queens) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.rooks & current.queens);
        cout << endl;
        a = false;
    }
    if ((current.rooks & current.kings) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.rooks & current.kings);
        cout << endl;
        a = false;
    }
    if ((current.queens & current.kings) != 0)
    {
        cout << "\npieces intersect\n";
        visualizeBoardOld(current.queens & current.kings);
        cout << endl;
        a = false;
    }
    if ((current.pawns | current.bishops | current.knights | current.rooks | current.queens | current.kings) != (current.whitePieces | current.blackPieces))
    {
        cout << "\npieces and colour bitboards dont match\n";
        visualizeBoardOld((current.pawns | current.bishops | current.knights | current.rooks | current.queens & current.kings) & (current.whitePieces | current.blackPieces));
        cout << endl;
        a = false;
    }
    if (popcount(current.kings) > 2)
    {
        cout << "\ntoo many kings\n";
        visualizeBoardOld(current.kings);
        cout << endl;
        a = false;
    }
    return a;
}