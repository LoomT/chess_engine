# chessEngine

My very first project made in highschool using Visual Studio and C++20.
# Instructions
Intended to be used with this chess board https://github.com/HollaFoil/KTUG-ChessBot. To run the bot, the chess board should be launched first with white_is_engine and/or black_is_engine set to True in the dist/config.txt. When the terminal says waiting for white/black to connect, the engine can be ran using VS and selecting w/b for white or black mode respectively.
# Description
The chess board app will send the complete state of the board to the engine when it's their turn in a string. The engine will then calculate the best move for some depth using a Minimax algorithm with several extensions like alpha-beta pruning, quiescence search, transposition tables and much more while trying to not run out of time. It will then send back the calculated move to the chess board and wait for the other engine or human to make a move.
