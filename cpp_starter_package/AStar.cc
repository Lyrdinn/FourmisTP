#include "AStar.h"

using namespace std;


// -------------------------------------- A-star methods --------------------------------------

// Check if we can move on a given node
bool isValid(State &state, Node &node)
{
	Square square = state.grid[node.x][node.y];
	if (square.isHill) return false;
	if (square.isWater) return false;
	if (square.isFood) return false;
	if (square.ant != -1) return false;
	return true;
}

// Check if a given node is the destination
bool isDestination(Node &node, Node &dest) 
{
    if (node.x == dest.x && node.y == dest.y) return true;
    return false;
}

// Returns the euclidean distance between two nodes with the edges wrapped
double distance(State &state, const Node &node1, const Node &node2)
{
    int d1 = abs(node1.x - node2.x),
        d2 = abs(node1.y - node2.y),
        dr = min(d1, state.rows - d1),
        dc = min(d2, state.cols - d2);
    return sqrt(dr*dr + dc*dc);
};

// Use A-star pathfinding to returns the shortest path from start to dest
vector<Node> aStar(State &state, Node &start, Node &dest)
{
	vector<Node> empty;

	if (!isValid(state, dest)) 
	{
		cout << "Destination is an obstacle" << endl;
		return empty;
	}

	if (isDestination(start, dest)) 
	{
		cout << "You've already reached the destination" << endl;
		return empty;
	}	

	// The set of nodes to be evaluated
	// Set : allow direct access to the node with the lowest f-cost
	set<Node> open;

	// The map of nodes already evaluated
	// Map : know immediately if a node is already evaluated or not
	int rows = state.rows;
	int cols = state.cols;

	vector<vector<bool>> closed(rows, vector<bool>(cols));
	vector<vector<Node>> map(rows, vector<Node>(cols));

	// Initialize all nodes
	for (int x = 0; x < rows; x++)
	{
		for (int y = 0; y < cols; y++)
		{
			map[x][y].fCost = FLT_MAX;
			map[x][y].gCost = FLT_MAX;
			map[x][y].hCost = FLT_MAX;
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
			Node retraceNode = current;

			// Retrace path
			while (!(retraceNode.parentX == start.x && retraceNode.parentY == start.y))
			{
				retraceNode = map[retraceNode.parentX][retraceNode.parentY];
			}

			// Because we are calculating the path every frame, we only need the first node
			vector<Node> path;
			path.push_back(retraceNode);
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
			if (current.gCost + 1 < neighbour.gCost || neighbour.fCost == FLT_MAX)
			{
				open.erase(neighbour);
				neighbour.gCost = current.gCost + 1;
				neighbour.hCost = distance(state, neighbour, dest);
				neighbour.fCost = neighbour.gCost + neighbour.hCost;
				neighbour.parentX = current.x;
				neighbour.parentY = current.y;
				open.insert(neighbour);
			}
		}
	}

	cout << "Destination not found" << endl;
	return empty;
}


// ------------------------------------- Helper methods -------------------------------------

// Move an ant towards the destination
// This method makes the connection between locations and nodes
bool Bot::doMoveLocation(const Location &start, const Location &dest)
{
	Node startNode;
	startNode.x = start.row;
	startNode.y = start.col;

	Node destNode;
	destNode.x = dest.row;
	destNode.y = dest.col;

	// Get path with A-star
	vector<Node> path = aStar(state, startNode, destNode);
	if(path.empty()) return false;

	Location newLocation;
	newLocation.row = path[0].x;
	newLocation.col = path[0].y;

	// Move ant one square
    state.grid[newLocation.row][newLocation.col].ant = state.grid[start.row][start.col].ant;
    state.grid[start.row][start.col].ant = -1;

	// /!\ Check what this insert function is used for
	orders->insert({ newLocation, start });
	return true;
}