#ifndef GEOMETRICOBJH
#define GEOMETRICOBJH

#include "ray.h"

class geometricObj
{
public:
    geometricObj(vec3 Color, double Kd, double Ks, double Ka, double Kr, double Kt, double N) : color(Color), kd(Kd), ks(Ks), ka(Ka), kr(Kr), kt(Kt), n(N) {}

    virtual pair<double, vec3> intersect(const ray &r) {}
    virtual vec3 getColor(vector<vector<vec3>> &texture, int resx, int resy) {}

    vec3 color;
    double kd;
    double ks;
    double ka;
    double kr;
    double kt;
    double n;

};

#endif
