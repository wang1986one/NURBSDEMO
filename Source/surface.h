#pragma once
#include"nurbs_share.h"
namespace NURBS {
    namespace SurfaceUtil {
        float BasisFunc(int j, int k, int o, float t, int l, KnotType kt) {
            if (k == 0) {
                return (t >= KnotVector(j, o, l, kt) && t < KnotVector(j + 1, o, l, kt)) ? 1 : 0;
            }
            else {
                float d1 = KnotVector(j + k, o, l, kt) - KnotVector(j, o, l, kt);
                float d2 = KnotVector(j + k + 1, o, l, kt) - KnotVector(j + 1, o, l, kt);
                float c1 = d1 != 0 ? (t - KnotVector(j, o, l, kt)) / d1 : 0;
                float c2 = d2 != 0 ? (KnotVector(j + k + 1, o, l, kt) - t) / d2 : 0;
                return c1 * BasisFunc(j, k - 1, o, t, l, kt) + c2 * BasisFunc(j + 1, k - 1, o, t, l, kt);
            }
        }
        MathN::vec3 GetCurve(std::vector<CP> cps, float tx, float ty, int o, int lx, int ly, KnotType kx, KnotType ky) {
            MathN::vec3 f(0, 0, 0);
            float d = 1e-9f;
            for (int y = 0; y < ly; y++) {
                for (int x = 0; x < lx; x++) {
                    float b = BasisFunc(x, o, o, tx, lx, kx) * BasisFunc(y, o, o, ty, ly, ky);
                    CP c = cps[x + y * lx];
                    f += c.pos * b * c.w;
                    d += b * c.w;
                }
            }
            return f / d;
        }
    }
    class Surface {
        std::vector<CP> cps;
        MathN::vec2 min{ 0,0 };
        MathN::vec2 max { 1,1 };
        int order, lx, ly;
        bool close = false;
        bool xloop;
        bool yloop;
        KnotType xknotType;
        KnotType yknotType;
        float shiftx(float t) { return min[0] + (max[0] - min[0]) * t; }
        float shifty(float t) { return min[1] + (max[1] - min[1]) * t; }
        int idx(int x, int y) { return x + y * this->lx; }
    public:
        Surface(std::vector<CP> cps, int order, int lx, int ly, SplineType xtype, SplineType ytype) {
            this->order = order;
            this->xloop = xtype == SplineType::Loop;
            this->yloop = ytype == SplineType::Loop;
            this->xknotType = xtype == SplineType::Clamped ? KnotType::OpenUniform : KnotType::Uniform;
            this->yknotType = ytype == SplineType::Clamped ? KnotType::OpenUniform : KnotType::Uniform;
            if (close) {
                std::vector<CP> xlooped = GenXLoop(cps, lx, ly);
                MathN::vec3 center1(0, 0, 0);
                MathN::vec3 center2(0, 0, 0);
                for (int i = 0; i < lx; i++) center1 += cps[i + lx].pos;
                for (int i = 0; i < lx; i++) center2 += cps[i + (ly - 2) * lx].pos;
                center1 /= lx;
                center2 /= lx;
                for (int i = 0; i < lx; i++) cps[i].pos = center1;
                for (int i = 0; i < lx; i++) cps[i + (ly - 1) * lx].pos = center2;
            }
            if (xloop && yloop) {
                std::vector<CP> xlooped = GenXLoop(cps, lx, ly);
                std::vector<CP> ylooped = GenYLoop(xlooped, lx, ly);
                SetData(ylooped, lx + order, ly + order);
            }
            else if (yloop) {
                SetData(GenYLoop(cps, lx, ly), lx, ly + order);
            }
            else if (xloop) {
                SetData(GenXLoop(cps, lx, ly), lx + order, ly);
            }
            else { SetData(cps, lx, ly); }
            min = MathN::vec2( KnotVector(order, order, lx, xknotType) ,KnotVector(order, order, ly, yknotType) );
            max = MathN::vec2(KnotVector(lx, order, lx, xknotType), KnotVector(ly, order, ly, yknotType));
        }
        bool GetCurve(float normTx, float normTy, MathN::vec3& v) {
            float tx = shiftx(normTx);
            float ty = shifty(normTy);
            float fx = tx >= min[0] && tx <= max[0];
            float fy = ty >= min[1] && ty <= max[1];
            v = SurfaceUtil::GetCurve(cps, tx, ty, order, lx, ly, xknotType, yknotType);
            return fx && fy;

        }
        void SetCP(MathN::ivec2 i, CP cp) {
            if (close) { return; }
            bool fx = xloop && i[0] < order;
            bool fy = yloop && i[1] < order;
            cps[idx(i[0], i[1])] = cp;
            if (fx) cps[idx(lx - order + i[0], i[1])] = cp;
            if (fy) cps[idx(i[0], ly - order + i[1])] = cp;
            if (fx && fy) cps[idx(lx - order + i[0], ly - order + i[1])] = cp;
        }
        std::vector<CP> GenXLoop(std::vector<CP> cps, int lx, int ly) {
            std::vector<CP> arr;
            arr.resize((lx + order) * ly);
            for (int y = 0; y < ly; y++)
                for (int x = 0; x < lx + order; x++) {
                    int i1 = x % lx + y * lx;
                    int i2 = x + y * (lx + order);
                    arr[i2] = cps[i1];
                }
            return arr;
        }
        std::vector<CP> GenYLoop(std::vector<CP> cps, int lx, int ly) {
            std::vector<CP> lst = cps;
            for (int i = 0; i < order; i++) {
                std::vector<CP> row(lx+order);// = new CP[lx + order];
                for (int j = 0; j < lx + order; j++) {
                    row[j] = cps[i * (lx + order) + j];
                }
                std::copy(row.begin(),row.end(),std::back_inserter(lst));
            }
            return lst;
        }
        void SetData(std::vector<CP> cps, int lx, int ly) {
            this->lx = lx;
            this->ly = ly;
            this->cps = cps;
        }
    };
}
