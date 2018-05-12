#include <cstdio>
#include <node.h>
#include "Taxi.h"
#include "Graph.h"

const char *NODE_FILE = "data/road.cnode";
const char *EDGE_FILE = "data/road.nedge";
const char *TAXI_FILE = "data/car.txt";

#define F_NUM(name, callback) \
    void name(const FunctionCallbackInfo<Value>& args) \
    { \
        Isolate* isolate = args.GetIsolate(); \
        args.GetReturnValue().Set(Number::New(isolate, (callback)())); \
    }

namespace ex
{
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Number;

    Graph graph;

    F_NUM(getMinLongitude, graph.getMinLongitude)
    F_NUM(getMaxLongitude, graph.getMaxLongitude)
    F_NUM(getMinLatitude, graph.getMinLatitude)
    F_NUM(getMaxLatitude, graph.getMaxLatitude)

    void init(Local<Object> exports)
    {
        FILE *nodeFile = fopen(NODE_FILE, "r");
        FILE *edgeFile = fopen(EDGE_FILE, "r");
        FILE *taxiFile = fopen(TAXI_FILE, "r");
        char *nodeBuff = new char[1024 * 1024];
        char *edgeBuff = new char[1024 * 1024];
        char *taxiBuff = new char[1024 * 1024];
        setvbuf(nodeFile, nodeBuff, _IOFBF, 1024 * 1024);
        setvbuf(edgeFile, edgeBuff, _IOFBF, 1024 * 1024);
        setvbuf(taxiFile, taxiBuff, _IOFBF, 1024 * 1024);
        if (!nodeFile || !edgeFile || !taxiFile)
        {
            puts("Unable to open data file");
            return;
        }
        graph.input(nodeFile, edgeFile);
        Taxi::input(graph, taxiFile);
        fclose(nodeFile);
        fclose(edgeFile);
        fclose(taxiFile);
        delete[] nodeBuff;
        delete[] edgeBuff;
        delete[] taxiBuff;

        NODE_SET_METHOD(exports, "getMinLongitude", getMinLongitude);
        NODE_SET_METHOD(exports, "getMaxLongitude", getMaxLongitude);
        NODE_SET_METHOD(exports, "getMinLatitude", getMinLatitude);
        NODE_SET_METHOD(exports, "getMaxLatitude", getMaxLatitude);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}
