#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "nurbs_curve.h"
#include "nurbs_util.h"

namespace Geomerty::nurbs::util
{
	/**
	 * Returns whether the curve is valid
	 * @tparam T Type of control point coordinates, knot values and weights
	 * @param[in] degree Degree of curve
	 * @param[in] knots Knot vector of curve
	 * @param[in] control_points Control points of curve
	 * @return Whether valid
	 */
	inline bool curve_is_valid(const size_t degree, const std::vector<scalar>& knots, const std::vector<vec3>& control_points, const std::vector<scalar>& weights)
	{
		if (!is_valid_relation(degree, knots.size(), control_points.size()))
		{
			return false;
		}
		if (weights.size() != control_points.size())
		{
			return false;
		}
		return true;
	}

	/**
	 * Returns whether the curve is valid
	 * @tparam T Type of control point coordinates, knot values
	 * @param[in] crv RationalCurve object
	 * @return Whether valid
	 */
	inline bool curve_is_valid(const RationalCurve& crv) { return curve_is_valid(crv.m_degree, crv.m_knots, crv.m_control_points, crv.m_weights); }

	/**
	 * Evaluate point on a nonrational NURBS curve
	 * @param[in] degree Degree of the given curve.
	 * @param[in] knots Knot vector of the curve.
	 * @param[in] control_points Control points of the curve.
	 * @param[in] u Parameter to evaluate the curve at.
	 * @return point result point on the curve at parameter u.
	 */
	template<typename T> inline T curve_point(const size_t degree, const std::vector<scalar>& knots, const std::vector<T>& control_points, scalar u)
	{

		// Initialize result to 0s
		T point = T::Zero();

		// Find span and corresponding non-zero basis functions
		int span = find_span(degree, knots, u);
		std::vector<scalar> n = bspline_basis(degree, span, knots, u);
		assert(!n.empty());

		// Compute point
		for (int j = 0; j < degree + 1; j++)
		{
			point += static_cast<scalar>(n[j]) * control_points[span - degree + j];
		}
		return point;

	}
	inline vec3 curve_point(const RationalCurve& crv, scalar u)
	{
		if (!curve_is_valid(crv))
		{
			return vec3::Zero();
		}
		// Compute homogenous coordinates of control points
		std::vector<vec4> cw;
		cw.reserve(crv.m_control_points.size());
		for (int i = 0; i < crv.m_control_points.size(); i++)
		{
			cw.emplace_back(cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]));
		}
		// Compute point using homogenous coordinates
		vec4 point_w = curve_point<vec4>(crv.m_degree, crv.m_knots, cw, u);
		// Convert back to cartesian coordinates
		return homogenous_to_cartesian(point_w);
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
	template<typename T>
	inline std::vector<T> curve_derivatives(const size_t degree, const std::vector<scalar>& knots, const std::vector<T>& control_points, int num_ders, scalar u)
	{

		std::vector<T> curve_ders;
		curve_ders.resize(num_ders + 1);

		// Assign higher order derivatives to zero
		for (int k = degree + 1; k < num_ders + 1; k++)
		{
			curve_ders[k] = T::Zero();
		}

		// Find the span and corresponding non-zero basis functions & derivatives
		int span = find_span(degree, knots, u);
		std::vector<std::vector<scalar>> ders = bspline_der_basis(degree, span, knots, u, num_ders);

		// Compute first num_ders derivatives
		int du = num_ders < static_cast<int>(degree) ? num_ders : static_cast<int>(degree);
		for (int k = 0; k < du + 1; k++)
		{
			curve_ders[k] = T::Zero();
			for (int j = 0; j < degree + 1; j++)
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

	inline std::vector<vec3> curve_derivatives(const RationalCurve& crv, int num_ders, scalar u)
	{
		if (!curve_is_valid(crv))
		{
			return std::vector<vec3>{};
		}
		std::vector<vec3> curve_ders;
		curve_ders.reserve(num_ders + 1);

		// Compute homogenous coordinates of control points
		std::vector<vec4> cw;
		cw.reserve(crv.m_control_points.size());
		for (int i = 0; i < crv.m_control_points.size(); i++)
		{
			cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]);
			cw.emplace_back(cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]));
		}

		// Derivatives of Cw
		std::vector<vec4> cw_ders = curve_derivatives<vec4>(crv.m_degree, crv.m_knots, cw, num_ders, u);

		// Split Cwders into coordinates and weights
		std::vector<vec3> a_ders;
		std::vector<scalar> w_ders;
		a_ders.reserve(cw_ders.size());
		w_ders.reserve(w_ders.size());
		for (const auto& val : cw_ders)
		{

			a_ders.emplace_back(val.head<3>());
			w_ders.emplace_back(val.w());
		}

		// Compute rational derivatives
		for (int k = 0; k < num_ders + 1; k++)
		{
			vec3 v = a_ders[k];
			for (int i = 1; i < k + 1; i++)
			{
				v -= binomial(k, i) * w_ders[i] * curve_ders[k - i];
			}
			curve_ders.emplace_back(v / w_ders[0]);
		}
		return curve_ders;
	}

	/**
	 * Evaluate the tangent of a rational B-spline curve
	 * @param[in] crv RationalCurve object
	 * @return Unit tangent of the curve at u.
	 */
	inline vec3 curve_tangent(const RationalCurve& crv, scalar u)
	{
		if (!curve_is_valid(crv))
		{
			return vec3::Zero();
		}
		std::vector<vec3> ders = curve_derivatives(crv, 1, u);
		vec3 du = ders[1];
		scalar du_len = du.norm();
		if (!close(du_len, 0.0f))
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
	template<typename T>
	inline void curve_knot_insert(size_t deg, const std::vector<scalar>& knots, const std::vector<T>& cp, scalar u, size_t r, std::vector<scalar>& new_knots,
		std::vector<T>& new_cp)
	{
		int k = find_span(deg, knots, u);
		size_t s = knot_multiplicity(knots, u);
		if (s > deg)
		{
			return;
		}
		if (s == deg)
		{
			new_knots = knots;
			new_cp = cp;
			return;
		}
		if ((r + s) > deg)
		{
			r = deg - s;
		}

		// Insert new knots between k and (k + 1)
		new_knots.resize(knots.size() + r);
		for (int i = 0; i < k + 1; i++)
		{
			new_knots[i] = knots[i];
		}
		for (int i = 1; i < r + 1; i++)
		{
			new_knots[k + i] = u;
		}
		for (int i = k + 1; i < knots.size(); i++)
		{
			new_knots[i + r] = knots[i];
		}
		// Copy unaffected control points
		new_cp.resize(cp.size() + r);
		for (int i = 0; i < k - deg + 1; i++)
		{
			new_cp[i] = cp[i];
		}
		for (int i = k - s; i < cp.size(); i++)
		{
			new_cp[i + r] = cp[i];
		}
		// Copy affected control points
		std::vector<T> tmp;
		tmp.resize(deg - s + 1);
		for (int i = 0; i < deg - s + 1; i++)
		{
			tmp[i] = cp[k - deg + i];
		}
		// Modify affected control points
		for (int j = 1; j < r + 1; j++)
		{
			int L = k - deg + j;
			for (int i = 0; i < deg - j - s + 1; i++)
			{
				scalar a = (u - knots[L + i]) / (knots[i + k + 1] - knots[L + i]);
				tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
			}
			new_cp[L] = tmp[0];
			new_cp[k + r - j - s] = tmp[deg - j - s];
		}
		int L = k - deg + r;
		for (int i = L + 1; i < k - s; i++)
		{
			new_cp[i] = tmp[i - L];
		}
	}

	/**
	 * Insert knots in the rational curve
	 * @param[in] crv RationalCurve object
	 * @param[in] u Parameter to insert knot at
	 * @param[in] repeat Number of times to insert
	 * @return New RationalCurve object with #repeat knots inserted at u
	 */
	inline RationalCurve curve_knot_insert(const RationalCurve& crv, scalar u, size_t repeat = 1)
	{
		if (!curve_is_valid(crv))
		{
			return RationalCurve();
		}
		RationalCurve new_crv;
		new_crv.m_degree = crv.m_degree;

		// Convert to homogenous coordinates
		std::vector<vec4> cw;
		cw.resize(crv.m_control_points.size());
		for (int i = 0; i < crv.m_control_points.size(); i++)
		{
			cw[i] = cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]);
		}

		// Perform knot insertion and get new knots and control points
		std::vector<vec4> new_cw;
		std::vector<scalar> new_knots;
		curve_knot_insert<vec4>(crv.m_degree, crv.m_knots, cw, u, repeat, new_crv.m_knots, new_cw);

		// Convert back to cartesian coordinates
		new_crv.m_control_points.resize(new_cw.size());
		new_crv.m_weights.resize(new_cw.size());
		for (int i = 0; i < new_cw.size(); i++)
		{

			new_crv.m_control_points[i] = homogenous_to_cartesian(new_cw[i]);
			new_crv.m_weights[i] = new_cw[i][3];
		}
		return new_crv;
	}
	/**
	 * Split the curve into two
	 * @param[in] degree Degree of curve
	 * @param[in] knots Knot vector
	 * @param[in] control_points std::vector of control points
	 * @param[in] u Parameter to split curve
	 * @param[out] left_knots Knots of the left part of the curve
	 * @param[out] left_control_points Control points of the left part of the curve
	 * @param[out] right_knots Knots of the right part of the curve
	 * @param[out] right_control_points Control points of the right part of the curve
	 */
	template<typename T>
	inline void curve_split(const size_t degree, const std::vector<scalar>& knots, const std::vector<T>& control_points, scalar u, std::vector<scalar>& left_knots,
		std::vector<T>& left_control_points, std::vector<scalar>& right_knots, std::vector<T>& right_control_points)
	{
		if (u <= knots[degree] || u >= knots[knots.size() - degree - 1])
		{
			return;
		}
		std::vector<scalar> tmp_knots;
		std::vector<T> tmp_cp;

		int span = find_span(degree, knots, u);
		int r = degree - knot_multiplicity(knots, u);

		curve_knot_insert(degree, knots, control_points, u, r, tmp_knots, tmp_cp);

		left_knots.clear();
		right_knots.clear();
		left_control_points.clear();
		right_control_points.clear();

		int span_l = find_span(degree, tmp_knots, u) + 1;
		for (int i = 0; i < span_l; i++)
		{
			left_knots.emplace_back(tmp_knots[i]);
		}
		left_knots.emplace_back(u);

		for (int i = 0; i < degree + 1; i++)
		{
			right_knots.emplace_back(u);
		}
		for (int i = span_l; i < tmp_knots.size(); i++)
		{
			right_knots.emplace_back(tmp_knots[i]);
		}

		int ks = span - degree + 1;
		for (int i = 0; i < ks + r; i++)
		{
			left_control_points.emplace_back(tmp_cp[i]);
		}
		for (int i = ks + r - 1; i < tmp_cp.size(); i++)
		{
			right_control_points.emplace_back(tmp_cp[i]);
		}
	}

	/**
	 * Split a rational curve into two
	 * @param[in] crv RationalCurve object
	 * @param[in] u Parameter to split at
	 * @return std::tuple with first half and second half of the curve
	 */
	inline std::tuple<RationalCurve, RationalCurve> curve_split(const RationalCurve& crv, scalar u)
	{
		if (!curve_is_valid(crv))
		{
			return {};
		}
		RationalCurve left, right;
		left.m_degree = crv.m_degree;
		right.m_degree = crv.m_degree;

		std::vector<vec4> Cw, left_Cw, right_Cw;
		Cw.resize(crv.m_control_points.size());
		for (int i = 0; i < crv.m_control_points.size(); i++)
		{
			Cw[i] = cartesian_to_homogenous(crv.m_control_points[i], crv.m_weights[i]);
		}

		curve_split(crv.m_degree, crv.m_knots, Cw, u, left.m_knots, left_Cw, right.m_knots, right_Cw);

		left.m_control_points.reserve(left_Cw.size());
		left.m_weights.reserve(left_Cw.size());
		right.m_control_points.reserve(right_Cw.size());
		right.m_weights.reserve(right_Cw.size());
		for (int i = 0; i < left_Cw.size(); i++)
		{
			left.m_control_points.emplace_back(homogenous_to_cartesian(left_Cw[i]));
			left.m_weights.emplace_back(left_Cw[i].w());
		}
		for (int i = 0; i < right_Cw.size(); i++)
		{
			right.m_control_points.emplace_back(util::homogenous_to_cartesian(right_Cw[i]));
			right.m_weights.emplace_back(right_Cw[i].w());
		}
		return std::tuple<RationalCurve, RationalCurve>(std::move(left), std::move(right));
	}
	/*
	 * Fit a NurbsSurface by through_points
	 */
	inline bool least_square_approximation(const size_t degree, const std::vector<vec3>& through_points, int control_points_count, RationalCurve& crv)
	{
		int n = control_points_count;
		int m = through_points.size();
		std::vector<scalar> uk = get_chord_parameterization(through_points);
		std::vector<scalar> knot_vector(n + degree + 1, 1.0f);

		scalar d = (scalar)m / (scalar)n;
		for (int j = 0; j < degree + 1; j++)
		{
			knot_vector[j] = 0.0f;
		}
		for (int j = 1; j < n - degree; j++)
		{
			knot_vector[degree + j] = 0.0f;
			for (int k = j; k < j + degree; k++)
			{
				int i1 = k * d;
				scalar a = k * d - i1;
				int i2 = ((k - 1) * d);
				knot_vector[degree + j] += a * uk[i2] + (1 - a) * uk[i1];
			}
			knot_vector[degree + j] /= degree;
		}

		std::vector<vec3> control_points(n);
		std::vector<scalar> weights(n, 1.0f);
		std::vector<vec3> R(n);
		std::vector<vec3> rk(m);
		std::vector<scalar> funs(degree + 1);
		std::vector<std::vector<scalar>> N(m, std::vector<scalar>(n));
		R[0] = through_points[0];
		R[n - 1] = through_points[m - 1];
		N[0][0] = 1.0;
		N[m - 1][n - 1] = 1.0;

		for (int i = 0; i < m; i++)
		{
			int spanIndex = find_span(degree, knot_vector, uk[i]);
			std::vector<scalar> basis = bspline_basis(degree, spanIndex, knot_vector, uk[i]);
			for (int j = 0; j < degree + 1; j++)
			{
				N[i][spanIndex - degree + j] = basis[j];
			}
			rk[i] = through_points[i] - N[i][0] * through_points[0] - N[i][n - 1] * through_points[m - 1];
		}

		for (int i = 0; i < n; i++)
		{
			R[i] = vec3::Zero();
			for (int j = 0; j < m; j++)
			{
				R[i] += N[j][i] * rk[j];
			}
			// this may don't work
			if (close(R[i], vec3::Zero()))
			{
				// return false;
			}
		}

		if (n - 2 > 0)
		{
			Eigen::Matrix<scalar, -1, -1> B = Eigen::Matrix<scalar, -1, -1>::Zero(n - 2, 3);
			Eigen::Matrix<scalar, -1, -1> Ns = Eigen::Matrix<scalar, -1, -1>::Zero(m - 2, n - 2);
			for (int i = 0; i < n - 2; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					B(i, j) = R[i + 1][j];
				}
			}
			for (int i = 1; i < m - 1; i++)
			{
				for (int j = 1; i < n - 1; j++)
				{
					Ns(i - 1, j - 1) = N[i][j];
				}
			}

			auto NsT = Ns.transpose();
			auto NsTNs = NsT * Ns;
			Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic> X = NsTNs.lu().solve(B);

			for (int i = 0; i < n - 2; i++)
			{
				control_points[i + 1] = X.row(i);
			}
		}
		control_points[0] = vec3(through_points[0].x(), through_points[0].y(), through_points[0].z());
		control_points[n - 1] = vec3(through_points[m - 1].x(), through_points[m - 1].y(), through_points[m - 1].z());

		crv.m_degree = degree;
		crv.m_knots.swap(knot_vector);
		crv.m_control_points.swap(control_points);
		crv.m_weights.swap(weights);
		return true;
	}

	/*
	 * Evaluate a ration B-spline curve by throughPoints
	 * @param[in] degree Degree of the ration B-spline curve
	 * @param[in] throughpoints The points that the ration B-spline curve will fit
	 * @param[out] crv the ration B-spline curve will fit
	 */
	inline void global_interpolation(const size_t degree, const std::vector<vec3>& throughpoints, RationalCurve& crv, const std::vector<scalar>& params = {})
	{

		int size = throughpoints.size();
		int n = size - 1;
		// The chord length parameterization maybe others better
		std::vector<scalar> uk(size);
		if (params.size() == 0)
			uk = get_chord_parameterization(throughpoints);
		else
		{
			uk = params;
		}

		// AverageKnotVector
		std::vector<scalar> knot_vector = average_knot_vector(degree, uk);

		// Solve Linear System
		Eigen::Matrix<scalar, -1, -1> A = Eigen::Matrix<scalar, -1, -1>::Zero(size, size);
		for (int i = 1; i < n; i++)
		{

			int span_index = find_span(degree, knot_vector, uk[i]);
			std::vector<scalar> basis = bspline_basis(degree, span_index, knot_vector, uk[i]);
			for (int j = 0; j < degree + 1; j++)
			{
				A(i, span_index - degree + j) = basis[j];
			}
		}
		A(0, 0) = 1.0;
		A(n, n) = 1.0;
		Eigen::Matrix<scalar, -1, -1> right = Eigen::Matrix<scalar, -1, -1>::Zero(size, 3);
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				right(i, j) = throughpoints[i][j];
			}
		}
		Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic> result = A.lu().solve(right);
		std::vector<vec3> control_points(size);
		std::vector<scalar> weight(size);
		for (int i = 0; i < result.rows(); i++)
		{
			control_points[i] = result.row(i);
			weight[i] = 1.0f;
		}
		crv.m_degree = degree;
		crv.m_knots.swap(knot_vector);
		crv.m_control_points.swap(control_points);
		crv.m_weights.swap(weight);
	}


	//
	inline void elevate_degree(const RationalCurve& curve, int times, RationalCurve& result)
	{
		int degree = curve.m_degree;
		std::vector<scalar> knotVector = curve.m_knots;
		std::vector<vec3> controlPoints = curve.m_control_points;
		std::vector<scalar> controlPointsweights = curve.m_weights;

		int n = controlPoints.size() - 1;
		int m = n + degree + 1;
		int ph = degree + times;
		int ph2 = floor(ph / 2);

		std::vector<std::vector<float>> bezalfs(degree + times + 1, std::vector<float>(degree + 1));
		bezalfs[0][0] = bezalfs[ph][degree] = 1.0;

		for (int i = 1; i <= ph2; i++)
		{
			float inv = 1.0 / binomial(ph, i);
			int mpi = std::min(degree, i);

			for (int j = std::max(0, i - times); j <= mpi; j++)
			{
				bezalfs[i][j] = inv * binomial(degree, j) * binomial(times, i - j);
			}
		}

		for (int i = ph2 + 1; i <= ph - 1; i++)
		{
			int mpi = std::min(degree, i);
			for (int j = std::max(0, i - times); j <= mpi; j++)
			{
				bezalfs[i][j] = bezalfs[ph - i][degree - j];
			}
		}

		int mh = ph;
		int kind = ph + 1;
		int r = -1;
		int a = degree;
		int b = degree + 1;
		int cind = 1;
		float ua = knotVector[0];

		int moresize = controlPoints.size() + controlPoints.size() * times;
		std::vector<vec3> updatedControlPoints(moresize, vec3(1e9, 1e9, 1e9));
		std::vector<scalar> updateweights(moresize, 1.0);
		updatedControlPoints[0] = controlPoints[0];
		updateweights[0] = controlPointsweights[0];
		std::vector<float> updatedKnotVector(moresize + ph + 1, 1e9);
		for (int i = 0; i <= ph; i++)
		{
			updatedKnotVector[i] = ua;
		}

		std::vector<vec3> bpts(degree + 1);
		std::vector<scalar> bpts_weights(degree + 1);
		for (int i = 0; i <= degree; i++)
		{
			bpts[i] = controlPoints[i];
			bpts_weights[i] = controlPointsweights[i];
		}

		std::vector<vec3> nextbpts(degree - 1);
		std::vector<float> nextbpts_weight(degree - 1);
		while (b < m)
		{
			int i = b;
			while (b < m && is_almost_equal(knotVector[b], knotVector[b + 1]))
			{
				b = b + 1;
			}
			int mul = b - i + 1;
			mh += mul + times;
			float ub = knotVector[b];

			int oldr = r;
			r = degree - mul;

			int lbz = oldr > 0 ? floor((oldr + 2) / 2) : 1;
			int rbz = r > 0 ? floor(ph - (r + 1) / 2) : ph;

			if (r > 0)
			{
				float numer = ub - ua;
				std::vector<float> alfs(degree - 1);
				for (int k = degree; k > mul; k--)
				{
					alfs[k - mul - 1] = numer / (knotVector[a + k] - ua);
				}
				for (int j = 1; j <= r; j++)
				{
					int save = r - j;
					int s = mul + j;

					for (int k = degree; k >= s; k--)
					{
						bpts[k] = alfs[k - s] * bpts[k] + (1.0 - alfs[k - s]) * bpts[k - 1];
						bpts_weights[k] = alfs[k - s] * bpts_weights[k] + (1.0 - alfs[k - s]) * bpts_weights[k - 1];
					}
					nextbpts[save] = bpts[degree];
					nextbpts_weight[save] = bpts_weights[degree];
				}
			}

			std::vector<vec3> ebpts(degree + times + 1);
			std::vector<scalar> ebpts_weight(degree + times + 1);
			for (int i = lbz; i <= ph; i++)
			{
				ebpts[i] = vec3(0.0, 0.0, 0.0);
				ebpts_weight[i] = 0.0;
				int mpi = std::min(degree, i);
				for (int j = std::max(0, i - times); j <= mpi; j++)
				{
					ebpts[i] += bezalfs[i][j] * bpts[j];
					ebpts_weight[i] += bezalfs[i][j] * bpts_weights[j];
				}
			}

			if (oldr > 1)
			{
				int first = kind - 2;
				int last = kind;
				float den = ub - ua;
				float bet = (ub - updatedKnotVector[kind - 1]) / den;

				for (int tr = 1; tr < oldr; tr++)
				{
					int i = first;
					int j = last;
					int kj = j - kind + 1;

					while (j - i > tr)
					{
						if (i < cind)
						{
							float alf = (ub - updatedKnotVector[i]) / (ua - updatedKnotVector[i]);
							updatedControlPoints[i] = alf * updatedControlPoints[i] + (1.0 - alf) * updatedControlPoints[i - 1];
							updateweights[i] = alf * updateweights[i] + (1.0 - alf) * updateweights[i - 1];
						}

						if (j >= lbz)
						{
							if (j - tr <= kind - ph + oldr)
							{
								float gam = (ub - updatedKnotVector[j - tr]) / den;
								ebpts[kj] = gam * ebpts[kj] + (1.0 - gam) * ebpts[kj + 1];
								ebpts_weight[kj] = gam * ebpts_weight[kj] + (1.0 - gam) * ebpts_weight[kj + 1];
							}
							else
							{
								ebpts[kj] = bet * ebpts[kj] + (1.0 - bet) * ebpts[kj + 1];
								ebpts_weight[kj] = bet * ebpts_weight[kj] + (1.0 - bet) * ebpts_weight[kj + 1];
							}
						}

						i = i + 1;
						j = j - 1;
						kj = kj - 1;
					}

					first -= 1;
					last += 1;
				}
			}

			if (a != degree)
			{
				for (int i = 0; i < ph - oldr; i++)
				{
					updatedKnotVector[kind++] = ua;
				}
			}

			for (int j = lbz; j <= rbz; j++)
			{
				updatedControlPoints[cind] = ebpts[j];
				updateweights[cind++] = ebpts_weight[j];
			}

			if (b < m)
			{
				for (int j = 0; j < r; j++)
				{
					bpts[j] = nextbpts[j];
					bpts_weights[j] = nextbpts_weight[j];
				}
				for (int j = r; j <= degree; j++)
				{
					bpts[j] = controlPoints[b - degree + j];
					bpts_weights[j] = controlPointsweights[b - degree + j];
				}

				a = b;
				b = b + 1;
				ua = ub;
			}
			else
			{
				for (int i = 0; i <= ph; i++)
				{
					updatedKnotVector[kind + i] = ub;
				}
			}
		}

		for (int i = updatedControlPoints.size() - 1; i > 0; i--)
		{
			if (is_almost_equal(updatedControlPoints[i][0], 1e9) && is_almost_equal(updatedControlPoints[i][1], 1e9) &&
				is_almost_equal(updatedControlPoints[i][2], 1e9))
			{
				updatedControlPoints.pop_back();
				updateweights.pop_back();
				continue;
			}
			break;
		}
		for (int i = updatedKnotVector.size() - 1; i > 0; i--)
		{
			if (is_almost_equal(updatedKnotVector[i], 1e9))
			{
				updatedKnotVector.pop_back();
				continue;
			}
			break;
		}
		result.m_degree = ph;
		result.m_knots = updatedKnotVector;
		result.m_control_points = updatedControlPoints;
		result.m_weights = updateweights;
	}
	/*
	 * sample a NURBS curve to a std::vector
	 */
	inline std::vector<vec3> sample_curve(const RationalCurve& crv, int segments)
	{
		scalar u_min = std::numeric_limits<float>::max();
		scalar u_max = -std::numeric_limits<float>::max();
		for (auto u_temp : crv.m_knots)
		{
			u_min = std::min(u_min, u_temp);
			u_max = std::max(u_max, u_temp);
		}
		std::vector<vec3> point_arr(segments + 1);

		for (int i = 0; i < segments + 1; i++)
		{
			scalar u = static_cast<scalar>(i) / static_cast<scalar>(segments);
			scalar norm_u = u_min + (u_max - u_min) * u;
			point_arr[i] = curve_point(crv, norm_u);
		}
		return point_arr;
	}
	/*
	 * Evaluate bounding box on a nonrational NURBS curve
	 * @param[in] crv NURBS curve
	 * @param segments param for sample NURBS curve
	 * return the bounding box with given segments
	 */
	inline AABB3 curve_box(const RationalCurve& crv, int segments)
	{
		const auto point_arr = sample_curve(crv, segments);
		AABB3 box;
		for (auto pos : point_arr)
		{
			box.extend(pos);
		}
		return box;
	}
	/*
	 * Evaluate a closest point to a NURBS curve
	 * @param[in] crv NURBS curve
	 * @param segments param for sample NURBS curve
	 * return the closest point
	 */
	inline vec3 curve_closest_point(const RationalCurve& crv, vec3 pos, int segments)
	{
		const auto point_arr = sample_curve(crv, segments);
		vec3 close_point;
		scalar dist = std::numeric_limits<float>::max();

		for (int i = 1; i < point_arr.size(); i++)
		{
			vec3 temp_nearest_point;
			scalar temp_dist = internal::dist_point_line_segment(pos, point_arr[i - 1], point_arr[i], temp_nearest_point);
			if (temp_dist < dist)
			{
				dist = temp_dist;
				close_point = temp_nearest_point;
			}
		}
		return close_point;
	}
	/*
	*
	*/
	//

	/*
	*+++
	*/
	enum CurveCurveIntersectionType
	{
		Intersecting = 0,
		Parallel = 1,
		Coincident = 2,
		Skew = 3,
	};
	inline CurveCurveIntersectionType ComputeRays(const vec3& point0, const vec3& vector0, const vec3& point1, const vec3& vector1, scalar& param0, scalar& param1, vec3& intersectPoint)
	{
		vec3 v0 = const_cast<vec3&>(vector0);
		vec3 v1 = const_cast<vec3&>(vector1);
		vec3 cross = v0.cross(v1);
		vec3 diff = point1 - point0;
		vec3 coinCross = diff.cross(v1);
		if (cross.isApprox(vec3::Zero()))
		{
			if (coinCross.isApprox(vec3::Zero()))
			{
				return CurveCurveIntersectionType::Coincident;
			}
			else
			{
				return CurveCurveIntersectionType::Parallel;
			}
		}

		scalar crossLength = cross.norm();
		scalar squareLength = crossLength * crossLength;

		vec3 pd1Cross = diff.cross(vector1);
		scalar pd1Dot = pd1Cross.dot(cross);
		param0 = pd1Dot / squareLength;

		vec3 pd2Cross = diff.cross(vector0);
		scalar pd2Dot = pd2Cross.dot(cross);
		param1 = pd2Dot / squareLength;

		vec3 rayP0 = point0 + vector0 * param0;
		vec3 rayP1 = point1 + vector1 * param1;

		if (is_almost_equal(rayP0, rayP1))
		{
			intersectPoint = rayP0;
			return CurveCurveIntersectionType::Intersecting;
		}
		return CurveCurveIntersectionType::Skew;
	}
	inline std::vector<scalar> rescale(const std::vector<scalar>& knotVector, scalar min, scalar max)
	{
		scalar origintMin = knotVector[0];
		scalar origintMax = knotVector.back();
		scalar k = (max - min) / (origintMax - origintMin);

		int size = knotVector.size();
		std::vector<scalar> result(size);
		for (int i = 0; i < size; i++)
		{
			result[i] = (k * knotVector[i] - origintMin) + min;
		}
		return result;
	}
	inline void reparametrize(const RationalCurve& curve, scalar min, scalar max, RationalCurve& result)
	{
		std::vector<scalar> knotVector = curve.m_knots;
		if (is_almost_equal(min, knotVector[0]) && is_almost_equal(max, knotVector[knotVector.size() - 1]))
		{
			result = curve;
			return;
		}

		std::vector<scalar> newKnotVector = rescale(knotVector, min, max);
		result = curve;
		result.m_knots = newKnotVector;
	}
	inline std::map<scalar, int> GetKnotMultiplicityMap(const std::vector<scalar>& knotVector)
	{
		std::map<scalar, int> result;

		for (int i = 0; i < knotVector.size(); i++)
		{
			scalar knot = knotVector[i];
			auto got = result.find(knot);
			if (got == result.end())
			{

				int multi = knot_multiplicity(knotVector, knotVector[i]);
				result.insert(std::make_pair(knotVector[i], multi));
			}
		}
		return result;
	}
	inline std::map<scalar, int> GetInternalKnotMultiplicityMap(const std::vector<scalar>& knotVector)
	{
		auto result = GetKnotMultiplicityMap(knotVector);
		if (!result.empty())
		{
			result.erase(result.begin());
			auto lastElementIterator = prev(result.end(), 1);
			result.erase(lastElementIterator);
		}
		return result;
	}
	inline bool merge(const RationalCurve& left, const RationalCurve& right, RationalCurve& result) {

		int degree_L = left.m_degree;
		std::vector<scalar> knotVector_L = left.m_knots;
		std::vector<vec3> controlPoints_L = left.m_control_points;
		std::vector<scalar>controlPoints_L_w = left.m_weights;

		int degree_R = right.m_degree;
		std::vector<scalar> knotVector_R = right.m_knots;
		std::vector<vec3> controlPoints_R = right.m_control_points;
		std::vector<scalar>controlPoints_R_w = right.m_weights;
		if (!is_almost_equal(controlPoints_L.back() / controlPoints_L_w.back(), controlPoints_R[0] / controlPoints_R_w[0]))
		{
			return false;
		}

		int degree = std::max(degree_L, degree_R);
		RationalCurve  tempL;
		reparametrize(left, 0.0, 1.0, tempL);
		if (degree > degree_L)
		{
			int times = degree - degree_L;
			elevate_degree(left, times, tempL);
		}

		RationalCurve tempR;
		reparametrize(right, 0.0, 1.0, tempR);
		if (degree > degree_R)
		{
			int times = degree - degree_R;
			elevate_degree(right, times, tempR);
		}


		int l = knot_multiplicity(tempL.m_knots, tempL.m_knots.back());
		int r = knot_multiplicity(tempR.m_knots, tempR.m_knots[0]);

		if (l != degree + 1 || r != degree + 1)
		{
			return false;
		}

		int size = tempL.m_control_points.size() + tempR.m_control_points.size() - 1;
		std::vector<vec3> controlPoints(size);
		std::vector<scalar>controlPoints_w(size);
		int ksize = size + degree + 1;
		std::vector<scalar> knotVector(ksize);

		int i;
		for (i = 0; i < tempL.m_control_points.size(); i++)
		{
			controlPoints[i] = tempL.m_control_points[i];
			controlPoints_w[i] = tempL.m_weights[i];
		}

		for (; i < size; i++)
		{
			controlPoints[i] = tempR.m_control_points[i - tempL.m_control_points.size() + 1];
			controlPoints_w[i] = tempR.m_weights[i - tempL.m_control_points.size() + 1];
		}

		scalar kl = tempL.m_knots[tempL.m_knots.size() - 1];
		for (i = 0; i < degree + 1; i++)
		{
			knotVector[i] = tempL.m_knots[0];
			knotVector[ksize - i - 1] = kl + tempR.m_knots[tempR.m_knots.size() - 1];
		}

		for (int j = 0; j < degree; j++, i++)
		{
			knotVector[i] = kl;
		}

		auto map = GetInternalKnotMultiplicityMap(tempR.m_knots);
		for (auto it = map.begin(); it != map.end(); it++)
		{
			for (int j = 0; j < degree; j++, i++)
			{
				knotVector[i] = kl + it->first;
			}
		}

		result.m_degree = degree;
		result.m_knots = knotVector;
		result.m_control_points = controlPoints;
		result.m_weights = controlPoints_w;
		return true;
	}
	inline scalar GetCentripetalLength(const std::vector<vec3>& throughPoints)
	{
		int size = throughPoints.size();
		int n = size - 1;

		scalar length = 0.0;
		for (int i = 1; i <= n; i++)
		{
			length += std::sqrt((throughPoints[i] - throughPoints[i - 1]).norm());
		}
		return length;
	}
	inline std::vector<scalar> GetCentripetalParameterization(const std::vector<vec3>& throughPoints)
	{
		int size = throughPoints.size();
		int n = size - 1;

		std::vector<scalar> uk(size, 0.0);
		uk[n] = 1.0;

		scalar d = GetCentripetalLength(throughPoints);
		for (int i = 1; i <= n - 1; i++)
		{
			uk[i] = uk[i - 1] + std::sqrt((throughPoints[i] - throughPoints[i - 1]).norm()) / d;
		}
		return uk;
	}
	inline void CreateCubicHermite(const std::vector<vec3>& throughPoints, const std::vector<vec3>& tangents, RationalCurve& curve)
	{
		int n = throughPoints.size();

		vec3 startPoint = throughPoints[0];
		vec3 endPoint = throughPoints[throughPoints.size() - 1];

		std::vector<scalar> uk = GetCentripetalParameterization(throughPoints);

		bool isCyclePoint = is_almost_equal(startPoint, endPoint);
		vec3 startTangent = tangents[0];
		vec3 endTangent = tangents[tangents.size() - 1];
		bool isCycleTangent = is_almost_equal(startTangent, endTangent);

		int kn = 2 * n;
		int knotSize = kn + 3 + 1;
		std::vector<scalar> knotVector(knotSize);
		for (int i = 2, j = 0; i < kn + 2; i += 2, j++)
		{
			knotVector[i] = knotVector[i + 1] = uk[j];
		}

		if (isCyclePoint && isCycleTangent)
		{
			knotVector[0] = knotVector[1] = uk[0] - (uk[n - 1] - uk[n - 2]);
			knotVector[kn + 2] = knotVector[kn + 3] = uk[n - 1] + uk[1] - uk[0];
		}
		else if (isCyclePoint && !isCycleTangent)
		{
			knotVector[0] = uk[0] - (uk[n - 1] - uk[n - 2]);
			knotVector[1] = knotVector[2];
			knotVector[kn + 2] = knotVector[kn];
			knotVector[kn + 3] = uk[n - 1] + uk[1] - uk[0];
		}
		else
		{
			knotVector[0] = knotVector[1] = knotVector[2];
			knotVector[kn + 2] = knotVector[kn + 3] = knotVector[kn];
		}

		std::vector<vec3> controlPoints(kn);
		std::vector<scalar> controlPoints_w(kn);
		for (int j = 0; j < kn; j += 2)
		{
			scalar i1 = knotVector[j + 3] - knotVector[j + 1];
			scalar i2 = knotVector[j + 4] - knotVector[j + 2];

			controlPoints[j] = throughPoints[j] - i1 * tangents[j] / 3.0;
			controlPoints_w[j] = 1.0;
			controlPoints[j + 1] = throughPoints[j + 1] + i2 * tangents[j + 1] / 3.0;
			controlPoints_w[j + 1] = 1.0;
		}

		curve.m_degree = 3;
		curve.m_knots = knotVector;
		curve.m_control_points = controlPoints;
		curve.m_weights = controlPoints_w;
	}
	inline void GlobalInterpolation(int degree, const std::vector<vec3>& throughPoints, const std::vector<vec3>& tangents, scalar tangentFactor, RationalCurve& curve)
	{

		std::vector<vec3> unitTangents(tangents.size());
		for (int i = 0; i < tangents.size(); i++)
		{
			unitTangents[i] = tangents[i].normalized();
		}

		int size = throughPoints.size();
		int n = 2 * size;

		std::vector<vec3> controlPoints(n);
		std::vector<scalar> controlPoints_W(n);
		std::vector<scalar> knotVector(n + degree + 1);

		scalar d = get_total_chord_length(throughPoints);

		std::vector<scalar> uk = get_chord_parameterization(throughPoints);
		switch (degree)
		{
		case 2:
		{
			for (int i = 0; i <= degree; i++)
			{
				knotVector[i] = 0.0;
				knotVector[knotVector.size() - 1 - i] = 1.0;
			}
			for (int i = 0; i < size - 1; i++)
			{
				knotVector[2 * i + degree] = uk[i];
				knotVector[2 * i + degree + 1] = (uk[i] + uk[i + 1]) / 2.0;
			}
			break;
		}

		case 3:
		{
			for (int i = 0; i <= degree; i++)
			{
				knotVector[i] = 0.0;
				knotVector[knotVector.size() - 1 - i] = 1.0;
			}
			for (int i = 1; i < size - 1; i++)
			{
				knotVector[degree + 2 * i] = (2 * uk[i] + uk[i + 1]) / 3.0;
				knotVector[degree + 2 * i + 1] = (uk[i] + 2 * uk[i + 1]) / 3.0;
			}
			knotVector[4] = uk[1] / 2.0;
			knotVector[knotVector.size() - degree - 2] = (uk[size - 1] + 1.0) / 2.0;
			break;
		}
		default:
		{
			std::vector<scalar> uk2(2 * size);
			for (int i = 0; i < size - 1; i++)
			{
				uk2[2 * i] = uk[i];
				uk2[2 * i + 1] = (uk[i] + uk[i + 1]) / 2.0;
			}
			uk2[uk2.size() - 2] = (uk2[uk2.size() - 1] + uk2[uk2.size() - 3]) / 2.0;

			knotVector = average_knot_vector(degree, uk2);
		}
		}
		Eigen::Matrix<scalar, -1, -1> A = Eigen::Matrix<scalar, -1, -1>::Zero(n, n);

		for (int i = 1; i < size - 1; i++)
		{

			int spanIndex = find_span(degree, knotVector, uk[i]);

			auto basis = bspline_basis(degree, spanIndex, knotVector, uk[i]);
			std::vector<std::vector<scalar>> derBasis = bspline_der_basis(degree, spanIndex, knotVector, uk[i], 1);
			for (int j = 0; j <= degree; j++)
			{
				A(2 * i, spanIndex - degree + j) = basis[j];
				A(2 * i + 1, spanIndex - degree + j) = derBasis[1][j];
			}
		}
		A(0, 0) = 1.0;
		A(1, 0) = -1.0;
		A(1, 1) = 1.0;
		A(A.rows() - 2, A.cols() - 2) = -1.0;
		A(A.rows() - 2, A.cols() - 1) = 1.0;
		A(A.rows() - 1, A.cols() - 1) = 1.0;
		Eigen::Matrix<scalar, -1, -1> right = Eigen::Matrix<scalar, -1, -1>::Zero(n, 3);

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				right(2 * i, j) = throughPoints[i][j];
				right(2 * i + 1, j) = unitTangents[i][j] * d;
			}
		}

		scalar d0 = knotVector[degree + 1] / degree;
		scalar dn = (1 - knotVector[knotVector.size() - degree - 2]) / degree;

		vec3 dp0 = unitTangents[0];
		vec3 dpn = unitTangents[unitTangents.size() - 1];
		vec3 qpn = throughPoints[size - 1];
		for (int j = 0; j < 3; j++)
		{
			right(1, j) = d0 * dp0[j] * d;
			right(A.rows() - 2, j) = dn * dpn[j] * d;
			right(A.cols() - 1, j) = qpn[j];
		}

		Eigen::Matrix<scalar, -1, -1> result = A.lu().solve(right);
		for (int i = 0; i < result.size(); i++)
		{
			vec3 temp = vec3(0, 0, 0);
			for (int j = 0; j < 3; j++)
			{
				temp[j] = result(i, j);
			}
			controlPoints[i] = temp;
			controlPoints_W[i] = 1.0;
		}
		curve.m_degree = degree;
		curve.m_knots = knotVector;
		curve.m_control_points = controlPoints;
		curve.m_weights = controlPoints_W;
	}
	inline scalar Getak(const vec3& qk_1, const vec3& qk, const vec3& qk1, const vec3& qk2)
	{
		return (qk_1.cross(qk)).norm() / ((qk_1.cross(qk).norm()) + (qk1.cross(qk2)).norm());
	}
	inline vec3 Getqk(const std::vector<vec3>& throughPoints, int index)
	{
		return throughPoints[index] - throughPoints[index - 1];
	}
	inline vec3 GetTk(const vec3& qk_1, const vec3& qk, const vec3& qk1, const vec3& qk2)
	{
		scalar ak = Getak(qk_1, qk, qk1, qk2);
		return ((1 - ak) * qk + ak * qk1).normalized();
	}
	inline bool ComputeTangent(const std::vector<vec3>& throughPoints, std::vector<vec3>& tangents)
	{
		int size = throughPoints.size();
		if (size < 5) return false;
		tangents.resize(size);
		for (int k = 2; k < size - 2; k++)
		{
			vec3 qk_1 = Getqk(throughPoints, k - 1);
			vec3 qk = Getqk(throughPoints, k);
			vec3 qk1 = Getqk(throughPoints, k + 1);
			vec3 qk2 = Getqk(throughPoints, k + 2);

			tangents[k] = GetTk(qk_1, qk, qk1, qk2);
		}
		int n = size - 1;
		vec3 q0 = 2 * Getqk(throughPoints, 1) - Getqk(throughPoints, 2);
		vec3 q_1 = 2 * q0 - Getqk(throughPoints, 1);
		vec3 qn1 = 2 * Getqk(throughPoints, n) - Getqk(throughPoints, n - 1);
		vec3 qn2 = 2 * qn1 - Getqk(throughPoints, n);
		tangents[0] = GetTk(q_1, q0, Getqk(throughPoints, 1), Getqk(throughPoints, 2));
		tangents[1] = GetTk(q0, Getqk(throughPoints, 1), Getqk(throughPoints, 2), Getqk(throughPoints, 3));
		tangents[n - 1] = GetTk(Getqk(throughPoints, n - 2), Getqk(throughPoints, n - 1), Getqk(throughPoints, n), qn1);
		tangents[n] = GetTk(Getqk(throughPoints, n - 1), Getqk(throughPoints, n), qn1, qn2);
		return true;
	}
	inline bool CubicLocalInterpolation(const std::vector<vec3>& throughPoints, RationalCurve& curve)
	{
		int degree = 3;
		std::vector<vec3> tangents;
		bool hasTangents = ComputeTangent(throughPoints, tangents);
		if (!hasTangents) return false;
		int size = throughPoints.size();
		std::vector<scalar> uk(size);
		int n = size - 1;
		uk[0] = 0;
		std::vector<vec3> tempControlPoints;
		for (int k = 0; k < n; k++)
		{
			vec3 t0 = tangents[k];
			vec3 t3 = tangents[k + 1];
			vec3 p0 = throughPoints[k];
			vec3 p3 = throughPoints[k + 1];
			scalar a = 16 - (t0 + t3).squaredNorm();
			scalar b = 12 * (p3 - p0).dot(t0 + t3);
			scalar c = -36 * (p3 - p0).squaredNorm();
			scalar alpha = (-b + std::sqrt(b * b - 4 * a * c)) / (2 * a);
			vec3 pk0 = throughPoints[k];
			vec3 pk1 = p0 + (1 / 3) * alpha * t0;
			vec3 pk2 = p3 - (1 / 3) * alpha * t3;
			uk[k + 1] = uk[k] + 3 * (pk1 - pk0).norm();
			tempControlPoints.emplace_back(pk1);
			tempControlPoints.emplace_back(pk2);
		}
		int kvSize = 2 * (degree + 1) + 2 * (size - 1);
		std::vector<scalar> knotVector(kvSize);
		for (int i = 0; i <= degree; i++)
		{
			knotVector[i] = 0;
			knotVector[kvSize - 1 - i] = 1;
		}

		for (int i = 1; i <= n - 1; i = i + 2)
		{
			knotVector[degree + i] = knotVector[degree + (i + 1)] = uk[i] / uk[n];
		}

		int tSize = tempControlPoints.size();
		std::vector<vec3> controlPoints(tSize + 2);
		std::vector<scalar> controlPoints_W(tSize + 2, 1.0);
		controlPoints[0] = throughPoints[0];
		controlPoints[tSize + 2 - 1] = throughPoints[n];
		for (int i = 0; i < tSize; i++)
		{
			controlPoints[i + 1] = tempControlPoints[i];
		}
		curve.m_degree = degree;
		curve.m_knots = knotVector;
		curve.m_control_points = controlPoints;
		curve.m_weights = controlPoints_W;
		return true;
	}
	inline void Bending(const RationalCurve& curve, scalar startParameter, scalar endParameter, vec3 bendCenter, scalar radius, scalar crossRatio, RationalCurve& result)
	{
		int degree = curve.m_degree;
		std::vector<scalar> knotVector = curve.m_knots;
		std::vector<vec3> controlPoints = curve.m_control_points;
		std::vector<scalar> controlPoints_W = curve.m_weights;


		int spanMinIndex = find_span(degree, knotVector, startParameter);
		int spanMaxIndex = find_span(degree, knotVector, endParameter);

		std::vector<vec3> updatedControlPoints = controlPoints;
		std::vector<scalar> updatedControlPoints_W = controlPoints_W;

		std::map<int, vec3> selectedControlPoints;
		for (int i = spanMinIndex; i <= spanMaxIndex - degree - 1; i++)
		{
			vec3 p = updatedControlPoints[i] / updatedControlPoints_W[i];
			selectedControlPoints.insert({ i, p });
		}

		for (auto it = selectedControlPoints.begin(); it != selectedControlPoints.end(); ++it)
		{
			int index = it->first;
			vec3 current = it->second;
			vec3 pointOnBendCurve = bendCenter + (current - bendCenter).normalized() * radius;
			scalar si = (bendCenter - pointOnBendCurve).norm() / (bendCenter - current).norm();
			scalar ti = (crossRatio * si) / (1 + (crossRatio - 1) * si);
			vec3 project = bendCenter + (current - bendCenter) * ti;
			updatedControlPoints[index] = project;
		}

		result.m_degree = degree;
		result.m_knots = knotVector;
		result.m_control_points = updatedControlPoints;
		result.m_weights = updatedControlPoints_W;
	}

}//
