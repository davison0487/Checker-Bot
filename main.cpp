//
//  main.cpp
//  Checkers
//
//  Created by rick gessner on 2/22/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "Game.hpp"
#include "Player.hpp"

//STUDENT: include your own playere file here
//         (named after you, like "RenjiePlayer")...
//         Use your player below, instead of the default Player class

std::ostream& getOutput() {return std::cout;}

int main(int argc, const char * argv[]) {
  
  const char* theStatus[]={"FAIL","PASS"};
        
  // insert code here...
  if(argc>1) {
    
    ECE141::Player player1;  //Use your own player class here...
    ECE141::Player player2;  //Make two players from YOUR player class
    
    std::stringstream theStream;
    
    srand(static_cast<uint32_t>(time(0)));
    std::string temp(argv[1]);
    
    if("compile"==temp) {
      std::cout << "Compile test PASS\n";
    }
    else if("win"==temp) {
      std::cout << "Win test ";
      auto theResult=ECE141::Game::run(player1, player2, theStream);
      std::cout << theStatus[ECE141::Reasons::eliminated==theResult];
      std::cout << "\n" << theStream.str() << "\n";
    }
    else if ("piece"==temp) {
      std::cout << "Piece test FAIL\n";
    }
    else if ("move"==temp) {
      std::cout << "Move test FAIL\n";
    }
    else if ("choice"==temp) {
      std::cout << "Choice test FAIL\n";
    }
  }
  
  return 0;
}
