#include "Route.h"

Location Route::getStart() 
{
	return start;
}
Location Route::getEnd()
{
	return end;
}
int Route::getDistance()
{
	return distance;
}

int Route::compareTo(Route route)
{
	return distance - route.distance;
}