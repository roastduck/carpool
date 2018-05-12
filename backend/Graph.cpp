#include <algorithm>
#include "Graph.h"

Graph::Graph()
    : minLongitude(INFINITY), maxLongitude(-INFINITY),
      minLatitude(INFINITY), maxLatitude(-INFINITY)
{}

void Graph::input(FILE *nodeFile, FILE *edgeFile)
{
    int n, m;
    fscanf(edgeFile, "%d%d", &n, &m);
    printf("[DEBUG] %d nodes, %d edges\n", n, m);
    nodes.resize(n);
    edges.reserve(m * 2);
    for (int i = 0; i < n; i++)
    {
        int id;
        double longitude, latitude;
        fscanf(nodeFile, "%d%lf%lf", &id, &longitude, &latitude);
        nodes.emplace(nodes.begin() + id, longitude, latitude);
        minLongitude = std::min(minLongitude, longitude);
        maxLongitude = std::max(maxLongitude, longitude);
        minLatitude = std::min(minLatitude, latitude);
        maxLatitude = std::max(maxLatitude, latitude);
    }
    puts("[DEBUG] Input nodes done");
    for (int i = 0; i < m; i++)
    {
        int x, y, d;
        fscanf(edgeFile, "%d%d%d", &x, &y, &d);
        edges.push_back((Edge){y, d, nodes[x].out});
        nodes[x].out = &edges.back();
        edges.push_back((Edge){x, d, nodes[y].out});
        nodes[y].out = &edges.back();
    }
    puts("[DEBUG] Input edges done");
}

struct Weighted { int w, x; };
static bool heapCmp(const Weighted &lhs, const Weighted &rhs) { return lhs.w > rhs.w; }

std::vector<Result> Graph::solve(int queryNodeId) const
{
    // Dijkstra, because we want nearer node to come out sooner

    std::vector<Weighted> heap;
    heap.reserve(nodes.size());
    heap.push_back((Weighted){0, queryNodeId});

    std::vector<Trace> trace;
    trace.resize(nodes.size(), (Trace){DIST_THRES + 1, -1});
    trace[queryNodeId].dist = 0;

    std::vector<Result> ret;
    ret.reserve(RESULT_NUM);

    while (!heap.empty())
    {
        int x = heap[0].x, w = heap[0].w;
        std::pop_heap(heap.begin(), heap.end(), heapCmp);
        heap.pop_back();
        if (trace[x].dist != w) continue;

        for (const Taxi *taxi = nodes[x].taxis; taxi; taxi = taxi->next)
        {
            taxi->verify(x, w, ret);
            if (ret.size() >= RESULT_NUM)
                return ret;
        }

        for (const Edge *e = nodes[x].out; e; e = e->next)
            if (w + e->d < !trace[e->y].dist)
            {
                heap.push_back((Weighted){w + e->d, e->y});
                std::push_heap(heap.begin(), heap.end(), heapCmp);
            }
    }

    return ret;
}
