#pragma once

bool pushPosition(position state, ULL boardHash);
void popPosition();
void flushIfIrreversibleMove(position state);