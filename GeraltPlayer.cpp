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

    GeraltPlayer::GeraltPlayer() : bestLocation(Location(0, 0)) {
        theBoard = Board(kBoardSize, std::vector<char>(kBoardSize, '.'));
        curPiece = nullptr;
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
        bestPiece = nullptr;
        bestLocation = Location(-1, -1 );
        maxScore = std::numeric_limits<int>::min();

        return;
    }

    bool GeraltPlayer::isValidLocation(Location &aLocation) {
        if (aLocation.col < 0 || aLocation.col >= kBoardSize)
            return false;
        if (aLocation.row < 0 || aLocation.row >= kBoardSize)
            return false;
        return true;
    }

    int GeraltPlayer::safetyScore(const Board& aBoard, Location& aLocation) {

    }

    int GeraltPlayer::evaluateMove(const Board& aBoard, Location& aLocation) {

    }

    Board GeraltPlayer::createNewBoard(const Board& aBoard, Move& aMove) {
        Board newBoard = aBoard;
        char thePiece = aBoard[aMove.oldLocation.row][aMove.oldLocation.col];

        //kinged
        if (this->color == PieceColor::blue && aMove.newLocation.row == kBoardSize - 1)
            thePiece = 'B';
        else if (aMove.newLocation.row == 0)
            thePiece = 'G';

        newBoard[aMove.oldLocation.row][aMove.oldLocation.col] = '.';
        newBoard[aMove.newLocation.row][aMove.newLocation.col] = thePiece;

        return newBoard;
    }


    void GeraltPlayer::computeBlueMoves(const Board &aBoard, Location aLocation, bool aFirstMove) {

    }

    void GeraltPlayer::computeGoldMoves(const Board &aBoard, Location aLocation, bool aFirstMove) {

    }

    bool GeraltPlayer::takeTurn(Game& aGame, Orientation aDirection, std::ostream& aLog) {
        size_t theCount = aGame.countAvailablePieces(color);
        updateBoardStatus(aGame);

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

        //make the move with best score
        if (bestPiece != nullptr) {
            aGame.movePieceTo(*bestPiece, bestLocation);
            return true;
        }

        //no available piece, forfeit
        return false;
    }
}
