#ifndef BUG_H_
#define BUG_H_

#include <fstream>

#ifndef DEBUG
//#define DEBUG
#endif

/*
    struct for debugging - this is gross but can be used pretty much like an ofstream,
                           except the debug messages are stripped while compiling if
                           DEBUG is not defined.
    example:
        Bug bug;
        bug.open("./debug.txt");
        bug << state << endl;
        bug << "testing" << 2.0 << '%' << endl;
        bug.close();
*/
struct Bug
{
    std::ofstream _file;

    Bug()
    {

    };

    //opens the specified file
    inline void open(const std::string& filename)
    {
#ifdef DEBUG
        _file.open(filename.c_str());
#endif
    };

    //closes the ofstream
    inline void close()
    {
#ifdef DEBUG
        _file.close();
#endif
    };
};

//output function for endl
inline Bug& operator<<(Bug& _bug, std::ostream& (*manipulator)(std::ostream&))
{
#ifdef DEBUG
    _bug._file << manipulator;
#endif

    return _bug;
};

//output function
template <class T>
inline Bug& operator<<(Bug& _bug, const T& t)
{
#ifdef DEBUG
    _bug._file << t;
#endif

    return _bug;
};

#endif //BUG_H_