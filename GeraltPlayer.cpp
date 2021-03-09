//
//  GeraltPlayer.hpp
//  Checkers
//
//  Created by Yunhsiu Wu on 3/7/21.
//

#include "Game.hpp"
#include "GeraltPlayer.hpp"

#include <vector>
#include <stack>
#include <limits>

namespace ECE141 {

    static const size_t kBoardSize = 8;

    GeraltPlayer::GeraltPlayer() : bestLocation(Location(-1, -1)), bestJumpLocation(Location(-1, -1)) {
        theBoard = Board(kBoardSize, std::vector<char>(kBoardSize, '.'));
        curPiece = nullptr;
        bestJumpPiece = nullptr;
        bestPiece = nullptr;
        maxScore = std::numeric_limits<int>::min();
    }

    void GeraltPlayer::updateBoardStatus(Game& aGame) {
        for (int i = 0; i < kBoardSize; ++i) {
            for (int j = 0; j < kBoardSize; ++j) {
                const Piece* thePiece = aGame.getTileAt(Location(i, j))->getPiece();
                if (thePiece == nullptr) {
                    theBoard[i][j] = '.';
                }
                else if (thePiece->getColor() == PieceColor::blue) {
                    if (thePiece->getKind() == PieceKind::king)
                        theBoard[i][j] = 'B';
                    else
                        theBoard[i][j] = 'b';
                }
                else {
                    if (thePiece->getKind() == PieceKind::king)
                        theBoard[i][j] = 'G';
                    else
                        theBoard[i][j] = 'g';
                }                
            }
        }

        curPiece = nullptr;
        bestJumpPiece = nullptr;
        bestJumpLocation = Location(-1, -1);
        bestPiece = nullptr;
        bestLocation = Location(-1, -1 );
        maxScore = std::numeric_limits<int>::min();

        return;
    }

    bool GeraltPlayer::isValidLocation(const Board& aBoard, int aRow, int aCol, char aPiece) {
        if (aCol < 0 || aCol >= kBoardSize)
            return false;
        if (aRow < 0 || aRow >= kBoardSize)
            return false;
        
        return tolower(aBoard[aRow][aCol]) == aPiece;
    }

    int GeraltPlayer::blueSafetyScore(const Board& aBoard, Location& aLocation) {
        int row = aLocation.row;
        int col = aLocation.col;
        int score = 0;

        //edge of the board, the safest
        if (col == 0 || col == kBoardSize - 1 || row == 0 || row == kBoardSize - 1)
            return 4;
        
        //diagonally adjacent to opponent, dangerous
        if (tolower(aBoard[row - 1][col - 1]) == 'g')
            score -= 2;
        if (tolower(aBoard[row - 1][col + 1]) == 'g')
            score -= 2;
        if (aBoard[row + 1][col - 1] == 'G')
            score -= 2;
        if (aBoard[row + 1][col + 1] == 'G')
            score -= 2;

        //adjacent friends
        if (tolower(aBoard[row + 1][col - 1]) == 'b') {
            if (tolower(aBoard[row - 1][col + 1]) == 'g')
                score += 3;
            else
                score += 1;
        }
        if (tolower(aBoard[row + 1][col + 1]) == 'b') {
            if (tolower(aBoard[row - 1][col - 1]) == 'g')
                score += 3;
            else
                score += 1;
        }

        if (tolower(aBoard[row - 1][col - 1]) == 'b') {
            if (aBoard[row + 1][col + 1] == 'G')
                score += 3;
            else
                score += 1;
        }
        if (tolower(aBoard[row - 1][col + 1]) == 'b') {
            if (aBoard[row + 1][col - 1] == 'G')
                score += 3;
            else
                score += 1;
        }

        return score;
    }

