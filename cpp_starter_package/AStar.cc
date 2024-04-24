#include "AStar.h"

using namespace std;


// -------------------------------------- A-star methods --------------------------------------

// Check if we can move on a given node
bool isValid(State &state, Node &node)
{
	if (node.x < state.rows && node.y < state.cols && node.x >= 0 && node.y >= 0) {
		Square square = state.grid[node.x][node.y];
		if (square.isHill) return false;
		if (square.isWater) return false;
		if (square.ant != -1) return false;
		return true;
	}
	else return false;
}

// Check if a given node is the destination
bool isDestination(Node &node, Node &dest) 
{
    if (node.x == dest.x && node.y == dest.y) return true;
    return false;
}

// Returns the Manhattan distance between two nodes with the edges wrapped
double distanceManhattan(State &state, const Node &node1, const Node &node2)
{
	return state.manhattanDistance(Location(node1.x, node1.y), Location(node2.x, node2.y));
    /*int d1 = abs(node1.x - node2.x),
        d2 = abs(node1.y - node2.y),
        dr = min(d1, state.rows - d1),
        dc = min(d2, state.cols - d2);
    return dr + dc;*/
};

const int STRAIGHT_COST = 10;
const int DIAGONAL_COST = 14;

int calculateDistanceCost(const Node &a, const Node &b) {
	int xDistance = abs(a.x - b.x);
	int yDistance = abs(a.y - b.y);
	int remaining = abs(xDistance - yDistance);
	return DIAGONAL_COST * fminf(xDistance, yDistance) + STRAIGHT_COST * remaining;
}

// Use A-star pathfinding to returns the shortest path from start to dest
vector<Node> aStar(State &state, Node &start, Node &dest)
{
	vector<Node> empty;

	if (state.timer.getTime() > 500) return empty;

	if (!isValid(state, dest)) 
	{
		state.bug << "Destination is an obstacle" << endl;
		return empty;
	}
	if (isDestination(start, dest)) 
	{
		state.bug << "You've already reached the destination" << endl;
		return empty;
	}	

	// The set of nodes to be evaluated
	// Set : allow direct access to the node with the lowest f-cost
	set<Node> open;

	// The map of nodes already evaluated
	// Map : know immediately if a node is already evaluated or not
	int rows = state.rows;
	int cols = state.cols;

	vector<vector<bool> > closed(rows, vector<bool>(cols));
	vector<vector<Node> > map(rows, vector<Node>(cols));


	// Initialize all nodes
	for (int x = 0; x < rows; x++)
	{
		for (int y = 0; y < cols; y++)
		{
			map[x][y].fCost = (std::numeric_limits<int>::max)();
			map[x][y].gCost = (std::numeric_limits<int>::max)();
			map[x][y].hCost = (std::numeric_limits<int>::max)();
			map[x][y].parentX = -1;
			map[x][y].parentY = -1;
			map[x][y].x = x;
			map[x][y].y = y;

			closed[x][y] = false;
		}
	}

	// Initialize open list with starting node
	int x = start.x;
	int y = start.y;
	map[x][y].fCost = 0.0;
	map[x][y].gCost = 0.0;
	map[x][y].hCost = 0.0;
	open.insert(map[x][y]);

	while (!open.empty())
	{
		// Get lowest f-cost in open
		Node current = *open.begin();

		// Move it from open to close
		open.erase(open.begin());
		closed[current.x][current.y] = true;

		// If we reach the destination
		if (isDestination(current, dest))
		{
			vector<Node> path;
			int x = current.x;
			int y = current.y;

			int num = 0;
			// Retrace path
			while (!(map[x][y].parentX == start.x && map[x][y].parentY == start.y))
			{
				int tempX = map[x][y].parentX;
				int tempY = map[x][y].parentY;
				x = tempX;
				y = tempY;
				num++;
				//state.bug << "cell step : " << x << "; " << y << endl; //to show all steps of path
			}

			// state.bug << "total : " << num << endl; //to show the total number of move needed to arrive to destination

			// Because we are calculating the path every frame, we only need the next node
			Node targetNode;
			targetNode.x = x;
			targetNode.y = y;
			path.push_back(targetNode);
			return path;
		}

		vector<Node> neighbours;
		Node east = map[(current.x + 1 + rows) % rows][current.y];
		Node west = map[(current.x - 1 + rows) % rows][current.y];
		Node north = map[current.x][(current.y + 1 + cols) % cols];
		Node south = map[current.x][(current.y - 1 + cols) % cols];
		neighbours.push_back(east);
		neighbours.push_back(west);
		neighbours.push_back(north);
		neighbours.push_back(south);

		for (Node neighbour : neighbours)
		{
			// If neighbour is not valid or already evaluated, skip to the next neighbour
			if (!isValid(state, neighbour) || closed[neighbour.x][neighbour.y])
			{
				continue;
			}

			// If new path to neighbour is shorter or neighbour is not in open
			if (current.gCost + 1 < neighbour.gCost || neighbour.fCost == (std::numeric_limits<int>::max)())
			{
				// If is already in open, remove it temporarily
				if (neighbour.fCost != (std::numeric_limits<int>::max)())
				{
					open.erase(map[neighbour.x][neighbour.y]);
				}

				map[neighbour.x][neighbour.y].gCost = current.gCost + calculateDistanceCost(current, neighbour);
				map[neighbour.x][neighbour.y].hCost = calculateDistanceCost(neighbour, dest);
				map[neighbour.x][neighbour.y].fCost = map[neighbour.x][neighbour.y].gCost + map[neighbour.x][neighbour.y].hCost;
				map[neighbour.x][neighbour.y].parentX = current.x;
				map[neighbour.x][neighbour.y].parentY = current.y;
				open.insert(map[neighbour.x][neighbour.y]);
			}
		}
	}

	state.bug << "Destination not found" << endl;
	return empty;
}



// ------------------------------------- Helper methods -------------------------------------

//if there isn't an ant that was already at the direction we move there and return true
bool Bot::doMoveDirection(const Location& antLoc, int direction) {
	Location newLoc = state.getLocation(antLoc, direction);

	if (!state.grid[newLoc.row][newLoc.col].isWater && orders->count(newLoc) == 0)
	{
		state.makeMove(antLoc, direction);
		orders->insert({ newLoc, antLoc });
		return true;
	}
	return false;
}

// Move an ant towards the destination
// This method makes the connection between locations and nodes
bool Bot::doMoveLocation(const Location &start, const Location &dest)
{
	Node startNode;
	startNode.x = start.row;
	startNode.y = start.col;

	state.bug << "Start Node :" << startNode.x << " " << startNode.y << endl;

	Node destNode;
	destNode.x = dest.row % state.rows;
	destNode.y = dest.col % state.cols;

	state.bug << "Dest Node :" << destNode.x << " " << destNode.y << endl;

	// Get path with A-star
	vector<Node> path = aStar(state, startNode, destNode);
	if(path.empty()) return false;

	state.bug << "Next Node :" << path[0].x << " " << path[0].y  << endl;

	Location newLocation = Location(path[0].x, path[0].y);

	// Move ant one square
	vector<int> directions = state.getDirections(start, newLocation);
	for (int d : directions)
	{
	 	if (doMoveDirection(start, d))
	 	{
	 		return true;
	 	}
	}
	return false;
}