#include <unordered_map>
#include <vector>
#include <stack>
#include <bit>
#include "generalData.h"
#include "repetition.h"

using namespace std;

struct simplePosition {
    ULL whitePawns = 0;
    ULL whiteKnights = 0;
    ULL whiteBishops = 0;
    ULL whiteRooks = 0;
    ULL whiteQueens = 0;
    ULL whiteKings = 0;
    ULL blackPawns = 0;
    ULL blackKnights = 0;
    ULL blackBishops = 0;
    ULL blackRooks = 0;
    ULL blackQueens = 0;
    ULL blackKings = 0;
};

vector<simplePosition> positions;
unordered_map<ULL, short> hashedPositions;
stack<ULL> hashes;

bool equals(simplePosition a, simplePosition b) {
    if (a.whitePawns == b.whitePawns && a.whiteKnights == b.whiteKnights && a.whiteBishops == b.whiteBishops && 
        a.whiteRooks == b.whiteRooks && a.whiteQueens == b.whiteQueens && a.whiteKings == b.whiteKings &&
        a.blackPawns == b.blackPawns && a.blackKnights == b.blackKnights && a.blackBishops == b.blackBishops &&
        a.blackRooks == b.blackRooks && a.blackQueens == b.blackQueens && a.blackKings == b.blackKings) return true;
    return false;
}

bool check(simplePosition board) {
    auto itr = positions.rbegin();
    int count = 0;
    while (itr != positions.rend()) {
        if (equals(board, *itr)) {
            count++;
            if (count == 3) return true;
        }
    }
    return false;
}

// returns true if there is a 3 position repetition
bool pushPosition(position state, ULL boardHash) {
    // convert state to a simple board represantation
    simplePosition board{ state.pawns & state.whitePieces, state.knights & state.whitePieces,
        state.bishops & state.whitePieces, state.rooks & state.whitePieces, state.queens & state.whitePieces, state.kings& state.whitePieces,
        state.pawns & state.blackPieces, state.knights & state.blackPieces,
        state.bishops & state.blackPieces, state.rooks & state.blackPieces, state.queens & state.blackPieces, state.kings& state.blackPieces };
    positions.push_back(board);
    hashes.push(boardHash);
    auto pair = hashedPositions.find(boardHash);
    if (pair == hashedPositions.end()) {
        hashedPositions.insert({ boardHash, 1 });
    }
    else {
        pair->second++;
        if (pair->second >= 3) {
            return check(board);
        }
    }
    return false;
}

void popPosition() {
    positions.pop_back();
    auto pair = hashedPositions.find(hashes.top());
    if (pair->second == 1) hashedPositions.erase(hashes.top());
    else pair->second--;
    hashes.pop();
}

void flush() {
    positions.clear();
    hashedPositions.clear();
    hashes = {};
}

void flushIfIrreversibleMove(position state) {
    if (positions.size() == 0) return;
    simplePosition lastPosition = *positions.rbegin();
    if (popcount(lastPosition.whitePawns ^ lastPosition.whiteKnights ^ lastPosition.whiteBishops ^
        lastPosition.whiteRooks ^ lastPosition.whiteQueens ^ lastPosition.whiteKings ^
        lastPosition.blackPawns ^ lastPosition.blackKnights ^ lastPosition.blackBishops ^
        lastPosition.blackRooks ^ lastPosition.blackQueens ^ lastPosition.blackKings) !=
        popcount(state.whitePieces ^ state.blackPieces)) flush();
    if (lastPosition.whitePawns != (state.pawns & state.whitePieces) || lastPosition.blackPawns != (state.pawns & state.blackPieces)) flush();
}