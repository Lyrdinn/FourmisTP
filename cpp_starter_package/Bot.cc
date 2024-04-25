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
void Bot::makeMoves()
{
	state.bug << "turn " <<  state.turn << endl;
	//state.bug << state << endl;

	//We clear all our orders and sort our ants by location
	orders->clear();
	vector<Location> sortedAnts = state.myAnts;

	state.timer.start();

	for (size_t i = 0; i < sortedAnts.size(); i++)
	{
		Location currentLoc = sortedAnts[i];
		Ant* foundMatchingAnt = nullptr;
		foundMatchingAnt = state.FindAntWithFutureLocation(currentLoc);
		if (foundMatchingAnt == nullptr) { //if no ant matches for this location, add new ant
			state.antList.push_back(new Ant(currentLoc));
		}
		else {
			foundMatchingAnt->currentLocation = currentLoc; //update current ant location to the old future location
		}
	}

	for (size_t i = 0; i < state.antList.size(); i++) //need to kill ant not found
	{
		bool found = false;
		for (size_t j = 0; j < sortedAnts.size(); j++)
		{
			if (state.antList[i]->currentLocation == sortedAnts[j]) {
				found = true;
				break;
			}
		}
		if (!found) {
			state.antList.erase(state.antList.begin() + i);
		}
	}

	state.bug << "ant number : " << state.antList.size() << endl;

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
	for (it = unseenTiles->begin(); it != unseenTiles -> end();)
	{
		if (state.grid[it->row][it->col].isVisible)
		{
			state.grid[it->row][it->col].exploreValue = 0;
			it = unseenTiles->erase(it);
		}
		else
		{
			if (state.grid[it->row][it->col].exploreValue < 10)
				state.grid[it->row][it->col].exploreValue++;
			++it;
			
		}
	}

	//We add our hills to our orders so that our ants don't go there.
	for (Location hill : state.myHills)
	{
		orders->insert({ hill, Location(-1,-1) });
	}

	if (state.myAnts.size() >= 35)
	{
		//We assign ants to defend our base
		defenseFormation(sortedAnts, state.myHills.front(), state.myAnts.size() + antLimitVariable);
	}

	//We Search for food
	searchFood(sortedAnts);

	//We attack the enemy ants if we see them
	//attackFormation(sortedAnts);

	//We explore other areas
	explore(sortedAnts);

	//We check for each of our hills if we currently have an ant stepping on it so we can move it further
	getOutOfHills();

	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//We tell each of our ants to go to the closest food visible
void Bot::searchFood(vector<Location> sortedAnts)
{
	state.bug << "STATE : SEARCH FOOD" << endl;
	map<Location, Location> foodTargets = map<Location, Location>();
	vector<Route> foodRoutes;
	vector<Location> sortedFood = state.food;

	//For each location that contains food and for each of our ants we check which one is closest
	//and create a route
	for (Location foodLoc : sortedFood)
	{
		for (Location antLoc : sortedAnts)
		{
			int distance = state.manhattanDistance(antLoc, foodLoc);
			if (distance == 0) break;

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


void Bot::attackFormation(std::vector<Location> sortedAnts)
{

	state.bug << "STATE : ATTACK FORMATION" << endl;
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
				int distance = state.manhattanDistance(antLoc, hillLoc);
				if (distance != 0)
				{
					Route route = Route(antLoc, hillLoc, distance);
					hillRoutes.push_back(route);
				}
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

//ATTACK : Uses Bresenham' algorithm to calculate each locations between two points for the attack
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

	locations.push_back(Location(y1, x1));
	return locations;
}

void Bot::defenseFormation(vector<Location> sortedAnts, Location myHill, int antLimit)
{
	state.bug << "STATE : DEFENSE FORMATION" << endl;

	map<Location, Location> defenseTarget = map<Location, Location>();

	//We stock our BFS for the defense of each of our hills so that we don't have to recalculate them
	//every frame. We check if it exists or not
	if (defenseLocationsPerHills->count(myHill) == 0)
	{
		defenseLocationsPerHills->insert({ myHill, calculateDefensePositions(myHill) });
	}
	set<Location> defenseLocation = defenseLocationsPerHills->at(myHill);

	//We want to pick the best routes that are the furtest from the hill to defend and pull out a max of ants.
	vector<Route> defRouteFromHill;
	for (Location defLoc : defenseLocation)
	{
		if (!state.doesContainsAnt(defLoc))
		{
			int distance = state.manhattanDistance(myHill, defLoc);
			if (distance != 0)
			{
				Route route(myHill, defLoc, distance);
				defRouteFromHill.push_back(route);
			}
		}
	}
	std::sort(defRouteFromHill.begin(), defRouteFromHill.end());

	//For each of our defense location and for each of our ants we check which one is closest
	//and create a route. We go from the furtest routes from our hill to the closest.
	vector<Route> defenseRoute;
	int ilimit = 0;
	for (std::vector<Route>::reverse_iterator it = defRouteFromHill.rbegin(); it != defRouteFromHill.rend(); ++it)
	{
		Location defLoc = it->getEnd();
		for (Location antLoc : sortedAnts)
		{
			int distance = state.manhattanDistance(antLoc, defLoc);
			if (distance != 0)
			{
				Route route(antLoc, defLoc, distance);
				defenseRoute.push_back(route);
			}
		}

		ilimit++;
		if (ilimit >= antLimit) break;
	}

	//We want to have only X percent of our ants defending
	int nbAntDefending = 0;
	int maxNbAntDefending = sortedAnts.size() * nbAntDefendingPercent / 100;

	//We sort our routes by the distance and move our ants according to the shortest routes
	std::sort(defenseRoute.begin(), defenseRoute.end());
	for (Route& route : defenseRoute) {
		if (defenseTarget.count(route.getEnd()) == 0
			&& !mapContainsValue(defenseTarget, route.getStart())
			&& doMoveLocation(route.getStart(), route.getEnd()))
		{
			defenseTarget[route.getEnd()] = route.getStart();
			nbAntDefending += 1;
		}

		//if we go above X percent of ants defending we break
		if (nbAntDefending >= maxNbAntDefending) break;

		if (state.timer.getTime() > 450) break;
	}
}

//Uses BFS algorithm to calculate from the hill the maximum number of ants we can place in a line
//https://en.wikipedia.org/wiki/Breadth-first_search
set<Location> Bot::calculateDefensePositions(Location myHill)
{
	set<Location> visited;
	queue<Location> tilesToSearch;

	tilesToSearch.push(myHill);
	while (tilesToSearch.size() > 0)
	{
		Location loc = tilesToSearch.front();

		state.bug << "Location : r " << loc.row << " c " << loc.col << endl;

		tilesToSearch.pop();

		visited.insert(loc);

		//We add all the neighbors tiles not visited and valid to our queue.
		vector<Location> neighbors = state.getNeighbors(loc);
		for (Location nTile : neighbors)
		{
			//if tile not in visited)
			if (visited.count(nTile) == 0)
			{
				int distance = state.manhattanDistance(myHill, nTile);

				//If not water, not enemy tile and distance < maxColonyBordersDistance
				if (canAntMoveThere(nTile) && distance <= maxColonyBordersDistance)
				{
					tilesToSearch.push(nTile);
				}
			}
		}
	}

	return visited;
}

bool Bot::canAntMoveThere(Location loc)
{
	//We check if is water
	if (state.grid[loc.row][loc.col].isWater)
	{
		return false;
	}
	else
	{
		//We check if there is an ant or not
		if (state.doesContainsAnt(loc))
		{
			//if enemy then we can't go there (obviously)
			if (std::find(state.enemyAnts.begin(), state.enemyAnts.end(), loc) != state.enemyAnts.end())
			{
				return false;
			}
		}
		return true;
	}
}

//Our ants will explore unseen tiles in the map
void Bot::explore(vector<Location> sortedAnts)
{
	state.bug << "STATE : EXPLORE" << endl;
	for (Location antLoc : sortedAnts)
	{
		//if we don't have orders for this ant yet we create unseen routes
		if (!mapContainsValue(*orders, antLoc))
		{
			state.bug << "antloc : " << antLoc.row << " | " << antLoc.col << endl;
			//search all 11 far tiles and pick the one with the highest exploreValue (if none more than 0, don't move)

			vector<Route> unseenRoutes = vector<Route>();

			set<Location>::iterator unseenLoc;
			//For each of the unseen locations in our map we create a route
			for (unseenLoc = unseenTiles->begin(); unseenLoc != unseenTiles->end(); unseenLoc++)
			{
				int distance = state.manhattanDistance(antLoc, *unseenLoc);
				if (distance == 0) break;

				Route route = Route(antLoc, *unseenLoc, distance);
				unseenRoutes.push_back(route);
			}

			//We sort the route by the shortest one to explore the most nearby place
			std::sort(unseenRoutes.begin(), unseenRoutes.end());
			for (Route route : unseenRoutes)
			{
				if (state.timer.getTime() > 450) break;
				if (doMoveLocation(route.getStart(), route.getEnd()))
				{
					state.bug << "route : " << route.getEnd().row << " | " << route.getEnd().col << endl;
					break;
				}
			}
		}
	}
}
//
//void Bot::breathFirstSearch() {
//
//	vector<Location> unexploredLocations = vector<Location>();
//vector<Location> exploredLocations = vector<Location>();

//unexploredLocations.push_back(antLoc);
//}

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

//finishes the turn
void Bot::endTurn()
{
	if (state.turn > 0)
		state.reset();
	state.turn++;

	cout << "go" << endl;
};