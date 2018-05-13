#include <cassert>
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

Path Graph::path(int stId, int enId) const
{
    // A* Dijkstra

    std::vector<Weighted> heap;
    heap.reserve(nodes.size());
    heap.push_back((Weighted){(int)ceil(directDist(nodes[enId], nodes[stId])), enId});

    std::vector<Trace> trace;
    trace.resize(nodes.size(), (Trace){0x7fffffff, -1});
    trace[enId].dist = 0;

    while (!heap.empty())
    {
        int x = heap[0].x, w = trace[x].dist, _w = heap[0].w;
        std::pop_heap(heap.begin(), heap.end(), heapCmp);
        heap.pop_back();
        if (trace[x].dist + (int)ceil(directDist(nodes[x], nodes[stId])) != _w)
            continue;

        if (x == stId)
            return collectPath(trace, x);

        for (const Edge &e : nodes[x].out)
            if (w + e.d < trace[e.y].dist)
            {
                trace[e.y] = (Trace){w + e.d, x};
                heap.push_back((Weighted){w + e.d + (int)ceil(directDist(nodes[e.y], nodes[stId])), e.y});
                std::push_heap(heap.begin(), heap.end(), heapCmp);
            }
    }
    assert(false);
}

Result Graph::solve(int stId, int enId) const
{
    printf("[DEBUG] Query node %d (%f, %f)\n", stId, nodes[stId].lng, nodes[stId].lat);

    // Dijkstra, because we want nearer nodes to come out sooner

    std::vector<Weighted> heap;
    heap.reserve(nodes.size());
    heap.push_back((Weighted){0, stId});

    std::vector<Trace> trace;
    trace.resize(nodes.size(), (Trace){DIST_THRES + 1, -1});
    trace[stId].dist = 0;

    Result ret;
    ret.depart = Point(nodes[stId].lng, nodes[stId].lat);
    ret.dest = Point(nodes[enId].lng, nodes[enId].lat);
    ret.candidates.reserve(RESULT_NUM);

    int aloneDist = path(stId, enId).dist;

    while (!heap.empty())
    {
        int x = heap[0].x, w = heap[0].w;
        std::pop_heap(heap.begin(), heap.end(), heapCmp);
        heap.pop_back();
        if (trace[x].dist != w) continue;

        for (const Taxi &taxi : nodes[x].taxis)
        {
            auto can = taxi.verify(*this, stId, enId, w, aloneDist);
            if (can)
            {
                Path _newPath = collectPath(trace, x);
                _newPath.dist += can->newPath.dist;
                for (const Point &p : can->newPath.pts)
                    if (_newPath.pts.back() != p)
                        _newPath.pts.push_back(p);
                can->newPath = std::move(_newPath);

                ret.candidates.push_back(std::move(*can));
                if (ret.candidates.size() >= RESULT_NUM)
                    return ret;
            }
        }

        for (const Edge &e : nodes[x].out)
            if (w + e.d < trace[e.y].dist)
            {
                trace[e.y] = (Trace){w + e.d, x};
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

Path Graph::collectPath(const std::vector<Trace> &trace, int dest) const
{
    Path path;
    path.dist = trace[dest].dist;
    for (int x = dest; x != -1; x = trace[x].last)
        path.pts.push_back(Point(nodes[x].lng, nodes[x].lat));
    return path;
}

