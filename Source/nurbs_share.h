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
}
