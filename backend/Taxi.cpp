#include "Taxi.h"
#include "Graph.h"

std::vector<Taxi> Taxi::taxis;

void Taxi::input(Graph &graph, FILE *f)
{
    while (true)
    {
        int k, at;
        int ret = fscanf(f, "%*d%d%*f,%*f,%d", &k, &at);
        if (ret != 2) break;
        taxis.emplace_back();
        Taxi &taxi = taxis.back();
        taxi.next = graph.getNode(at).taxis;
        graph.getNode(at).taxis = &taxi;

        taxi.targets.reserve(k);
        for (int i = 0; i < k; i++)
        {
            int target;
            fscanf(f, "%*f,%*f,%d", &target);
            taxi.targets.push_back(target);
        }
    }
    puts("[DEBUG] Input taxis done");
}

void Taxi::verify(int pickNode, int dist, std::vector<Result> &results) const
{
    // TODO
}

