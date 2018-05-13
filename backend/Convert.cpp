#include <cmath>
#include "Convert.h"

const double PI = acos(-1);

/* Krasovsky 1940
 *
 * a = 6378245.0, 1/f = 298.3
 * b = a * (1 - f)
 * ee = (a^2 - b^2) / a^2
 */
const double a = 6378245.0;
const double ee = 0.00669342162296594323;

double Convert::encodeLng(double x, double y)
{
	double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
	ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(x * PI) + 40.0 * sin(x / 3.0 * PI)) * 2.0 / 3.0;
	ret += (150.0 * sin(x / 12.0 * PI) + 300.0 * sin(x / 30.0 * PI)) * 2.0 / 3.0;
	return ret;
}

double Convert::encodeLat(double x, double y)
{
	double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
	ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(y * PI) + 40.0 * sin(y / 3.0 * PI)) * 2.0 / 3.0;
	ret += (160.0 * sin(y / 12.0 * PI) + 320 * sin(y * PI / 30.0)) * 2.0 / 3.0;
	return ret;
}

void Convert::encode(double wgLng, double wgLat, double &gcjLng, double &gcjLat)
{
	double dLat = encodeLat(wgLng - 105.0, wgLat - 35.0);
	double dLng = encodeLng(wgLng - 105.0, wgLat - 35.0);
	double radLat = wgLat / 180.0 * PI;
	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * PI);
	dLng = (dLng * 180.0) / (a / sqrtMagic * cos(radLat) * PI);
    gcjLat = wgLat + dLat;
	gcjLng = wgLng + dLng;
}

void Convert::decode(double gcjLng, double gcjLat, double &wgLng, double &wgLat)
{
    // encode(p) ~ p
    // encode(p) - encode(encode(p)) ~ p - encode(p)
    // p = 2 encode(p) - encode(encode(p))
    double encLng, encLat;
    Convert::encode(gcjLng, gcjLat, encLng, encLat);
    wgLng = 2 * gcjLng - encLng;
    wgLat = 2 * gcjLat - encLat;
}

