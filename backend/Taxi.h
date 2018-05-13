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
    static void input(Graph &graph, FILE *f);

    void verify(const Graph &graph, int pickNode, int dist, std::vector<Candidate> &candidates) const;
};

#endif // TAXI_H_

