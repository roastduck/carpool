#include <algorithm>
#include "Graph.h"

Graph::Graph()
    : minLongitude(INFINITY), maxLongitude(-INFINITY),
      minLatitude(INFINITY), maxLatitude(-INFINITY)
{}

int Graph::nearestNode(double lng, double lat) const
{
    // Slow implementation: Can be optimized using a tree
    Node qry(lng, lat);
    int id = -1;
    double dist = INFINITY;
    for (int i = 0; i < int(nodes.size()); i++)
    {
        double _dist = directDist(nodes[i], qry);
        if (_dist < dist)
            id = i, dist = _dist;
    }
    return id;
}

void Graph::input(FILE *nodeFile, FILE *edgeFile)
{
    int n, m;
    fscanf(edgeFile, "%d%d", &n, &m);
    printf("[DEBUG] %d nodes, %d edges\n", n, m);
    nodes.resize(n);
    for (int i = 0; i < n; i++)
    {
        int id;
        double longitude, latitude;
        fscanf(nodeFile, "%d%lf%lf", &id, &longitude, &latitude);
        nodes[id] = Node(longitude, latitude);
        minLongitude = std::min(minLongitude, longitude);
        maxLongitude = std::max(maxLongitude, longitude);
        minLatitude = std::min(minLatitude, latitude);
        maxLatitude = std::max(maxLatitude, latitude);
    }
    puts("[DEBUG] Read nodes done");
    for (int i = 0; i < m; i++)
    {
        int x, y, d;
        fscanf(edgeFile, "%d%d%d", &x, &y, &d);
        nodes[x].out.push_back((Edge){y, d});
        nodes[y].out.push_back((Edge){x, d});
    }
    puts("[DEBUG] Read edges done");
}

struct Weighted { int w, x; };
static bool heapCmp(const Weighted &lhs, const Weighted &rhs) { return lhs.w > rhs.w; }

Result Graph::solve(int stId, int enId) const
{
    printf("[DEBUG] Query node %d (%f, %f)\n", stId, nodes[stId].lng, nodes[stId].lat);

    // Dijkstra, because we want nearer node to come out sooner

    std::vector<Weighted> heap;
    heap.reserve(nodes.size());
    heap.push_back((Weighted){0, stId});

    std::vector<Trace> trace;
    trace.resize(nodes.size(), (Trace){DIST_THRES + 1, -1});
    trace[stId].dist = 0;

    Result ret;
    ret.depart = (Point){nodes[stId].lng, nodes[stId].lat};
    ret.dest = (Point){nodes[enId].lng, nodes[enId].lat};
    ret.candidates.reserve(RESULT_NUM);

    while (!heap.empty())
    {
        int x = heap[0].x, w = heap[0].w;
        std::pop_heap(heap.begin(), heap.end(), heapCmp);
        heap.pop_back();
        if (trace[x].dist != w) continue;

        for (const Taxi &taxi : nodes[x].taxis)
        {
            taxi.verify(x, w, ret.candidates);
            if (ret.candidates.size() >= RESULT_NUM)
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

Result Graph::solve(double lngSt, double latSt, double lngEn, double latEn) const
{
    return solve(nearestNode(lngSt, latSt), nearestNode(lngEn, latEn));
}

