#ifndef ASTAR_H_
#define ASTAR_H_

#include "Location.h"
#include "State.h"
#include "Bot.h"
#include <array>
#include <vector>
#include <set>
#include <limits>

struct Node
{
    int y;
    int x;
    int parentX;
    int parentY;
    int gCost;
    int hCost;
    int fCost;

    // We need to overload the "<" operator to put our struct into a set
    inline bool operator < (const Node& node) const
    {
        return fCost < node.fCost || (fCost == node.fCost && hCost < node.hCost);
    }
 
};

#endif