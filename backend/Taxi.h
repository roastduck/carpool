#ifndef TAXI_H_
#define TAXI_H_

#include <cstdio>
#include <vector>
#include <memory>
#include "Result.h"

class Graph;

class Taxi
{
private:
    int at;
    std::vector<int> targets;

    Path findShortest(const std::vector<Path> &begin, const std::vector<std::vector<Path>> &paths) const;

public:
    static void input(Graph &graph, FILE *f);

    std::unique_ptr<Candidate> verify(const Graph &graph, int pickSt, int pickEn, int dist) const;
};

#endif // TAXI_H_

