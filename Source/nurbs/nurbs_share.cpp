#pragma once
#include"nurbs_share.h"
namespace NURBS {
    std::vector<float> GenKnotVectors(int order, int cpsNum, KnotType type) {
        int knotNum = cpsNum + order + 1;
        std::vector<float> knots(knotNum);
        for (int i = 0; i < knotNum; i++) knots[i] = KnotVector(i, order, cpsNum, type);
        return knots;
    }
    float KnotVector(int j, int order, int cpsNum, KnotType type) {
        int knotNum = cpsNum + order + 1;
        if (type == KnotType::Uniform)     return UniformKnotVector(j, knotNum);
        return OpenUniformKnotVector(j, order, knotNum);
    }
    float UniformKnotVector(int j, int knotNum) {
        float t0 = 0.0f;
        float t1 = 1.0f;
        return t0 + (t1 - t0) / (knotNum - 1) * j;
    }
    float OpenUniformKnotVector(int j, int order, int knotNum) {
        if (j <= order) return 0.0f;
        if (j >= knotNum - 1 - order) return 1.0f;
        return (float)j / (knotNum - order + 1);
    }

}
