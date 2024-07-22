#pragma once
#include"mat_vec.h"

namespace MathN
{

class  Ray
{
public:
    /// Construct a degenerate ray with zero origin and direction.
    Ray() noexcept = default;

    /// Construct from origin and direction. The direction will be normalized.
    Ray(const vec3& origin, const vec3& direction) noexcept
    {
        Define(origin, direction);
    }

    /// Copy-construct from another ray.
    Ray(const Ray& ray) noexcept = default;

    /// Assign from another ray.
    Ray& operator =(const Ray& rhs) noexcept = default;

    /// Check for equality with another ray.
    bool operator ==(const Ray& rhs) const { return origin_ == rhs.origin_ && direction_ == rhs.direction_; }

    /// Check for inequality with another ray.
    bool operator !=(const Ray& rhs) const { return origin_ != rhs.origin_ || direction_ != rhs.direction_; }

    /// Define from origin and direction. The direction will be normalized.
    void Define(const vec3& origin, const vec3& direction)
    {
        origin_ = origin;
        direction_ = direction;
        direction_.normalize();
    }

    /// Project a point on the ray.
    vec3 Project(const vec3& point) const
    {
        vec3 offset = point - origin_;
        return origin_ + dot(offset, direction_) * direction_;
    }

    /// Return distance of a point from the ray.
    float Distance(const vec3& point) const
    {
        vec3 projected = Project(point);
        return norm(point - projected);
    }

    /// Return closest point to another ray.
    vec3 ClosestPoint(const Ray& ray) const;


    bool HitDistance(const vec3& center,const float& radius,float& dis) const;


    /// Ray origin.
    vec3 origin_;
    /// Ray direction.
    vec3 direction_;
};

}
