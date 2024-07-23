#pragma once
#include"mat_vec.h"
#include"nurbs_share.h"
namespace NURBS {
   class Spline {
    public:
        std::vector<CP>cps;
        enum SplineType splineType;
        KnotType knotType;
        bool loop;
        int order;
        float min = 0.0;
        float max = 1.0f;
        std::vector<float> knots;
        float shift(float t) { return min + (max - min) * t; }
        Spline(const std::vector<CP>& cps, int order, enum SplineType t);
        void SetCP(int i, CP cp);
        bool GetCurve(float normT, MathN::vec3& v);
        float BasisFunc(int j, int k, float t);
        float BasisFancFirstDerivative(int i, int o, float t);
        float BasisFancSecondDerivative(int i, int o, float t);
    };


}
