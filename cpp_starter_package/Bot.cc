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

	for (Location hill : state.myHills)
	{
		orders->insert({ hill, Location(-1,-1)});
	}

	orders->clear();

	map<Location, Location> foodTargets = map<Location, Location>();

	vector<Route> foodRoutes;
	vector<Location> sortedFood = state.food;
	vector<Location> sortedAnts = state.myAnts;

	for (Location foodLoc : sortedFood) {
		for (Location antLoc : sortedAnts) {
			int distance = state.distance(antLoc, foodLoc);
			Route route(antLoc, foodLoc, distance);
			foodRoutes.push_back(route);
		}
	}

	std::sort(foodRoutes.begin(), foodRoutes.end());
	for (Route& route : foodRoutes) {
		if (foodTargets.count(route.getEnd()) == 0
			&& !mapContainsValue(foodTargets, route.getStart())
			&& doMoveLocation(route.getStart(), route.getEnd())) {
			foodTargets[route.getEnd()] = route.getStart();
		}
	}

	for (Location hill : state.myHills)
	{
		if (antOnLocation(state.myAnts, hill) && !mapContainsValue(*orders, hill))
		{
			for (int d = 0; d < TDIRECTIONS; d++)
			{
				//We check if we have an ant at the new location
				if (!state.doesContainsAnt(state.getLocation(hill, d)) && doMoveDirection(hill, d))
				{
					break;
				}
			}
		}
	}

	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

bool Bot::mapContainsValue(map<Location, Location> loc, Location value)
{
	for (const auto &pair: loc)
	{ 
		if (pair.second == value) return true;
	}
	return false;
}

bool Bot::antOnLocation(vector<Location> ants, Location hill)
{
	for (Location ant : ants)
	{
		if (ant == hill) return true;
	}
	return false;
}

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