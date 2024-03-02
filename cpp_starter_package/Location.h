#ifndef LOCATION_H_
#define LOCATION_H_

/*
    struct for representing locations in the grid.
*/
struct Location
{
    int row, col;

    Location()
    {
        row = col = 0;
    };

    Location(int r, int c)
    {
        row = r;
        col = c;
    };

    //redefining the operators so that we can compare and sort easily
    inline bool operator==(const Location& l) const { return (row == l.row &&col == l.col); }
    inline bool operator!=(const Location& l) const { return (row != l.row ||col != l.col); }
    inline bool operator<(const Location& l) const { return row < l.row || (l.row >= row && col < l.col);}
};

#endif //LOCATION_H_
