#ifndef ROUTE_H_
#define ROUTE_H_

#include "Location.h"

class Route
{
private :
	Location start;
	Location end;
	int distance;
public :
	Route(Location _start, Location _end, int _distance) : start(_start), end(_end), distance(_distance) {}

	Location getStart();
	Location getEnd();
	int getDistance();

	int compareTo(Route route);

	inline bool operator==(const Route& r) const { return (start == r.start && end == r.end); }
	inline bool operator!=(const Route& r) const { return (start != r.start || end != r.end); }
	inline bool operator<(const Route& r) const { return (distance < r.distance); }
};

#endif