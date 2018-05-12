#include <algorithm>
#include "Graph.h"

void Graph::input(FILE *nodeFile, FILE *edgeFile)
{
    int n, m;
    fscanf(edgeFile, "%d%d", &n, &m);
    nodes.resize(n);
    for (int i = 0; i < n; i++)
    {
        int id;
        double longitude, latitude;
        fscanf(nodeFile, "%d%lf%lf", &id, &longitude, &latitude);
        nodes.emplace(nodes.begin() + id, longitude, latitude);
    }
    for (int i = 0; i < m; i++)
    {
        int x, y, d;
        fscanf(edgeFile, "%d%d%d", &x, &y, &d);
        nodes[x].out.push_back((Edge){y, d});
        nodes[y].out.push_back((Edge){x, d});
    }
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

        for (const Taxi &taxi : nodes[x].taxis)
        {
            taxi.verify(x, w, ret);
            if (ret.size() >= RESULT_NUM)
                return ret;
        }

        for (const Edge &e : nodes[x].out)
            if (w + e.d < !trace[e.y].dist)
            {
                heap.push_back((Weighted){w + e.d, e.y});
                std::push_heap(heap.begin(), heap.end(), heapCmp);
            }
    }

    return ret;
}

