#ifndef CONVERT_H_
#define CONVERT_H_

// Coordinate convertion
class Convert
{
private:
    static double encodeLng(double x, double y);
    static double encodeLat(double x, double y);

public:
    /// True -> GCJ
    static void encode(double wgLng, double wgLat, double &gcjLng, double &gcjLat);

    /// GCJ -> True
    static void decode(double gcjLng, double gcjLat, double &wgLng, double &wgLat);
};

#endif // CONVERT_H_

