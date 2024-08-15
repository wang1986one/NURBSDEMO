#pragma once
#include "nous_mesh/mesh.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "nous_nurbs_curve.h"
#include "nous_nurbs_util.h"
namespace nous
{
namespace nurbs
{
namespace util
{

/**
 * Evaluate point on a nonrational NURBS curve
 * @param[in] degree Degree of the given curve.
 * @param[in] knots Knot vector of the curve.
 * @param[in] control_points Control points of the curve.
 * @param[in] u Parameter to evaluate the curve at.
 * @return point Resulting point on the curve at parameter u.
 */

inline pos_t curve_point(size_t degree, const std::vector<float>& knots, const std::vector<pos_t>& control_points, float u)
{
    // Initialize result to 0s
    pos_t point = pos_t::Zero();

    // Find span and corresponding non-zero basis functions
    int span = find_span(degree, knots, u);
    std::vector<float> n = bspline_basis(degree, span, knots, u);

    // Compute point
    for(size_t j = 0; j <= degree; j++)
    {
        point += static_cast<float>(n[j]) * control_points[span - degree + j];
    }
    return point;
}
inline vec4 curve_point(size_t degree, const std::vector<float>& knots, const std::vector<vec4>& control_points, float u)
{
    // Initialize result to 0s
    vec4 point = vec4::Zero();

    // Find span and corresponding non-zero basis functions
    int span = find_span(degree, knots, u);
    std::vector<float> n = bspline_basis(degree, span, knots, u);

    // Compute point
    for(size_t j = 0; j <= degree; j++)
    {
        point += static_cast<float>(n[j]) * control_points[span - degree + j];
    }
    return point;
}
inline vec3 curve_point(const RationalCurve& crv, float u)
{

    // Compute homogenous coordinates of control points
    std::vector<vec4> cw;
    cw.reserve(crv.m_control_points.size());
    for(size_t i = 0; i < crv.m_control_points.size(); i++)
    {
        cw.push_back(vec4(crv.m_control_points[i].x() * crv.m_weights[i], crv.m_control_points[i].y() * crv.m_weights[i],
                                crv.m_control_points[i].z() * crv.m_weights[i], crv.m_weights[i]));
    }

    // Compute point using homogenous coordinates
    vec4 pointw = curve_point(crv.m_degree, crv.m_knots, cw, u);

    // Convert back to cartesian coordinates
    return vec3(pointw.x() / pointw.w(), pointw.y() / pointw.w(), pointw.z() / pointw.w());
}

/**
 * Evaluate derivatives of a non-rational NURBS curve
 * @param[in] degree Degree of the curve
 * @param[in] knots Knot vector of the curve.
 * @param[in] control_points Control points of the curve.
 * @param[in] num_ders Number of times to derivate.
 * @param[in] u Parameter to evaluate the derivatives at.
 * @return curve_ders Derivatives of the curve at u.
 * E.g. curve_ders[n] is the nth derivative at u, where 0 <= n <= num_ders.
 */

inline std::vector<vec3> curve_derivatives(size_t degree, const std::vector<float>& knots, const std::vector<vec3>& control_points,
                                                 int num_ders, float u)
{
    std::vector<vec3> curve_ders;
    curve_ders.resize(num_ders + 1);

    // Assign higher order derivatives to zero
    for(int k = degree + 1; k <= num_ders; k++)
    {
        curve_ders[k] = vec3::Zero();
    }

    // Find the span and corresponding non-zero basis functions & derivatives
    int span = find_span(degree, knots, u);
    std::vector<std::vector<float>> ders = bspline_der_basis(degree, span, knots, u, num_ders);

    // Compute first num_ders derivatives
    int du = num_ders < static_cast<int>(degree) ? num_ders : static_cast<int>(degree);
    for(int k = 0; k <= du; k++)
    {
        curve_ders[k] = vec3::Zero();
        for(int j = 0; j <= static_cast<int>(degree); j++)
        {
            curve_ders[k] += ders[k][j] * control_points[span - degree + j];
        }
    }
    return curve_ders;
}
inline std::vector<vec4> curve_derivatives(size_t degree, const std::vector<float>& knots, const std::vector<vec4>& control_points,
                                                 int num_ders, float u)
{
    std::vector<vec4> curve_ders;
    curve_ders.resize(num_ders + 1);

    // Assign higher order derivatives to zero
    for(int k = degree + 1; k <= num_ders; k++)
    {
        curve_ders[k] = vec4::Zero();
    }

    // Find the span and corresponding non-zero basis functions & derivatives
    int span = find_span(degree, knots, u);
    std::vector<std::vector<float>> ders = bspline_der_basis(degree, span, knots, u, num_ders);

    // Compute first num_ders derivatives
    int du = num_ders < static_cast<int>(degree) ? num_ders : static_cast<int>(degree);
    for(int k = 0; k <= du; k++)
    {
        curve_ders[k] = vec4::Zero();
        for(int j = 0; j <= static_cast<int>(degree); j++)
        {
            curve_ders[k] += ders[k][j] * control_points[span - degree + j];
        }
    }
    return curve_ders;
}

/**
 * Evaluate derivatives of a non-rational NURBS curve
 * @param[in] crv Curve object
 * @param[in] num_ders Number of times to derivate.
 * @param[in] u Parameter to evaluate the derivatives at.
 * @return curve_ders Derivatives of the curve at u.
 * E.g. curve_ders[n] is the nth derivative at u, where 0 <= n <= num_ders.
 */
inline std::vector<vec3> curve_derivatives(const Curve& crv, int num_ders, float u)
{
    return curve_derivatives(crv.m_degree, crv.m_knots, crv.m_control_points, num_ders, u);
}

inline std::vector<vec3> curve_derivatives(const RationalCurve& crv, int num_ders, float u)
{

    // typedef glm::vec<3, T> tvecn;
    // typedef glm::vec<4, T> tvecnp1;

    std::vector<vec3> curve_ders;
    curve_ders.reserve(num_ders + 1);

    // Compute homogenous coordinates of control points
    std::vector<vec4> cw;
    cw.reserve(crv.m_control_points.size());
    for(size_t i = 0; i < crv.m_control_points.size(); i++)
    {
        cw.push_back(vec4(crv.m_control_points[i].x() * crv.m_weights[i], crv.m_control_points[i].y() * crv.m_weights[i],
                                crv.m_control_points[i].z() * crv.m_weights[i], crv.m_weights[i]));
    }

    // Derivatives of Cw
    std::vector<vec4> cw_ders = curve_derivatives(crv.m_degree, crv.m_knots, cw, num_ders, u);

    // Split Cwders into coordinates and weights
    std::vector<vec3> a_ders;
    std::vector<float> w_ders;
    for(const auto& val: cw_ders)
    {
        a_ders.push_back(vec3(val.x(), val.y(), val.z()));
        w_ders.push_back(val.w());
    }

    // Compute rational derivatives
    for(int k = 0; k <= num_ders; k++)
    {
        vec3 v = a_ders[k];
        for(int i = 1; i <= k; i++)
        {
            v -= binomial(k, i) * w_ders[i] * curve_ders[k - i];
        }
        curve_ders.push_back(v / w_ders[0]);
    }
    return curve_ders;
}

/**
 * Evaluate the tangent of a B-spline curve
 * @param[in] crv Curve object
 * @return Unit tangent of the curve at u.
 */
inline vec3 curve_tangent(const Curve& crv, float u)
{
    std::vector<vec3> ders = curve_derivatives(crv, 1, u);
    vec3 du = ders[1];

    float du_len = du.norm();
    if(!close(du_len, 0.0f))
    {
        du /= du_len;
    }
    return du;
}
/**
 * Evaluate the tangent of a rational B-spline curve
 * @param[in] crv RationalCurve object
 * @return Unit tangent of the curve at u.
 */
inline vec3 curve_tangent(const RationalCurve& crv, float u)
{
    std::vector<vec3> ders = curve_derivatives(crv, 1, u);
    vec3 du = ders[1];
    float du_len = du.norm();
    if(!close(du_len, 0.0f))
    {
        du /= du_len;
    }
    return du;
}
/**
 * Insert knots in the curve
 * @param[in] deg Degree of the curve
 * @param[in] knots Knot vector of the curve
 * @param[in] cp Control points of the curve
 * @param[in] u Parameter to insert knot(s) at
 * @param[in] r Number of times to insert knot
 * @param[out] new_knots Updated knot vector
 * @param[out] new_cp Updated control points
 */
inline void curve_knot_insert(size_t deg, const std::vector<float>& knots, const std::vector<vec3>& cp, float u, size_t r,
                              std::vector<float>& new_knots, std::vector<vec3>& new_cp)
{
    int k = find_span(deg, knots, u);
    size_t s = knot_multiplicity(knots, u);
    assert(s <= deg);
    if(s == deg)
    {
        new_knots = knots;
        new_cp = cp;
        return;
    }
    if((r + s) > deg)
    {
        r = deg - s;
    }

    // Insert new knots between k and (k + 1)
    new_knots.resize(knots.size() + r);
    for(int i = 0; i < k + 1; ++i)
    {
        new_knots[i] = knots[i];
    }
    for(size_t i = 1; i < r + 1; ++i)
    {
        new_knots[k + i] = u;
    }
    for(int i = k + 1; i < knots.size(); ++i)
    {
        new_knots[i + r] = knots[i];
    }
    // Copy unaffected control points
    new_cp.resize(cp.size() + r);
    for(int i = 0; i < k - deg + 1; ++i)
    {
        new_cp[i] = cp[i];
    }
    for(int i = k - s; i < cp.size(); ++i)
    {
        new_cp[i + r] = cp[i];
    }
    // Copy affected control points
    std::vector<vec3> tmp;
    tmp.resize(deg - s + 1);
    for(int i = 0; i < deg - s + 1; ++i)
    {
        tmp[i] = cp[k - deg + i];
    }
    // Modify affected control points
    for(int j = 1; j <= r; ++j)
    {
        int L = k - deg + j;
        for(int i = 0; i < deg - j - s + 1; ++i)
        {
            float a = (u - knots[L + i]) / (knots[i + k + 1] - knots[L + i]);
            tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
        }
        new_cp[L] = tmp[0];
        new_cp[k + r - j - s] = tmp[deg - j - s];
    }
    int L = k - deg + r;
    for(int i = L + 1; i < k - s; ++i)
    {
        new_cp[i] = tmp[i - L];
    }
}
inline void curve_knot_insert(size_t deg, const std::vector<float>& knots, const std::vector<vec4>& cp, float u, size_t r,
                              std::vector<float>& new_knots, std::vector<vec4>& new_cp)
{
    int k = find_span(deg, knots, u);
    size_t s = knot_multiplicity(knots, u);
    assert(s <= deg);
    if(s == deg)
    {
        new_knots = knots;
        new_cp = cp;
        return;
    }
    if((r + s) > deg)
    {
        r = deg - s;
    }

    // Insert new knots between k and (k + 1)
    new_knots.resize(knots.size() + r);
    for(int i = 0; i < k + 1; ++i)
    {
        new_knots[i] = knots[i];
    }
    for(size_t i = 1; i < r + 1; ++i)
    {
        new_knots[k + i] = u;
    }
    for(int i = k + 1; i < knots.size(); ++i)
    {
        new_knots[i + r] = knots[i];
    }
    // Copy unaffected control points
    new_cp.resize(cp.size() + r);
    for(int i = 0; i < k - deg + 1; ++i)
    {
        new_cp[i] = cp[i];
    }
    for(int i = k - s; i < cp.size(); ++i)
    {
        new_cp[i + r] = cp[i];
    }
    // Copy affected control points
    std::vector<vec4> tmp;
    tmp.resize(deg - s + 1);
    for(int i = 0; i < deg - s + 1; ++i)
    {
        tmp[i] = cp[k - deg + i];
    }
    // Modify affected control points
    for(int j = 1; j <= r; ++j)
    {
        int L = k - deg + j;
        for(int i = 0; i < deg - j - s + 1; ++i)
        {
            float a = (u - knots[L + i]) / (knots[i + k + 1] - knots[L + i]);
            tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
        }
        new_cp[L] = tmp[0];
        new_cp[k + r - j - s] = tmp[deg - j - s];
    }
    int L = k - deg + r;
    for(int i = L + 1; i < k - s; ++i)
    {
        new_cp[i] = tmp[i - L];
    }
}
/**
 * Insert knots in the curve
 * @param[in] crv Curve object
 * @param[in] u Parameter to insert knot at
 * @param[in] repeat Number of times to insert
 * @return New curve with #repeat knots inserted at u
 */
inline Curve curve_knot_insert(const Curve& crv, float u, size_t repeat = 1)
{
    Curve new_crv;
    new_crv.m_degree = crv.m_degree;
    curve_knot_insert(crv.m_degree, crv.m_knots, crv.m_control_points, u, repeat, new_crv.m_knots, new_crv.m_control_points);
    return new_crv;
}

/**
 * Insert knots in the rational curve
 * @param[in] crv RationalCurve object
 * @param[in] u Parameter to insert knot at
 * @param[in] repeat Number of times to insert
 * @return New RationalCurve object with #repeat knots inserted at u
 */
inline RationalCurve curve_knot_insert(const RationalCurve& crv, float u, size_t repeat = 1)
{
    RationalCurve new_crv;
    new_crv.m_degree = crv.m_degree;

    // Convert to homogenous coordinates
    std::vector<vec4> Cw;
    Cw.resize(crv.m_control_points.size());
    for(int i = 0; i < crv.m_control_points.size(); ++i)
    {
        Cw.push_back(vec4(crv.m_control_points[i][0] * crv.m_weights[i], crv.m_control_points[i][1] * crv.m_weights[i],
                                crv.m_control_points[i][2] * crv.m_weights[i], crv.m_weights[i]));
    }

    // Perform knot insertion and get new knots and control points
    std::vector<vec4> new_Cw;
    std::vector<float> new_knots;
    curve_knot_insert(crv.m_degree, crv.m_knots, Cw, u, repeat, new_crv.m_knots, new_Cw);

    // Convert back to cartesian coordinates
    new_crv.m_control_points.resize(new_Cw.size());
    new_crv.m_weights.resize(new_Cw.size());
    for(int i = 0; i < new_Cw.size(); ++i)
    {
        new_crv.m_control_points.push_back(vec3(new_Cw[i][0] / new_Cw[i][3], new_Cw[i][1] / new_Cw[i][3], new_Cw[i][2] / new_Cw[i][3]));
        new_crv.m_weights.push_back(new_Cw[i].w());
    }
    return new_crv;
}
/**
 * Split the curve into two
 * @param[in] degree Degree of curve
 * @param[in] knots Knot vector
 * @param[in] control_points Array of control points
 * @param[in] u Parameter to split curve
 * @param[out] left_knots Knots of the left part of the curve
 * @param[out] left_control_points Control points of the left part of the curve
 * @param[out] right_knots Knots of the right part of the curve
 * @param[out] right_control_points Control points of the right part of the curve
 */
inline void curve_split(size_t degree, const std::vector<float>& knots, const std::vector<vec3>& control_points, float u,
                        std::vector<float>& left_knots, std::vector<vec3>& left_control_points, std::vector<float>& right_knots,
                        std::vector<vec3>& right_control_points)
{
    std::vector<float> tmp_knots;
    std::vector<vec3> tmp_cp;

    int span = find_span(degree, knots, u);
    int r = degree - knot_multiplicity(knots, u);

    curve_knot_insert(degree, knots, control_points, u, r, tmp_knots, tmp_cp);

    left_knots.clear();
    right_knots.clear();
    left_control_points.clear();
    right_control_points.clear();

    int span_l = find_span(degree, tmp_knots, u) + 1;
    for(int i = 0; i < span_l; ++i)
    {
        left_knots.push_back(tmp_knots[i]);
    }
    left_knots.push_back(u);

    for(int i = 0; i < degree + 1; ++i)
    {
        right_knots.push_back(u);
    }
    for(int i = span_l; i < tmp_knots.size(); ++i)
    {
        right_knots.push_back(tmp_knots[i]);
    }

    int ks = span - degree + 1;
    for(int i = 0; i < ks + r; ++i)
    {
        left_control_points.push_back(tmp_cp[i]);
    }
    for(int i = ks + r - 1; i < tmp_cp.size(); ++i)
    {
        right_control_points.push_back(tmp_cp[i]);
    }
}
inline void curve_split(size_t degree, const std::vector<float>& knots, const std::vector<vec4>& control_points, float u,
                        std::vector<float>& left_knots, std::vector<vec4>& left_control_points, std::vector<float>& right_knots,
                        std::vector<vec4>& right_control_points)
{
    std::vector<float> tmp_knots;
    std::vector<vec4> tmp_cp;

    int span = find_span(degree, knots, u);
    int r = degree - knot_multiplicity(knots, u);

    curve_knot_insert(degree, knots, control_points, u, r, tmp_knots, tmp_cp);

    left_knots.clear();
    right_knots.clear();
    left_control_points.clear();
    right_control_points.clear();

    int span_l = find_span(degree, tmp_knots, u) + 1;
    for(int i = 0; i < span_l; ++i)
    {
        left_knots.push_back(tmp_knots[i]);
    }
    left_knots.push_back(u);

    for(int i = 0; i < degree + 1; ++i)
    {
        right_knots.push_back(u);
    }
    for(int i = span_l; i < tmp_knots.size(); ++i)
    {
        right_knots.push_back(tmp_knots[i]);
    }

    int ks = span - degree + 1;
    for(int i = 0; i < ks + r; ++i)
    {
        left_control_points.push_back(tmp_cp[i]);
    }
    for(int i = ks + r - 1; i < tmp_cp.size(); ++i)
    {
        right_control_points.push_back(tmp_cp[i]);
    }
}

/**
 * Split a curve into two
 * @param[in] crv Curve object
 * @param[in] u Parameter to split at
 * @return Tuple with first half and second half of the curve
 */
inline Tuple<Curve, Curve> curve_split(const Curve& crv, float u)
{
    Curve left, right;
    left.m_degree = crv.m_degree;
    right.m_degree = crv.m_degree;
    curve_split(crv.m_degree, crv.m_knots, crv.m_control_points, u, left.m_knots, left.m_control_points, right.m_knots, right.m_control_points);
    return Tuple<Curve, Curve>(std::move(left), std::move(right));
}

/**
 * Split a rational curve into two
 * @param[in] crv RationalCurve object
 * @param[in] u Parameter to split at
 * @return Tuple with first half and second half of the curve
 */
inline Tuple<RationalCurve, RationalCurve> curve_split(const RationalCurve& crv, float u)
{
    RationalCurve left, right;
    left.m_degree = crv.m_degree;
    right.m_degree = crv.m_degree;

    std::vector<vec4> Cw, left_Cw, right_Cw;
    Cw.resize(crv.m_control_points.size());
    for(int i = 0; i < crv.m_control_points.size(); ++i)
    {
        Cw.push_back(cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]));
    }

