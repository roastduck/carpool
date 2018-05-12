#ifndef TAXI_H_
#define TAXI_H_

#include <cstdio>
#include <vector>
#include "Result.h"

class Graph;

class Taxi
{
private:
    std::vector<int> targets;

public:
    Taxi *next;

    Taxi() : next(NULL) {}
    void verify(int pickNode, int dist, std::vector<Result> &results) const;

    static std::vector<Taxi> taxis;

    static void input(Graph &graph, FILE *f);
};

#endif // TAXI_H_