    int GeraltPlayer::goldSafetyScore(const Board& aBoard, Location& aLocation) {
        int row = aLocation.row;
        int col = aLocation.col;
        int score = 0;

        //edge of the board, the safest
        if (col == 0 || col == kBoardSize - 1 || row == 0 || row == kBoardSize - 1)
            return 4;

        //diagonally adjacent to opponent, dangerous
        if (tolower(aBoard[row + 1][col - 1]) == 'b')
            score -= 2;
        if (tolower(aBoard[row + 1][col + 1]) == 'b')
            score -= 2;
        if (aBoard[row - 1][col - 1] == 'B')
            score -= 2;
        if (aBoard[row - 1][col + 1] == 'B')
            score -= 2;

        //adjacent friends
        if (tolower(aBoard[row - 1][col - 1]) == 'g') {
            if (tolower(aBoard[row + 1][col + 1]) == 'b')
                score += 3;
            else
                score += 1;
        }
        if (tolower(aBoard[row - 1][col + 1]) == 'g') {
            if (tolower(aBoard[row + 1][col - 1]) == 'b')
                score += 3;
            else
                score += 1;
        }

        if (tolower(aBoard[row + 1][col - 1]) == 'g') {
            if (aBoard[row - 1][col + 1] == 'B')
                score += 3;
            else
                score += 1;
        }
        if (tolower(aBoard[row + 1][col + 1]) == 'g') {
            if (aBoard[row - 1][col - 1] == 'B')
                score += 3;
            else
                score += 1;
        }

        return score;
    }

    void GeraltPlayer::evaluateMove(const Board& aBoard, Location& aLocation, bool aJump) {
        //showBoard(aBoard);
        int score = 0;
        score += this->color == PieceColor::blue ? blueSafetyScore(aBoard, aLocation) : goldSafetyScore(aBoard, aLocation);

        //color of the opponent
        char pawn = this->color == PieceColor::blue ? 'g' : 'b';
        char king = this->color == PieceColor::blue ? 'G' : 'B';

        for (int i = 0; i < kBoardSize; ++i) {
            for (int j = 0; j < kBoardSize; ++j) {
                //pawn captured
                if (theBoard[i][j] == pawn && aBoard[i][j] != pawn)
                    score += 5;
                //king captured
                if (theBoard[i][j] == king && aBoard[i][j] != king)
                    score += 11;
            }
        }

        if (aJump) {
            if (score > maxJumpScore) {
                maxJumpScore = score;
                bestJumpPiece = curPiece;
                bestJumpLocation = aLocation;
            }
        }
        else {
            if (score > maxScore) {
                maxScore = score;
                bestPiece = curPiece;
                bestLocation = aLocation;
            }
        }

    }

    Board GeraltPlayer::createNewBoard(const Board& aBoard, Move& aMove, Location aCaptured) {
        Board newBoard = aBoard;
        char thePiece = aBoard[aMove.oldLocation.row][aMove.oldLocation.col];

        //kinged
        if (this->color == PieceColor::blue && aMove.newLocation.row == kBoardSize - 1)
            thePiece = 'B';
        else if (aMove.newLocation.row == 0)
            thePiece = 'G';

        newBoard[aMove.oldLocation.row][aMove.oldLocation.col] = '.';
        newBoard[aMove.newLocation.row][aMove.newLocation.col] = thePiece;

        if (aCaptured.row != -1)
            newBoard[aCaptured.row][aCaptured.col] = '.';

        return newBoard;
    }

    void GeraltPlayer::showBoard(const Board& aBoard) {
        std::cout << "   |---|---|---|---|---|---|---|---|\n";
        for (int i = 0; i < kBoardSize; ++i) {
            std::cout << ' ' << i << ' ';
            for (int j = 0; j < kBoardSize; ++j) {
                std::cout << "| " << aBoard[i][j] << ' ';
            }
            std::cout << "|\n" << "   |---|---|---|---|---|---|---|---|\n";
        }
        std::cout << "     0   1   2   3   4   5   6   7\n";
    }

