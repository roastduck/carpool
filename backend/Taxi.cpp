#include <algorithm>
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
        taxi.at = at;
        taxi.targets.reserve(k);
        for (int i = 0; i < k; i++)
        {
            int target;
            fscanf(f, "%*f,%*f,%d", &target);
            taxi.targets.push_back(target);
        }
        if (k >= 4)
            graph.getNode(at).taxis.pop_back();
    }
    puts("[DEBUG] Read taxis done");
}

Path Taxi::findShortest(const std::vector<Path> &begin, const std::vector<std::vector<Path>> &paths) const
{
    Path ret;
    ret.dist = 0x7fffffff;
    const int k = begin.size();
    std::vector<int> permu(k);
    for (int i = 0; i < k; i++)
        permu[i] = i;
    do
    {
        int _dist = begin[permu[0]].dist;
        for (int i = 1; i < k; i++)
        {
            int x = std::max(permu[i - 1], permu[i]);
            int y = std::min(permu[i - 1], permu[i]);
            _dist += paths[x][y].dist;
        }
        if (_dist < ret.dist)
        {
            ret.dist = _dist;
            ret.pts = begin[permu[0]].pts;
            for (int i = 1; i < k; i++)
            {
                int x = std::max(permu[i - 1], permu[i]);
                int y = std::min(permu[i - 1], permu[i]);
                if (permu[i - 1] > permu[i])
                {
                    for (auto i = paths[x][y].pts.begin(); i != paths[x][y].pts.end(); i++)
                        if (*i != ret.pts.back())
                            ret.pts.push_back(*i);
                } else
                {
                    for (auto i = paths[x][y].pts.rbegin(); i != paths[x][y].pts.rend(); i++)
                        if (*i != ret.pts.back())
                            ret.pts.push_back(*i);
                }
            }
        }
    } while (std::next_permutation(permu.begin(), permu.end()));
    return ret;
}

std::unique_ptr<Candidate> Taxi::verify(const Graph &graph, int pickSt, int pickEn, int dist) const
{
    const int k = targets.size();
    std::vector<Path> oldBegin, newBegin;
    std::vector<std::vector<Path>> paths;
    for (int i = 0; i < k; i++)
        oldBegin.push_back(graph.path(at, targets[i]));
    for (int i = 0; i <= k; i++)
        newBegin.push_back(graph.path(pickSt, i < k ? targets[i] : pickEn));
    for (int i = 0; i <= k; i++)
    {
        paths.emplace_back();
        for (int j = 0; j < i; j++)
            paths.back().push_back(graph.path(i < k ? targets[i] : pickEn, targets[j]));
    }

    std::unique_ptr<Candidate> can(new Candidate());
    can->taxi = Point(graph.getNode(at).lng, graph.getNode(at).lat);
    can->targets.reserve(targets.size());
    for (int t : targets)
        can->targets.push_back(Point(graph.getNode(t).lng, graph.getNode(t).lat));
    if (k > 0)
        can->oldPath = findShortest(oldBegin, paths);
    can->newPath = findShortest(newBegin, paths);

    // TODO: Verify restrictions

    return can;
}

