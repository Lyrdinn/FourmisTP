#pragma once
#include "Location.h"

class Ant
{
public:
	Location currentLocation;
	Location futureLocation;
	bool isExploring = false;

	Ant(Location _currentLocation) : currentLocation(_currentLocation) {
		isExploring = false;
	}
};