    void GeraltPlayer::computeBlueMoves(const Board &aBoard, Location aLocation, bool aFirstMove) {
        //blue's row index goes down
        int row = aLocation.row;
        int col = aLocation.col;
        Location curLocation(row, col);
        bool isKing = aBoard[row][col] == 'B';
        bool outOfMove = true;
        
        if (aFirstMove) { //move
            if (isValidLocation(aBoard, row - 1, col - 1)) {                
                Location newLocation(row - 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
            }
            if (isValidLocation(aBoard, row - 1, col + 1)) {
                Location newLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
            }
            if (isKing) {
                if (isValidLocation(aBoard, row + 1, col - 1)) {
                    Location newLocation(row + 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                }
                if (isValidLocation(aBoard, row + 1, col + 1)) {
                    Location newLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                }
            }
        }
        
        //jump
        if (isValidLocation(aBoard, row - 1, col - 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col - 2)) {
                outOfMove = false;
                Location newLocation(row - 2, col - 2);
                Location capturedLocation(row - 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                computeBlueMoves(newBoard, newLocation);
            }
        }
        if (isValidLocation(aBoard, row - 1, col + 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row - 2, col + 2);
                Location capturedLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                computeBlueMoves(newBoard, newLocation);
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row + 1, col - 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col - 2)) {
                    outOfMove = false;
                    Location newLocation(row + 2, col - 2);
                    Location capturedLocation(row + 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    computeBlueMoves(newBoard, newLocation);
                }
            }
            if (isValidLocation(aBoard, row + 1, col + 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row + 2, col + 2);
                    Location capturedLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    computeBlueMoves(newBoard, newLocation);
                }
            }
        }

        if (!aFirstMove && outOfMove) {
            evaluateMove(aBoard, aLocation, true);
        }
        
    }

    void GeraltPlayer::computeGoldMoves(const Board &aBoard, Location aLocation, bool aFirstMove) {
        //gold's row index goes up
        int row = aLocation.row;
        int col = aLocation.col;
        Location curLocation(row, col);
        bool isKing = aBoard[row][col] == 'G';
        bool outOfMove = true;

        //move
        if (aFirstMove) {
            if (isValidLocation(aBoard, row + 1, col - 1)) {
                Location newLocation(row + 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
            }
            if (isValidLocation(aBoard, row + 1, col + 1)) {
                Location newLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
            }
            if (isKing) {
                if (isValidLocation(aBoard, row - 1, col - 1)) {
                    Location newLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                }
                if (isValidLocation(aBoard, row - 1, col + 1)) {
                    Location newLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                }
            }
        }

        //jump
        if (isValidLocation(aBoard, row + 1, col - 1, 'g')) {
            if (isValidLocation(aBoard, row + 2, col - 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col - 2);
                Location capturedLocation(row + 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                computeGoldMoves(newBoard, newLocation, true);
            }
        }
        if (isValidLocation(aBoard, row + 1, col + 1, 'g')) {
            if (isValidLocation(aBoard, row + 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col + 2);
                Location capturedLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                computeGoldMoves(newBoard, newLocation, true);
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row - 1, col - 1, 'g')) {
                if (isValidLocation(aBoard, row - 2, col - 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col - 2);
                    Location capturedLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    computeGoldMoves(newBoard, newLocation, true);
                }
            }
            if (isValidLocation(aBoard, row - 1, col + 1, 'g')) {
                if (isValidLocation(aBoard, row - 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col + 2);
                    Location capturedLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    computeGoldMoves(newBoard, newLocation, true);
                }
            }
        }

        if (!aFirstMove && outOfMove) {
            evaluateMove(aBoard, aLocation, true);
        }

    }

    bool GeraltPlayer::takeTurn(Game& aGame, Orientation aDirection, std::ostream& aLog) {
        size_t theCount = aGame.countAvailablePieces(color);
        updateBoardStatus(aGame);
        //showBoard(this->theBoard);

        for (int pos = 0; pos < theCount; pos++) {            
             if(const Piece *thePiece = aGame.getAvailablePiece(this->color, pos)) {
                 curPiece = thePiece;
                 if (this->color == PieceColor::blue)
                     computeBlueMoves(this->theBoard, thePiece->getLocation(), true);
                 else
                     computeGoldMoves(this->theBoard, thePiece->getLocation(), true);
                 curPiece = nullptr;
             }
        }

        //if a move jumped, must choose it
        if (bestJumpPiece != nullptr) {
            aGame.movePieceTo(*bestPiece, bestLocation);
            return true;
        }

        //make the move with best score
        if (bestPiece != nullptr) {
            aGame.movePieceTo(*bestPiece, bestLocation);
            return true;
        }

        //no available piece, forfeit
        return false;
    }
}
