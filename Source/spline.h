#pragma once
#include"mat_vec.h"
namespace NURBS {
    struct CP
    {
        MathN::vec3 pos;
        float w=1.0f;
        CP() {}
        CP(float x,float y,float z,float weight) {
            pos = MathN::vec3(x,y,z);
            w = weight;
        }
        CP(float x, float y, float z) {
            pos = MathN::vec3(x, y, z);
            w = 1.0f;
        }
    };
    enum SplineType
    {
        Standrad,
        Loop,
        Clamped
    };
    enum KnotType
    {
        Uniform,
        OpenUniform
    };
    std::vector<float> GenKnotVectors(int order, int cpsNum, KnotType type);
    float KnotVector(int j, int order, int cpsNum, KnotType type);
    float UniformKnotVector(int j, int knotNum);
    float OpenUniformKnotVector(int j, int order, int knotNum);
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