    curve_split(crv.m_degree, crv.m_knots, Cw, u, left.m_knots, left_Cw, right.m_knots, right_Cw);

    left.m_control_points.reserve(left_Cw.size());
    left.m_weights.reserve(left_Cw.size());
    right.m_control_points.reserve(right_Cw.size());
    right.m_weights.reserve(right_Cw.size());
    for(int i = 0; i < left_Cw.size(); ++i)
    {
        left.m_control_points.push_back(homogenous_to_cartesian(left_Cw[i]));
        left.m_weights.push_back(left_Cw[i].w());
    }
    for(int i = 0; i < right_Cw.size(); ++i)
    {
        right.m_control_points.push_back(util::homogenous_to_cartesian(right_Cw[i]));
        right.m_weights.push_back(right_Cw[i].w());
    }
    return Tuple<RationalCurve, RationalCurve>(std::move(left), std::move(right));
}
/*
 *
 */
inline bool least_square_approximation(int degree, const std::vector<vec3>& through_points, int control_points_count, RationalCurve& crv)
{
    int n = control_points_count;
    int m = through_points.size();
    std::vector<float> uk = get_chord_parameterization(through_points);
    std::vector<float> knot_vector(n + degree + 1, 1.0f);

    float d = (float)m / (float)n;
    for(int j = 0; j <= degree; j++)
    {
        knot_vector[j] = 0.0f;
    }
    for(int j = 1; j < n - degree; j++)
    {
        knot_vector[degree + j] = 0.0f;
        for(int k = j; k < j + degree; k++)
        {
            int i1 = k * d;
            float a = k * d - i1;
            int i2 = ((k - 1) * d);
            knot_vector[degree + j] += a * uk[i2] + (1 - a) * uk[i1];
        }
        knot_vector[degree + j] /= degree;
    }

    std::vector<vec3> control_points(n);
    std::vector<float> weights(n, 1.0f);
    std::vector<vec3> R(n);
    std::vector<vec3> rk(m);
    std::vector<float> funs(degree + 1);
    std::vector<std::vector<float>> N(m, std::vector<float>(n));
    R[0] = through_points[0];
    R[n - 1] = through_points[m - 1];
    N[0][0] = 1.0;
    N[m - 1][n - 1] = 1.0;

    for(int i = 0; i < m; i++)
    {
        int spanIndex = find_span(degree, knot_vector, uk[i]);
        std::vector<float> basis = bspline_basis(degree, spanIndex, knot_vector, uk[i]);
        for(int j = 0; j <= degree; j++)
        {
            N[i][spanIndex - degree + j] = basis[j];
        }
        rk[i] = through_points[i] - N[i][0] * through_points[0] - N[i][n - 1] * through_points[m - 1];
    }

    for(int i = 0; i < n; i++)
    {
        R[i] = vec3::Zero();
        for(int j = 0; j < m; j++)
        {
            R[i] += N[j][i] * rk[j];
        }
        // this may don't work
        if(close(R[i], vec3::Zero()))
        {
            // return false;
        }
    }

    if(n - 2 > 0)
    {
        std::vector<std::vector<float>> X(n - 2, std::vector<float>(3));
        std::vector<std::vector<float>> B(n - 2, std::vector<float>(3));
        std::vector<std::vector<float>> Ns(m - 2, std::vector<float>(n - 2));
        for(int i = 0; i < n - 2; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                B[i][j] = R[i + 1][j];
            }
        }
        for(int i = 1; i <= m - 2; i++)
        {
            for(int j = 1; j <= n - 2; j++)
            {
                Ns[i - 1][j - 1] = N[i][j];
            }
        }

        std::vector<std::vector<float>> NsT;
        transpose(Ns, NsT);
        auto NsTNs = matrix_multiply(NsT, Ns);
        X = solve_linear_system(NsTNs, B);

        for(int i = 0; i < n - 2; i++)
        {
            control_points[i + 1] = vec3(X[i][0], X[i][1], X[i][2]);
        }
    }
    control_points[0] = vec3(through_points[0].x(), through_points[0].y(), through_points[0].z());
    control_points[n - 1] = vec3(through_points[m - 1].x(), through_points[m - 1].y(), through_points[m - 1].z());

