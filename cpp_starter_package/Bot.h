#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include "Location.h"
#include "Route.h"
#include "AStar.h"
#include <map>
#include <vector>
#include <set>
#include <queue>

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    State state;
    std::map<Location, Location>* orders = new std::map<Location, Location>();
    std::set<Location>* unseenTiles = new std::set<Location>(); //TODO : replace with bigger zones unseen
    std::set<Location>* enemyTiles = new std::set<Location>();
    const int antLimitVariable = 30;
    const int nbAntDefendingPercent = 70;
    const int maxColonyBordersDistance = 8;

    Bot();

    void playGame();    //plays a single game of Ants
    void searchFood(std::vector<Location> sortedAnts);
    void attackEnemy(std::vector<Location> sortedAnts);
    void attackFormation(std::vector<Location> sortedAnts);
    void defenseFormation(std::vector<Location> sortedAnts, Location myHill, int antLimit);
    std::set<Location> calculateDefensePositions(Location myHill);
    bool canAntMoveThere(Location loc);
    std::vector<Location> getAllLocationsBetween(int x0, int x1, int y0, int y1);
    void explore(std::vector<Location> sortedAnts);

    bool doMoveDirection(const Location& antLoc, int direction);
    bool doMoveLocation(const Location& start, const Location& dest);

    void getOutOfHills();
    bool mapContainsValue(std::map<Location, Location> locations, Location value);

    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
