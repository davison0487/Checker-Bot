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

    GeraltPlayer::GeraltPlayer() : bestLocation(Location(-1, -1)) {
        theBoard = Board(kBoardSize, std::vector<char>(kBoardSize, '.'));
        curPiece = nullptr;
        bestJumpPiece = nullptr;
        maxJumpScore = std::numeric_limits<int>::min();
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
        tempJumpLocation.clear();
        bestJumpLocation.clear();
        bestPiece = nullptr;
        bestLocation = Location(-1, -1);
        maxJumpScore = std::numeric_limits<int>::min();
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

        //empty at all direction
        if ((aBoard[row + 1][col + 1] == '.' || tolower(aBoard[row + 1][col + 1]) == 'b') &&
            (aBoard[row + 1][col - 1] == '.' || tolower(aBoard[row + 1][col - 1]) == 'b') &&
            (aBoard[row - 1][col + 1] == '.' || tolower(aBoard[row - 1][col + 1]) == 'b') &&
            (aBoard[row - 1][col - 1] == '.' || tolower(aBoard[row - 1][col - 1]) == 'b'))
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

        //empty at all direction
        if ((aBoard[row + 1][col + 1] == '.' || tolower(aBoard[row + 1][col + 1]) == 'g') &&
            (aBoard[row + 1][col - 1] == '.' || tolower(aBoard[row + 1][col - 1]) == 'g') &&
            (aBoard[row - 1][col + 1] == '.' || tolower(aBoard[row - 1][col + 1]) == 'g') &&
            (aBoard[row - 1][col - 1] == '.' || tolower(aBoard[row - 1][col - 1]) == 'g'))
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

    int GeraltPlayer::evaluateBlueFutureMoves(const Board& aBoard, Location aLocation) {
        //blue's row index goes down
        int row = aLocation.row;
        int col = aLocation.col;
        Location curLocation(row, col);
        bool isKing = aBoard[row][col] == 'B';

        int score = 0;

        //jump
        if (isValidLocation(aBoard, row - 1, col - 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col - 2)) {
                score += 2;
                Location newLocation(row - 2, col - 2);
                Location capturedLocation(row - 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                score += evaluateBlueFutureMoves(newBoard, newLocation);
            }
        }
        if (isValidLocation(aBoard, row - 1, col + 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col + 2)) {
                score += 2;
                Location newLocation(row - 2, col + 2);
                Location capturedLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                score += evaluateBlueFutureMoves(newBoard, newLocation);
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row + 1, col - 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col - 2)) {
                    score += 2;
                    Location newLocation(row + 2, col - 2);
                    Location capturedLocation(row + 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    score += evaluateBlueFutureMoves(newBoard, newLocation);
                }
            }
            if (isValidLocation(aBoard, row + 1, col + 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col + 2)) {
                    score += 2;
                    Location newLocation(row + 2, col + 2);
                    Location capturedLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    score += evaluateBlueFutureMoves(newBoard, newLocation);
                }
            }
        }

        return score;
    }

    int GeraltPlayer::evaluateGoldFutureMoves(const Board& aBoard, Location aLocation) {
        //gold's row index goes up
        int row = aLocation.row;
        int col = aLocation.col;
        Location curLocation(row, col);
        bool isKing = aBoard[row][col] == 'G';

        int score = 0;

        //jump
        if (isValidLocation(aBoard, row + 1, col - 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col - 2)) {
                score += 2;
                Location newLocation(row + 2, col - 2);
                Location capturedLocation(row + 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                score += evaluateGoldFutureMoves(newBoard, newLocation);
            }
        }
        if (isValidLocation(aBoard, row + 1, col + 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col + 2)) {
                score += 2;
                Location newLocation(row + 2, col + 2);
                Location capturedLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                score += evaluateGoldFutureMoves(newBoard, newLocation);
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row - 1, col - 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col - 2)) {
                    score += 2;
                    Location newLocation(row - 2, col - 2);
                    Location capturedLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    score += evaluateGoldFutureMoves(newBoard, newLocation);
                }
            }
            if (isValidLocation(aBoard, row - 1, col + 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col + 2)) {
                    score += 2;
                    Location newLocation(row - 2, col + 2);
                    Location capturedLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    score += evaluateGoldFutureMoves(newBoard, newLocation);
                }
            }
        }

        return score;
    }

    void GeraltPlayer::evaluateMove(const Board& aBoard, Location& aLocation, bool aJump) {
        //showBoard(aBoard);
        int score = 0;
        score += this->color == PieceColor::blue ? blueSafetyScore(aBoard, aLocation) : goldSafetyScore(aBoard, aLocation);

        char oppPawn = this->color == PieceColor::blue ? 'g' : 'b';
        char oppKing = this->color == PieceColor::blue ? 'G' : 'B';
        char ownKing = this->color == PieceColor::blue ? 'B' : 'G';

        for (int i = 0; i < kBoardSize; ++i) {
            for (int j = 0; j < kBoardSize; ++j) {
                //pawn captured
                if (theBoard[i][j] == oppPawn && aBoard[i][j] != oppPawn)
                    score += 5;
                //king captured
                if (theBoard[i][j] == oppKing && aBoard[i][j] != oppKing)
                    score += 11;
            }
        }

        if (score >= 0)
            score += this->color == PieceColor::blue ? evaluateBlueFutureMoves(aBoard, aLocation) : evaluateGoldFutureMoves(aBoard, aLocation);

        //a king is born
        if (curPiece->getKind() == PieceKind::pawn &&
            aBoard[aLocation.row][aLocation.col] == ownKing)
            score += 15;

        srand(static_cast<uint32_t>(time(0)));

        if (aJump) {
            if (score == maxJumpScore && rand() % 2) {
                maxJumpScore = score;
                bestJumpPiece = curPiece;
                bestJumpLocation = tempJumpLocation;
            }
            if (score > maxJumpScore) {
                maxJumpScore = score;
                bestJumpPiece = curPiece;
                bestJumpLocation = tempJumpLocation;
            }
        }
        else {
            if (score == maxScore && rand() % 2) {
                maxScore = score;
                bestPiece = curPiece;
                bestLocation = aLocation;
            }
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
        if (this->color == PieceColor::blue && aMove.newLocation.row == 0)
            thePiece = 'B';
        else if (aMove.newLocation.row == kBoardSize - 1)
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

    void GeraltPlayer::computeBlueMoves(const Board& aBoard, Location aLocation, bool aFirstMove) {
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
                tempJumpLocation.push_back(newLocation);
                computeBlueMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isValidLocation(aBoard, row - 1, col + 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row - 2, col + 2);
                Location capturedLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeBlueMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
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
                    tempJumpLocation.push_back(newLocation);
                    computeBlueMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
            if (isValidLocation(aBoard, row + 1, col + 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row + 2, col + 2);
                    Location capturedLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeBlueMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
        }

        if (!aFirstMove && outOfMove) {
            evaluateMove(aBoard, aLocation, true);
        }

    }

    void GeraltPlayer::computeGoldMoves(const Board& aBoard, Location aLocation, bool aFirstMove) {
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
        if (isValidLocation(aBoard, row + 1, col - 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col - 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col - 2);
                Location capturedLocation(row + 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeGoldMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isValidLocation(aBoard, row + 1, col + 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col + 2);
                Location capturedLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeGoldMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row - 1, col - 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col - 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col - 2);
                    Location capturedLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeGoldMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
            if (isValidLocation(aBoard, row - 1, col + 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col + 2);
                    Location capturedLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeGoldMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
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
            if (const Piece* thePiece = aGame.getAvailablePiece(this->color, pos)) {
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
            for (auto location : bestJumpLocation)
                aGame.movePieceTo(*bestJumpPiece, location);
            return true;
        }

        //make the move with best score
        if (bestPiece != nullptr) {
            aGame.movePieceTo(*bestPiece, bestLocation);
        }

        //no available piece, forfeit
        return true;
    }

    void GeraltPlayer2::evaluateMove(const Board& aBoard, Location& aLocation, bool aJump) {
        //showBoard(aBoard);
        int score = 0;
        //score += this->color == PieceColor::blue ? blueSafetyScore(aBoard, aLocation) : goldSafetyScore(aBoard, aLocation);

        char oppPawn = this->color == PieceColor::blue ? 'g' : 'b';
        char oppKing = this->color == PieceColor::blue ? 'G' : 'B';
        char ownKing = this->color == PieceColor::blue ? 'B' : 'G';

        for (int i = 0; i < kBoardSize; ++i) {
            for (int j = 0; j < kBoardSize; ++j) {
                //pawn captured
                if (theBoard[i][j] == oppPawn && aBoard[i][j] != oppPawn)
                    score += 5;
                //king captured
                if (theBoard[i][j] == oppKing && aBoard[i][j] != oppKing)
                    score += 11;
            }
        }

        //a king is born
        if (curPiece->getKind() == PieceKind::pawn &&
            aBoard[aLocation.row][aLocation.col] == ownKing)
            score += 15;

        srand(static_cast<uint32_t>(time(0)));

        if (aJump) {
            if (score == maxJumpScore && rand() % 2) {
                maxJumpScore = score;
                bestJumpPiece = curPiece;
                bestJumpLocation = tempJumpLocation;
            }
            if (score > maxJumpScore) {
                maxJumpScore = score;
                bestJumpPiece = curPiece;
                bestJumpLocation = tempJumpLocation;
            }
        }
        else {
            if (score == maxScore && rand() % 2) {
                maxScore = score;
                bestPiece = curPiece;
                bestLocation = aLocation;
            }
            if (score > maxScore) {
                maxScore = score;
                bestPiece = curPiece;
                bestLocation = aLocation;
            }
        }

    }

    void GeraltPlayer2::computeBlueMoves(const Board& aBoard, Location aLocation, bool aFirstMove) {
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
                theRandomMoves.insert({ curPiece, newLocation });
            }
            if (isValidLocation(aBoard, row - 1, col + 1)) {
                Location newLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
                theRandomMoves.insert({ curPiece, newLocation });
            }
            if (isKing) {
                if (isValidLocation(aBoard, row + 1, col - 1)) {
                    Location newLocation(row + 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                    theRandomMoves.insert({ curPiece, newLocation });
                }
                if (isValidLocation(aBoard, row + 1, col + 1)) {
                    Location newLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                    theRandomMoves.insert({ curPiece, newLocation });
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
                tempJumpLocation.push_back(newLocation);
                computeBlueMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isValidLocation(aBoard, row - 1, col + 1, 'g')) {
            if (isValidLocation(aBoard, row - 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row - 2, col + 2);
                Location capturedLocation(row - 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeBlueMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
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
                    tempJumpLocation.push_back(newLocation);
                    computeBlueMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
            if (isValidLocation(aBoard, row + 1, col + 1, 'g')) {
                if (isValidLocation(aBoard, row + 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row + 2, col + 2);
                    Location capturedLocation(row + 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeBlueMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
        }

        if (!aFirstMove && outOfMove) {
            evaluateMove(aBoard, aLocation, true);
        }

    }

    void GeraltPlayer2::computeGoldMoves(const Board& aBoard, Location aLocation, bool aFirstMove) {
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
                theRandomMoves.insert({ curPiece, newLocation });
            }
            if (isValidLocation(aBoard, row + 1, col + 1)) {
                Location newLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove);
                evaluateMove(newBoard, newLocation);
                theRandomMoves.insert({ curPiece, newLocation });
            }
            if (isKing) {
                if (isValidLocation(aBoard, row - 1, col - 1)) {
                    Location newLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                    theRandomMoves.insert({ curPiece, newLocation });
                }
                if (isValidLocation(aBoard, row - 1, col + 1)) {
                    Location newLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove);
                    evaluateMove(newBoard, newLocation);
                    theRandomMoves.insert({ curPiece, newLocation });
                }
            }
        }

        //jump
        if (isValidLocation(aBoard, row + 1, col - 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col - 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col - 2);
                Location capturedLocation(row + 1, col - 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeGoldMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isValidLocation(aBoard, row + 1, col + 1, 'b')) {
            if (isValidLocation(aBoard, row + 2, col + 2)) {
                outOfMove = false;
                Location newLocation(row + 2, col + 2);
                Location capturedLocation(row + 1, col + 1);
                Move theMove(curLocation, newLocation);
                Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                tempJumpLocation.push_back(newLocation);
                computeGoldMoves(newBoard, newLocation);
                tempJumpLocation.pop_back();
            }
        }
        if (isKing) {
            if (isValidLocation(aBoard, row - 1, col - 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col - 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col - 2);
                    Location capturedLocation(row - 1, col - 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeGoldMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
            if (isValidLocation(aBoard, row - 1, col + 1, 'b')) {
                if (isValidLocation(aBoard, row - 2, col + 2)) {
                    outOfMove = false;
                    Location newLocation(row - 2, col + 2);
                    Location capturedLocation(row - 1, col + 1);
                    Move theMove(curLocation, newLocation);
                    Board newBoard = createNewBoard(aBoard, theMove, capturedLocation);
                    tempJumpLocation.push_back(newLocation);
                    computeGoldMoves(newBoard, newLocation);
                    tempJumpLocation.pop_back();
                }
            }
        }

        if (!aFirstMove && outOfMove) {
            evaluateMove(aBoard, aLocation, true);
        }

    }

    bool GeraltPlayer2::takeTurn(Game& aGame, Orientation aDirection, std::ostream& aLog) {
        size_t theCount = aGame.countAvailablePieces(color);
        updateBoardStatus(aGame);
        //showBoard(this->theBoard);

        for (int pos = 0; pos < theCount; pos++) {
            if (const Piece* thePiece = aGame.getAvailablePiece(this->color, pos)) {
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
            for (auto location : bestJumpLocation)
                aGame.movePieceTo(*bestJumpPiece, location);
            return true;
        }

        //make the move with best score
        //if (rand() % 3 == 0) {
            if (bestPiece != nullptr) {
                aGame.movePieceTo(*bestPiece, bestLocation);
                return true;
            }
        /*}
        else {
            int randomNum = rand() % theRandomMoves.size();
            for (auto& cur : theRandomMoves) {
                if (--randomNum == 0) {
                    aGame.movePieceTo(*(cur.first), cur.second);
                    std::cout << "move " << cur.first->getLocation().row << ", " << cur.first->getLocation().row
                        << " to" << cur.second.row << ", " << cur.second.col << '\n';
                    return true;
                }
            }
        }*/

        //no available piece, forfeit
        return true;
    }

}