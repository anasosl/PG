#ifndef PLANEH
#define PLANEH
#include "geometricObj.h"

class plane : public geometricObj
{
public:
    plane(const point3 &planepoint, const vec3& normal, 
    const vec3 &color, double Kd, double Ks, double Ka, double Kr, double Kt, double N)
        : geometricObj(color, Kd, Ks, Ka, Kr, Kt, N), Normal(normal), PP(planepoint) {}

    pair<double, vec3> intersect(const ray &r)
    {
        double t;

        vec3 pp = vec3(PP.x(), PP.y(), PP.z());
        vec3 og = vec3(r.origin().x(), r.origin().y(), r.origin().z());

        double a = dot(Normal, pp);
        double b = dot(Normal, og);
        double c = dot(Normal, r.direction());

        if (c != 0) {
            t = (a - b) / c;
        }
            
    
        return {t, intNormal(r,t)};
    }

    vec3 intNormal(const ray &r, double t){
        if (dot(r.direction(), Normal) > 0) {
            return vec3(-Normal.x(), -Normal.y(), -Normal.z());
        } else return Normal;
    }

    vec3 getColor(vector<vector<vec3>> &texture, int resx, int resy) {
        return color;
    }

    point3 PP;
    vec3 Normal;
};

#endif  
