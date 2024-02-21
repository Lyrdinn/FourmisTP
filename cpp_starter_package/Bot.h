#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include "Location.h"
#include "Route.h"
#include <map>
#include <vector>
#include <set>

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    State state;
    std::map<Location, Location>* orders = new std::map<Location, Location>();
    std::set<Location>* unseenTiles = new std::set<Location>(); //TODO : replace with bigger zones unseen

    Bot();

    void playGame();    //plays a single game of Ants
    bool doMoveDirection(const Location& antLoc, int direction);
    bool doMoveLocation(const Location& antLoc, const Location& newLoc);
    void searchFood(std::vector<Location> sortedAnts);
    void explore(std::vector<Location> sortedAnts);
    void getOutOfHills();
    bool mapContainsValue(std::map<Location, Location> locations, Location value);

    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
