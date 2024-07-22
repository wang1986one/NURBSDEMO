#include"spline.h"
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
    Spline::Spline(const std::vector<CP>& cps, int order, enum SplineType t) {
        this->order = order;
        this->loop = t == SplineType::Loop;
        this->knotType = t == SplineType::Clamped ? KnotType::OpenUniform : KnotType::Uniform;

        if (loop) {
            this->cps.resize(cps.size() + order);

            std::copy(cps.begin(), cps.end(), this->cps.begin());
            for (int i = 0; i < order; i++) this->cps[this->cps.size() - order + i] = cps[i];
        }
        else {
            this->cps = cps;
        }
        this->knots = GenKnotVectors(order, this->cps.size(), knotType);
    }
    void Spline::SetCP(int i, CP cp) {
        cps[i] = cp;
        if (loop && i < order) cps[cps.size() - order + i] = cp;

    }
    bool Spline::GetCurve(float normT, MathN::vec3& v) {
        float t = shift(normT);
        MathN::vec3 f(0, 0, 0);
        float d = 0.0f;
        for (int i = 0; i < cps.size(); i++) {
            float b = BasisFunc(i, order, t);
            CP c = cps[i];
            f += c.pos * b * c.w;
            d += b * c.w;
        }
        v = f / (d != 0 ? d : 1e-9f);
        return t >= min && t <= max;
    }
    float Spline::BasisFunc(int j, int k, float t) {
        if (k == 0) { return (t >= knots[j] && t < knots[j + 1]) ? 1.0f : 0.0f; }
        else {
            float d1 = knots[j + k] - knots[j];
            float d2 = knots[j + k + 1] - knots[j + 1];
            float c1 = d1 != 0 ? (t - knots[j]) / d1 : 0;
            float c2 = d2 != 0 ? (knots[j + k + 1] - t) / d2 : 0;
            return c1 * BasisFunc(j, k - 1, t) + c2 * BasisFunc(j + 1, k - 1, t);
        }
    }
    float Spline::BasisFancFirstDerivative(int i, int o, float t) {
        float i1 = BasisFunc(i + 1, o - 1, t) / (knots[i + o] - knots[i + 1]);
        float i2 = BasisFunc(i, o - 1, t) / (knots[i + o - 1] - knots[i]);
        return (o - 1) * (-i1 + i2);
    }
    float Spline::BasisFancSecondDerivative(int i, int o, float t) {
        float i1 = BasisFancFirstDerivative(i + 1, o - 1, t) / (knots[i + o] - knots[i + 1]);
        float i2 = BasisFancFirstDerivative(i, o - 1, t) / (knots[i + o - 1] - knots[i]);
        return (o - 1) * (-i1 + i2);
    }
}
