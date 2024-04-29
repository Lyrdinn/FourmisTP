#pragma once
#include "Location.h"

class Ant
{
public:
	int turnApparitionKey;
	Location currentLocation;
	Location futureLocation;
	bool isExplorer = false;
	Location exploGoal;

	Ant(int turnNum, Location _currentLocation) : turnApparitionKey(turnNum), currentLocation(_currentLocation) {
		isExplorer = false;
	}
};