    crv.m_degree = degree;
    crv.m_knots = knot_vector;
    crv.m_control_points = control_points;
    crv.m_weights = weights;
    return true;
}

/*
 * Evaluate a ration B-spline curve by throughPoints
 * @param[in] degree Degree of the ration B-spline curve
 * @param[in] throughpoints The points that the ration B-spline curve will fit
 * @param[out] crv The ration B-spline curve will fit
 */
inline void global_interpolation(int degree, const Array<vec3>& throughpoints, RationalCurve& crv)
{

    int size = throughpoints.size();
    int n = size - 1;

    // The chord length parameterization maybe others better
    Array<float> uk(size);
    uk[n] = 1.0f;
    float d = 0.0f;
    for(int i = 1; i < size; i++)
    {
        d += (throughpoints[i] - throughpoints[i - 1]).norm();
    }
    for(int i = 1; i < n; i++)
    {
        uk[i] = uk[i - 1] + (throughpoints[i] - throughpoints[i - 1]).norm() / d;
    }

    // AverageKnotVector
    int m = n + degree + 1;
    Array<float> knot_vector(m + 1, 0.0f);
    for(int i = m - degree; i <= m; i++)
    {
        knot_vector[i] = 1.0f;
    }

    for(int j = 1; j <= n - degree; j++)
    {
        float sum = 0.0f;
        for(int i = j; i <= j + degree - 1; i++)
        {
            sum += uk[i];
        }
        knot_vector[j + degree] = (1.0f / degree) * sum;
    }

    Eigen::SparseMatrix<float> a;
    Eigen::SparseMatrix<float> b;
    Array<Eigen::Triplet<float>> triplets_a;
    Array<Eigen::Triplet<float>> triplets_b;
    for(int i = 1; i < n; i++)
    {
        int spanIndex = find_span(degree, knot_vector, uk[i]);
        std::vector<float> basis = bspline_basis(degree, spanIndex, knot_vector, uk[i]);
        for(int j = 0; j <= degree; j++)
        {
            triplets_a.emplace_back(i, spanIndex - degree + j, basis[j]);
        }
    }
    triplets_a.emplace_back(0, 0, 1.0f);
    triplets_a.emplace_back(n, n, 1.0f);
    a.resize(n + 1, n + 1);
    a.setFromTriplets(triplets_a.begin(), triplets_a.end());
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            triplets_b.emplace_back(i, j, throughpoints[i][j]);
        }
    }
    b.resize(n + 1, 3);
    b.setFromTriplets(triplets_b.begin(), triplets_b.end());

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
    solver.compute(a);

    Eigen::MatrixXf result = solver.solve(b);

    std::vector<vec3> control_points(size);
    std::vector<float> weight(size);
    for(int i = 0; i < result.rows(); i++)
    {
        control_points[i] = vec3(result(i, 0), result(i, 1), result(i, 2));
        weight[i] = 1.0f;
    }
    crv.m_degree = degree;
    crv.m_knots = knot_vector;
    crv.m_control_points = control_points;
    crv.m_weights = weight;
}

}// namespace util

}// namespace nurbs
}// namespace nous