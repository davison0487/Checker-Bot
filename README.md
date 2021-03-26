# Checker Bot
## Let's Play Checkers
This is a checker bot that will make decisions according to the objective function.

### The Neighborhood Function

A _neighbourhood function_ is a simple function that returns set of the states that are considered 'adjacent' to current state. These are the options that the `Player` must consider during its turn. For example, given any Pawn on the gameboard, it can move only two directions : forward left, or forward right (because Pawns only move forward, and all moves are diagonal). 

### The Objective Function

An objective function indicates the 'quality/rank' of a given option, in terms of leading to the optimal outcome.

Consider the case where you have two Pawns that can move forward by 1 square. In a sense, these options appear to have exactly the same value. But what if one of your Pawns is only a square away from the back row of your opponent, where it can become a King. In this case, moving that piece produces a result of higher value to the outcome of your game (since Kings are more flexible and thus a much greater threat than Pawns). Between these two move options, the "move to become a King" has greater value, and is therefore ranked higher by your objective function.

1. move to back-row, get "kinged"  (best)
2. One ore more safe jumps over opponent(s) (landing somewhere aren't at risk) 
3. Jumping (and caputuring opponent) but landing in a risky spot
3. Moving safely (not diagonally adjacent to opponent)
4. Moving diagonally adjacent to opponent (risk of capture)
