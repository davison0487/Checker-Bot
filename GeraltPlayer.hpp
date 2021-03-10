//
//  GeraltPlayer.hpp
//  Checkers
//
//  Created by Yunhsiu Wu on 3/7/21.
//

#ifndef GeraltPlayer_hpp
#define GeraltPlayer_hpp

#include "Piece.hpp"
#include "Player.hpp"
#include <iostream>
#include <vector>
#include <stack>

namespace ECE141 {

    using Board = std::vector<std::vector<char>>;

    class Game; //forward declare...  
    
    //store a old position and a new position
    struct Move {
    public:
        Location oldLocation;
        Location newLocation;
        Move(Location& anOld, Location& aNew) :oldLocation(anOld), newLocation(aNew) {}
    };

    class GeraltPlayer : public Player {
    public:
        GeraltPlayer();
        virtual bool      takeTurn(Game& aGame, Orientation aDirection, std::ostream& aLog);
                
    protected:
        //reset varibles and update board status from the game
        void updateBoardStatus(Game& aGame);

        //determine if a location is valid, look for empty spot by default
        bool isValidLocation(const Board& aBoard, int aRow, int aCol, char aPiece = '.');

        //evaluate the safety score of a new board and new location
        int blueSafetyScore(const Board& aBoard, Location& aLocation);
        int goldSafetyScore(const Board& aBoard, Location& aLocation);

        //evaluate the total score of a new board and new location
        void evaluateMove(const Board& aBoard, Location& aLocation, bool aJump = false);
        
        //move a piece and creat a new board
        Board createNewBoard(const Board& aBoard, Move& aMove, Location aCaptured = Location(-1, -1));

        //show the board, for debugging
        void showBoard(const Board& aBoard);

        //compute and evaluate moves
        void computeBlueMoves(const Board &aBoard, Location aLocation, bool aFirstMove = false);
        void computeGoldMoves(const Board &aBoard, Location aLocation, bool aFirstMove = false);

        Board theBoard;

        /* used for evaluating moves */
        const Piece* curPiece;
        const Piece* bestJumpPiece;
        std::vector<Location> tempJumpLocation;
        std::vector<Location> bestJumpLocation;
        int maxJumpScore;
        const Piece* bestPiece;
        Location bestLocation;
        int maxScore;
        /* used for evaluating moves */
    };

    class GeraltPlayer2 : public GeraltPlayer {
        void evaluateMove(const Board& aBoard, Location& aLocation, bool aJump = false);
    };
}

#endif /* GeraltPlayer_hpp */