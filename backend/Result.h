#ifndef RESULT_H_
#define RESULT_H_

#include <vector>

const int RESULT_NUM = 5;

struct Point
{
    double x, y;
};

struct Path
{
    int dist;
    std::vector<Point> pts;
};

struct Candidate
{
    Point taxi;
    std::vector<Point> targets;
    Path oldPath, newPath;
};

struct Result
{
    Point depart, dest;
    std::vector<Candidate> candidates;
};

inline bool operator==(const Point &lhs, const Point &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Point &lhs, const Point &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

#endif // RESULT_H_

