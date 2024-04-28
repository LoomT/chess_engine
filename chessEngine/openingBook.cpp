#include <string>
#include "openingBook.h"
using namespace std;

// short opening book
string openingBook(int moveCounter, string pos, bool amIWhite)
{
    if (moveCounter == 1 && amIWhite) // white first move
        return "e2e4";
    else if (moveCounter == 1 && !amIWhite) // black first move
    {
        if (pos == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR") // e4
            return "c7c5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR") // d4
            return "g8f6";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R") // knight f3
            return "g8f6";
        else if (pos == "rnbqkbnr/pppppppp/8/8/2P5/8/PP1PPPPP/RNBQKBNR") // c4
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/6P1/PPPPPP1P/RNBQKBNR") // g3
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/1P6/P1PPPPPP/RNBQKBNR") // b3
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/5P2/8/PPPPP1PP/RNBQKBNR") // f4
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/2N5/PPPPPPPP/R1BQKBNR") // knight c3
            return "c7c5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/1P6/8/P1PPPPPP/RNBQKBNR") // b4
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR") // e3
            return "g8f6";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR") // d3
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR") // a3
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/2P5/PP1PPPPP/RNBQKBNR") // c3
            return "g8f6";
        else if (pos == "rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR") // g4
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/7P/PPPPPPP1/RNBQKBNR") // h3
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/7P/8/PPPPPPP1/RNBQKBNR") // h4
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/7N/PPPPPPPP/RNBQKB1R") // knight h3
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/5P2/PPPPP1PP/RNBQKBNR") // f3
            return "e7e5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR") // a4
            return "d7d5";
        else if (pos == "rnbqkbnr/pppppppp/8/8/8/N7/PPPPPPPP/R1BQKBNR") // knight a3
            return "d7d5";
        else
            return "x";
    }
    else if (moveCounter == 2 && amIWhite)
    {
        if (pos == "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR") // e5
            return "g1f3";
        else if (pos == "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR") // c5
            return "g1f3";
        else if (pos == "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR") // e6
            return "d2d4";
        else if (pos == "rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR") // c6
            return "d2d4";
        else if (pos == "rnbqkbnr/ppp1pppp/3p4/8/4P3/8/PPPP1PPP/RNBQKBNR") // d6
            return "d2d4";
        else if (pos == "rnbqkbnr/pppppp1p/6p1/8/4P3/8/PPPP1PPP/RNBQKBNR") // g6
            return "d2d4";
        else if (pos == "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR") // d5
            return "e4d5";
        else if (pos == "rnbqkb1r/pppppppp/5n2/8/4P3/8/PPPP1PPP/RNBQKBNR") // knight f6
            return "e4e5";
        else if (pos == "r1bqkbnr/pppppppp/2n5/8/4P3/8/PPPP1PPP/RNBQKBNR") // knight c6
            return "g1f3";
        else if (pos == "rnbqkbnr/p1pppppp/1p6/8/4P3/8/PPPP1PPP/RNBQKBNR") // b6
            return "d2d4";
        else if (pos == "rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR") // a6
            return "d2d4";
        else if (pos == "rnbqkbnr/pppppp1p/8/6p1/4P3/8/PPPP1PPP/RNBQKBNR") // g5
            return "d2d4";
        else if (pos == "rnbqkbnr/ppppppp1/7p/8/4P3/8/PPPP1PPP/RNBQKBNR") // h6
            return "d2d4";
        else if (pos == "rnbqkb1r/pppppppp/7n/8/4P3/8/PPPP1PPP/RNBQKBNR") // knight h6
            return "d2d4";
        else if (pos == "r1bqkbnr/pppppppp/n7/8/4P3/8/PPPP1PPP/RNBQKBNR") // knight a6
            return "d2d4";
        else if (pos == "rnbqkbnr/ppppp1pp/5p2/8/4P3/8/PPPP1PPP/RNBQKBNR") // f6
            return "d2d4";
        else if (pos == "rnbqkbnr/1ppppppp/8/p7/4P3/8/PPPP1PPP/RNBQKBNR") // a5
            return "d2d4";
        else if (pos == "rnbqkbnr/ppppp1pp/8/5p2/4P3/8/PPPP1PPP/RNBQKBNR") // f5
            return "e4f5";
        else if (pos == "rnbqkbnr/ppppppp1/8/7p/4P3/8/PPPP1PPP/RNBQKBNR") // h5
            return "d2d4";
        else if (pos == "rnbqkbnr/p1pppppp/8/1p6/4P3/8/PPPP1PPP/RNBQKBNR") // b5
            return "d2d4";
        else
            return "x";
    }
    else if (moveCounter == 2 && !amIWhite)
    {
        if (pos == "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R") // e4 - knight f3
            return "d7d6";
        else if (pos == "rnbqkbnr/pp1ppppp/8/2p5/4P3/2N5/PPPP1PPP/R1BQKBNR") // e4 - knight c3
            return "b8c6";
        else if (pos == "rnbqkbnr/pp1ppppp/8/2p5/3PP3/8/PPP2PPP/RNBQKBNR") // e4 - d4
            return "c5d4";
        else if (pos == "rnbqkbnr/pp1ppppp/8/2p5/4P3/3P4/PPP2PPP/RNBQKBNR") // e4 - d3
            return "b8c6";
        else if (pos == "rnbqkbnr/pp1ppppp/8/2p5/4P3/1P6/P1PP1PPP/RNBQKBNR") // e4 - b3
            return "b8c6";
        else if (pos == "rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR") // d4 - c4
            return "e7e6";
        else if (pos == "rnbqkb1r/pppppppp/5n2/8/3P4/5N2/PPP1PPPP/RNBQKB1R") // d4 - knight f3
            return "d7d5";
        else if (pos == "rnbqkb1r/pppppppp/5n2/6B1/3P4/8/PPP1PPPP/RN1QKBNR") // d4 - bishop g5
            return "f6e4";
        else if (pos == "rnbqkb1r/pppppppp/5n2/8/3P4/2N5/PPP1PPPP/R1BQKBNR") // d4 - knight c3
            return "d7d5";
        else return "x";
    }
    else if (moveCounter == 3 && amIWhite)
    {
        if (pos == "rnbqkb1r/pppppppp/8/3nP3/8/8/PPPP1PPP/RNBQKBNR") // knight f3 - knight d5
            return "d2d4";
        else if (pos == "rnb1kbnr/ppp1pppp/8/3q4/8/8/PPPP1PPP/RNBQKBNR") // d5 - queen d5
            return "b1c3";
        else if (pos == "rn1qkbnr/pbpppppp/1p6/8/3PP3/8/PPP2PPP/RNBQKBNR") // b6 - rook b7
            return "f1d3";
        else if (pos == "rnbqkbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R") // c5 - d6
            return "d2d4";
        else if (pos == "r1bqkbnr/pp1ppppp/2n5/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R") // c5 - knight c6
            return "d2d4";
        else return "x";
    }
    else if (moveCounter == 3 && !amIWhite)
    {
        if (pos == "rnbqkb1r/ppp1pppp/5n2/3p4/2PP4/5N2/PP2PPPP/RNBQKB1R") // e4-f3-c4
            return "c7c6";
        else return "x";
    }

    return "x";
}