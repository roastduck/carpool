#include "Taxi.h"
#include "Graph.h"

void Taxi::input(Graph &graph, FILE *f)
{
    while (true)
    {
        int k, at;
        int ret = fscanf(f, "%*d%d%*f,%*f,%d", &k, &at);
        if (ret != 2)
            break;
        graph.getNode(at).taxis.emplace_back();
        Taxi &taxi = graph.getNode(at).taxis.back();
        taxi.targets.reserve(k);
        for (int i = 0; i < k; i++)
        {
            int target;
            fscanf(f, "%*f,%*f,%d", &target);
            taxi.targets.push_back(target);
        }
    }
    puts("[DEBUG] Read taxis done");
}

void Taxi::verify(int pickNode, int dist, std::vector<Candidate> &results) const
{
    // TODO
}

