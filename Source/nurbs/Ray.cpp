#include"ray.h"


namespace MathN
{

vec3 Ray::ClosestPoint(const Ray& ray) const
{
    // Algorithm based on http://paulbourke.net/geometry/lineline3d/
    vec3 p13 = origin_ - ray.origin_;
    vec3 p43 = ray.direction_;
    vec3 p21 = direction_;

    float d1343 = dot(p13,p43);
    float d4321 = dot(p43,p21);
    float d1321 = dot(p13,p21); 
    float d4343 = dot(p43,p43);
    float d2121 = dot(p21,p21); 

    float d = d2121 * d4343 - d4321 * d4321;
    if (abs(d) < 0.000001f)
        return origin_;
    float n = d1343 * d4321 - d1321 * d4343;
    float a = n / d;

    return origin_ + a * direction_;
}

bool Ray::HitDistance(const vec3& center, const float& radius,float& dis) const
{
    vec3 centeredOrigin = origin_ - center;
    float squaredRadius = radius * radius;

    // Check if ray originates inside the sphere
    if (sqrnorm(centeredOrigin) <= squaredRadius)
        return false;

    // Calculate intersection by quadratic equation
    float a = dot(direction_,direction_);
    float b = 2.0f * dot(centeredOrigin,direction_);
    float c = dot(centeredOrigin,centeredOrigin) - squaredRadius;
    float d = b * b - 4.0f * a * c;

    // No solution
    if (d < 0.0f)
        return false;

    // Get the nearer solution
    float dSqrt = sqrtf(d);
    float dist = (-b - dSqrt) / (2.0f * a);
    if (dist >= 0.0f)
        dis= dist;
    else
        dis= (-b + dSqrt) / (2.0f * a);

    return true;
}
}
