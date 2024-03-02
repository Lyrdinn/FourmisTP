#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
	//reads the game parameters and sets up
	cin >> state;
	state.setup();
	endTurn();

	//continues making moves while the game is not over
	while (cin >> state)
	{
		state.updateVisionInformation();
		makeMoves();
		endTurn();
	}
};

//makes the bots moves for the turn
//makes the bots moves for the turn
void Bot::makeMoves()
{
	state.bug << "turn " << state.turn << ":" << endl;
	state.bug << state << endl;

	//We clear all our orders and sort our ants by location
	orders->clear();
	vector<Location> sortedAnts = state.myAnts;

	//If we haven't initialized our unseen tiles yet we add all the Locations
	if (unseenTiles->size() == 0)
	{
		for (int row = 0; row < state.rows; row++)
		{
			for (int col = 0; col < state.cols; col++)
			{
				unseenTiles->insert(Location(row, col));
			}
		}
	}

	//We remove every tiles we can see in the map
	set<Location>::iterator it;
	for (it = unseenTiles->begin(); it != unseenTiles -> end(); ++it)
	{
		if (state.grid[it->row][it->col].isVisible)
		{
			it = unseenTiles->erase(it);
		}
	}

	//We add our hills to our orders so that our ants don't go there.
	for (Location hill : state.myHills)
	{
		orders->insert({ hill, Location(-1,-1) });
	}

	if (state.myAnts.size() >= 7)
	{
		defenseFormation(sortedAnts, Location(6, 71), Location(12, 75));
	}

	//We Search for food
	searchFood(sortedAnts);

	//We attack the enemy ants if we see them
	attackEnemy(sortedAnts);

	//We explore other areas
	explore(sortedAnts);

	//We check for each of our hills if we currently have an ant stepping on it so we can move it further
	getOutOfHills();

	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//We tell each of our ants to go to the closest food visible
void Bot::searchFood(vector<Location> sortedAnts)
{
	map<Location, Location> foodTargets = map<Location, Location>();
	vector<Route> foodRoutes;
	vector<Location> sortedFood = state.food;

	//For each location that contains food and for each of our ants we check which one is closest
	//and create a route
	for (Location foodLoc : sortedFood)
	{
		for (Location antLoc : sortedAnts)
		{
			int distance = state.distance(antLoc, foodLoc);
			Route route(antLoc, foodLoc, distance);
			foodRoutes.push_back(route);
		}
	}

	//We sort our routes by the distance and move our ants according to the shortest routes
	std::sort(foodRoutes.begin(), foodRoutes.end());
	for (Route& route : foodRoutes) {
		if (foodTargets.count(route.getEnd()) == 0
			&& !mapContainsValue(foodTargets, route.getStart())
			&& doMoveLocation(route.getStart(), route.getEnd()))
		{
			foodTargets[route.getEnd()] = route.getStart();
		}
	}
}

void Bot::attackEnemy(vector<Location> sortedAnts)
{
	//We add all of the enemy hills to
	for (Location enemyHill : state.enemyHills)
	{
		if (enemyTiles->count(enemyHill) == 0)
		{
			enemyTiles->insert(enemyHill);
		}
	}

	vector<Route> hillRoutes = vector<Route>();
	for (Location hillLoc : *enemyTiles)
	{
		//We grab each of our ant and make them attack the colony
		for (Location antLoc : sortedAnts)
		{
			if (!mapContainsValue(*orders, antLoc))
			{
				int distance = state.distance(antLoc, hillLoc);
				Route route = Route(antLoc, hillLoc, distance);
				hillRoutes.push_back(route);
			}
		}
	}

	//We move each of our ants
	std::sort(hillRoutes.begin(), hillRoutes.end());
	for (Route route : hillRoutes)
	{
		doMoveLocation(route.getStart(), route.getEnd());
	}
}

void Bot::attackFormation(std::vector<Location> sortedAnts)
{
	
}

void Bot::defenseFormation(std::vector<Location> sortedAnts, const Location& lignBeg, const Location& lignEnd)
{
	map<Location, Location> defenseTarget = map<Location, Location>();
	vector<Location> locations = getAllLocationsBetween(lignBeg.col, lignEnd.col, lignBeg.row, lignEnd.row);
	vector<Route> defenseRoute;

	//For each of our defense location and for each of our ants we check which one is closest
	//and create a route
	for (Location defLoc : locations)
	{
		for (Location antLoc : sortedAnts)
		{
			int distance = state.distance(antLoc, defLoc);
			Route route(antLoc, defLoc, distance);
			defenseRoute.push_back(route);
		}
	}

	//We sort our routes by the distance and move our ants according to the shortest routes
	std::sort(defenseRoute.begin(), defenseRoute.end());
	for (Route& route : defenseRoute) {
		if (defenseTarget.count(route.getEnd()) == 0
			&& !mapContainsValue(defenseTarget, route.getStart())
			&& doMoveLocation(route.getStart(), route.getEnd()))
		{
			defenseTarget[route.getEnd()] = route.getStart();
		}
	}
}

//Uses Bresenham' algorithm to calculate each locations between two points
//https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
vector<Location> Bot::getAllLocationsBetween(int x0, int x1, int y0, int y1)
{
	vector<Location> locations;

	//We calculate or slope values for our line
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	//We try to find the sign to figure out where in each diagonal directions our line is going
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;

	int e = dx - dy;

	while (x0 != x1 && y0 != y1)
	{
		locations.push_back(Location(y0, x0));
		int e2 = 2 * e;

		//Compares to the real lign it's vertical distance
		if (e2 > -dy)
		{
			if (x0 == x1) break;
			e -= dy;
			x0 += sx;
		}
		//Compares to the real lign it's horizontal distance
		if (e2 < dx)
		{
			if (y0 == y1) break;
			e += dx;
			y0 += sy;
		}	
	}

	return locations;
}

//Our ants will explore unseen tiles in the map
void Bot::explore(vector<Location> sortedAnts)
{
	for (Location antLoc : sortedAnts)
	{
		//if we don't have orders for this ant yet we create unseen routes
		if (!mapContainsValue(*orders, antLoc))
		{
			vector<Route> unseenRoutes = vector<Route>();

			set<Location>::iterator unseenLoc;
			//For each of the unseen locations in our map we create a route
			for (unseenLoc = unseenTiles->begin(); unseenLoc != unseenTiles->end(); unseenLoc++)
			{
				int distance = state.distance(antLoc, *unseenLoc);
				Route route = Route(antLoc, *unseenLoc, distance);
				unseenRoutes.push_back(route);
			}

			//We sort the route by the shortest one to explore the most nearby place
			std::sort(unseenRoutes.begin(), unseenRoutes.end());
			for (Route route : unseenRoutes)
			{
				if (doMoveLocation(route.getStart(), route.getEnd()))
				{
					break;
				}
			}
		}
	}
}

void Bot::getOutOfHills()
{
	for (Location hill : state.myHills)
	{
		if (state.doesContainsAnt(hill) && !mapContainsValue(*orders, hill))
		{
			for (int d = 0; d < TDIRECTIONS; d++)
			{
				//We check if we have an ant at the new location and if not we can move
				if (!state.doesContainsAnt(state.getLocation(hill, d)) && doMoveDirection(hill, d))
				{
					break;
				}
			}
		}
	}
}

//returns true if our map does contain the location value
bool Bot::mapContainsValue(map<Location, Location> loc, Location value)
{
	for (const auto &pair: loc)
	{ 
		if (pair.second == value) return true;
	}
	return false;
}

//if there isn't an ant that was already at the direction we move there and return true
bool Bot::doMoveDirection(const Location& antLoc, int direction) {
	Location newLoc = state.getLocation(antLoc, direction);

	if (!state.grid[newLoc.row][newLoc.col].isWater && orders -> count(newLoc) == 0)
	{
		state.makeMove(antLoc, direction);
		orders->insert({ newLoc, antLoc });
		return true;
	}
	return false;
}

//moves an ant at a specified location
bool Bot::doMoveLocation(const Location& antLoc, const Location& newLoc)
{
	vector<int> directions = state.getDirections(antLoc, newLoc);
	for (int d : directions)
	{
		if (doMoveDirection(antLoc, d))
		{
			return true;
		}
	}
	return false;
}

//finishes the turn
void Bot::endTurn()
{
	if (state.turn > 0)
		state.reset();
	state.turn++;

	cout << "go" << endl;
};