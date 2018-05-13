#include <cstdio>
#include <node.h>
#include "Taxi.h"
#include "Graph.h"
#include "Result.h"

const char *NODE_FILE = "data/road.cnode";
const char *EDGE_FILE = "data/road.nedge";
const char *TAXI_FILE = "data/car.txt";

#define F_NUM(name, callback) \
    void name(const FunctionCallbackInfo<Value> &args) \
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
    using v8::Exception;
    using v8::Array;

    Graph graph;

    void bdDecrypt(double bd_lon, double bd_lat, double &gg_lon, double &gg_lat)
    {
        double x = bd_lon - 0.0065, y = bd_lat - 0.006;
        double z = sqrt(x * x + y * y) - 0.00002 * sin(y * X_PI);
        double theta = atan2(y, x) - 0.000003 * cos(x * X_PI);
        gg_lon = z * cos(theta);
        gg_lat = z * sin(theta);
    }

    bool checkArgs(const FunctionCallbackInfo<Value> &args, int num)
    {
        Isolate* isolate = args.GetIsolate();
        if (args.Length() != num)
        {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "query: Wrong number of arguments")
            ));
            return false;
        }
        return true;
    }

    Local<Object> getPoint(Isolate *isolate, const Point &pt)
    {
        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, pt.x));
        obj->Set(String::NewFromUtf8(isolate, "y"), Number::New(isolate, pt.y));
        return obj;
    }

    Local<Array> getPoints(Isolate *isolate, const std::vector<Point> &pts)
    {
        Local<Array> arr = Array::New(isolate, pts.size());
        for (int i = 0; i < int(pts.size()); i++)
            arr->Set(i, getPoint(isolate, pts[i]));
        return arr;
    }

    Local<Object> getPath(Isolate *isolate, const Path &path)
    {
        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "dist"), Number::New(isolate, path.dist));
        obj->Set(String::NewFromUtf8(isolate, "pts"), getPoints(isolate, path.pts));
        return obj;
    }

    F_NUM(getMinLongitude, graph.getMinLongitude)
    F_NUM(getMaxLongitude, graph.getMaxLongitude)
    F_NUM(getMinLatitude, graph.getMinLatitude)
    F_NUM(getMaxLatitude, graph.getMaxLatitude)

    void query(const FunctionCallbackInfo<Value> &args)
    {
        if (!checkArgs(args, 4))
            return;
        Isolate* isolate = args.GetIsolate();

        double lngStBd = args[0]->NumberValue(), latStBd = args[1]->NumberValue();
        double lngEnBd = args[2]->NumberValue(), latEnBd = args[3]->NumberValue();
        double lngSt, latSt, lngEn, latEn;
        bdDecrypt(lngStBd, latStBd, lngSt, latSt);
        bdDecrypt(lngEnBd, latEnBd, lngEn, latEn);
        printf("[DEBUG] Query GCJ coordinate (%f, %f) -> (%f, %f)\n", lngSt, latSt, lngEn, latEn);

        Result res = graph.solve(lngSt, latSt, lngEn, latEn);
        puts("[DEBUG] Query returns");

        Local<Object> ret = Object::New(isolate);
        ret->Set(String::NewFromUtf8(isolate, "depart"), getPoint(isolate, res.depart));
        ret->Set(String::NewFromUtf8(isolate, "dest"), getPoint(isolate, res.dest));
        Local<Array> candidates = Array::New(isolate, res.candidates.size());
        for (int i = 0; i < int(res.candidates.size()); i++)
        {
            Local<Object> can = Object::New(isolate);
            can->Set(String::NewFromUtf8(isolate, "taxi"), getPoint(isolate, res.candidates[i].taxi));
            can->Set(String::NewFromUtf8(isolate, "targets"), getPoints(isolate, res.candidates[i].targets));
            can->Set(String::NewFromUtf8(isolate, "oldPath"), getPath(isolate, res.candidates[i].oldPath));
            can->Set(String::NewFromUtf8(isolate, "newPath"), getPath(isolate, res.candidates[i].newPath));
            candidates->Set(i, can);
        }
        ret->Set(String::NewFromUtf8(isolate, "candidates"), candidates);

        args.GetReturnValue().Set(ret);
    }

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
        NODE_SET_METHOD(exports, "query", query);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}
