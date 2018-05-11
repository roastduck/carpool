#include "Taxi.h"
#include "Graph.h"

void Taxi::input(Graph &graph, FILE *f)
{
    while (true)
    {
        int k, at;
        fscanf(f, "%*d%d%*lf,%*lf,%d", &k, &at);
        graph.getNode(at).taxis.emplace_back();
        Taxi &taxi = graph.getNode(at).taxis.back();
        taxi.targets.reserve(k);
        for (int i = 0; i < k; i++)
        {
            int target;
            fscanf(f, "%*lf,%*lf,%d", &target);
            taxi.targets.push_back(target);
        }
    }
}

void Taxi::verify(int pickNode, int dist, std::vector<Result> &results) const
{
    // TODO
}

