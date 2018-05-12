#ifndef GRAPH_H_
#define GRAPH_H_

#include <cmath>
#include <cstdio>
#include <vector>
#include "Taxi.h"
#include "Result.h"

const double PI = acos(-1);
const double EARTH_RADIUS = 63710000;
const int DIST_THRES = 10000;

/// Road map
class Graph
{
private:
    struct Edge
    {
        int y; /// Node ID
        int d; /// Distance
    };

    struct Node
    {
        double x, y, z; /// Coordinate in the unit ball
        std::vector<Edge> out; /// Outcoming edges
        std::vector<Taxi> taxis;

        Node() {}
        Node(double longitude, double latitude);
    };

    struct Trace
    {
        int dist, last;
    };

    std::vector<Node> nodes;

    static double directDist(const Node &a, const Node &b);

public:
    Node &getNode(int id);
    void input(FILE *nodeFile, FILE *edgeFile);
    std::vector<Result> solve(int queryNodeId) const;
};

inline Graph::Node::Node(double longitude, double latitude)
{
    double phi = longitude / 180 * PI;
    double theta = (90 - latitude) / 180 * PI;
    x = sin(theta) * cos(phi);
    y = sin(theta) * sin(phi);
    z = cos(phi);
}

inline double Graph::directDist(const Node &a, const Node &b)
{
    return acos(a.x * b.x + a.y * b.y + a.z * b.z) * EARTH_RADIUS;
}

inline Graph::Node &Graph::getNode(int id)
{
    return nodes[id];
}

#endif // GRAPH_H_
