#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <stdint.h>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Ant.h"


class Ant;
class Location;
/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}
const std::vector<Location> exploPoints = { Location(16, 70), Location(15, 112), Location(14, 31), Location(34, 70), Location(40, 90), Location(32, 114), Location(9, 11), Location(16, 71), Location(15, 113), Location(14, 32), Location(34, 71), Location(40, 91), Location(32, 115), Location(9, 12), Location(23, 46), Location(23, 47), Location(6, 15), Location(7, 16), Location(40, 9), Location(40, 10), Location(27, 58), Location(20, 59), Location(47, 78), Location(47, 79) };
/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;
    int64_t seed;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;
    std::vector<Ant*> antList;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();

    void makeMove(const Location &loc, int direction);
    Ant* FindAntWithLocation(const Location& loc);
    Ant* FindAntWithFutureLocation(const Location& loc);
    bool AntIsInLocationList(Ant* ant);
    double distance(const Location& loc1, const Location& loc2);
    double manhattanDistance(const Location& loc1, const Location& loc2);

    bool doesContainsAnt(const Location& loc);
    Location getLocation(const Location& startLoc, int direction);
    std::vector<int> getDirections(const Location& startLoc, const Location& endLoc);
    std::vector<Location> getNeighbors(const Location& loc);

    void updateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_
