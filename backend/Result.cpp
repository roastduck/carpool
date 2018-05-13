#include "Result.h"

/// GCJ -> BD
Point::Point(double _x, double _y)
{
    double _z = sqrt(_x * _x + _y * _y) + 0.00002 * sin(_y * X_PI);
    double theta = atan2(_y, _x) + 0.000003 * cos(_x * X_PI);
    x = _z * cos(theta) + 0.0065;
    y = _z * sin(theta) + 0.006;
}

