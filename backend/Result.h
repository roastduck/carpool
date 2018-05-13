#ifndef RESULT_H_
#define RESULT_H_

#include <vector>

const int RESULT_NUM = 5;

struct Point
{
    double x, y;
};

typedef std::vector<Point> Path;

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

#endif // RESULT_H_

