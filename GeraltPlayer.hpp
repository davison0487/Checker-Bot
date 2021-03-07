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

    //enum class PieceColor  {gold, blue};
    //blue's row index goes up, gold's row index goes down

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
        
        
    private:
        //reset varibles and update board status from the game
        void updateBoardStatus(Game& aGame);

        //determine if a location is valid
        bool isValidLocation(Location& aLocation);

        //evaluate the safety score of a new board and new location
        int safetyScore(const Board& aBoard, Location& aLocation);

        //evaluate the total score of a new board and new location
        int evaluateMove(const Board& aBoard, Location& aLocation);
        
        //move a piece and creat a new board
        Board createNewBoard(const Board &aBoard, Move& aMove);

        //compute and evaluate moves
        void computeBlueMoves(const Board &aBoard, Location aLocation, bool aFirstMove = false);
        void computeGoldMoves(const Board &aBoard, Location aLocation, bool aFirstMove = false);
                
        Board theBoard;

        const Piece* curPiece;
        const Piece* bestPiece;
        Location bestLocation;
        int maxScore;
    };
}

#endif /* GeraltPlayer_hpp */