#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "nurbs_curve.h"
#include "nurbs_surface.h"
#include "nurbs/nurbs_evalute_curve.h"
#include "nurbs_util.h"

namespace Geomerty::nurbs::util
{




	//  make nurbs shapes
	inline RationalCurve rational_bezier_curve(const std::vector<vec3>& control_points, std::vector<scalar> weights)
	{
		size_t n = control_points.size();
		if (weights.empty())
		{
			weights = std::vector<scalar>(n, 1.0);
		}
		else
		{
			assert(n == weights.size());
		}
		size_t degree = n - 1;
		std::vector<scalar> knots(n + degree + 1, 0.0);
		for (int i = n; i < knots.size(); i++)
		{
			knots[i] = 1.0;
		}
		RationalCurve crv;
		crv.m_degree = degree;
		crv.m_knots = knots;
		crv.m_control_points = control_points;
		crv.m_weights = weights;
		return crv;
	}
	inline RationalCurve rational_polyline_curve(const std::vector<vec3>& control_points)
	{
		size_t n = control_points.size();
		size_t degree = 1;
		std::vector<scalar> knots(n + degree + 1, 1.0);
		for (int i = 0; i < degree + 1; i++)
		{
			knots[i] = 0.0;
		}
		scalar length = get_total_chord_length(control_points);
		scalar addsum = 0.0f;
		for (int i = 1; i < control_points.size() - 1; i++)
		{
			addsum += (control_points[i] - control_points[i - 1]).norm();
			knots[i + degree + 1] = addsum / length;
		}

		RationalCurve crv;
		crv.m_degree = degree;
		crv.m_knots = knots;
		crv.m_control_points = control_points;
		crv.m_weights = std::vector<scalar>(n, 1.0);
		return crv;
	}
	inline RationalCurve rational_ellipse_arc_curve(const vec3& center, vec3 xaxis, vec3 yaxis, scalar start_angle, scalar end_angle)
	{
		scalar xradius = xaxis.norm();
		scalar yradius = yaxis.norm();
		xaxis = xaxis.normalized();
		yaxis = yaxis.normalized();
		if (end_angle < start_angle)
		{
			end_angle = start_angle + static_cast<scalar>(N_FLOAT_PI);
		}
		scalar theta = end_angle - start_angle;
		int num_arcs = 0;
		if (theta <= (N_FLOAT_PI / 2))
		{
			num_arcs = 1;
		}
		else
		{
			if ((theta <= N_FLOAT_PI))
			{
				num_arcs = 2;
			}
			else
			{
				if (theta <= (N_FLOAT_PI * 1.5))
				{
					num_arcs = 3;
				}
				else
				{
					num_arcs = 4;
				}
			}
		}

		scalar dtheta = (theta / num_arcs);
		int n = 2 * num_arcs;
		scalar w1 = cos(dtheta / 2);
		vec3 P0 = xradius * cos(start_angle) * xaxis;
		vec3 P01 = center + P0 + yradius * sin(start_angle) * yaxis;
		vec3 T0 = cos(start_angle) * yaxis;
		vec3 T01 = T0 - sin(start_angle) * xaxis;
		std::vector<vec3> control_points;
		control_points.resize(num_arcs * 2 + 1);
		std::vector<scalar> knots(2 * num_arcs + 4, 0.0);
		int index = 0.0;
		scalar angle = start_angle;
		std::vector<scalar> weights(num_arcs * 2 + 1, 0.0);
		control_points[0] = P01;
		weights[0] = 1.0;

		int _g = 1;
		int _g1 = (num_arcs + 1);
		while ((_g < _g1))
		{
			_g = (_g + 1);
			int i = (_g - 1);
			angle = (angle + dtheta);
			vec3 P2 = xradius * cos(angle) * xaxis;
			vec3 P21 = center + P2 + yradius * sin(angle) * yaxis;
			weights[(index + 2)] = 1.0f;
			control_points[(index + 2)] = P21;
			vec3 T2 = cos(angle) * yaxis;
			vec3 T21 = T2 - sin(angle) * xaxis;
			vec3 inters = T01.normalized();
			scalar t;
			{
				vec3 a0 = P01;
				vec3 a = inters;
				vec3 b0 = P21;
				vec3 b = T21.normalized();
				scalar dab = a.dot(b);
				scalar dab0 = a.dot(b0);
				scalar daa0 = a.dot(a0);
				scalar dbb0 = b.dot(b0);
				scalar dba0 = b.dot(a0);
				scalar daa = a.dot(a);
				scalar dbb = b.dot(b);
				scalar div = ((daa * dbb) - (dab * dab));
				if (div < N_SCALAR_EPSILON)
				{
					return RationalCurve();
				}
				scalar num = ((dab * (dab0 - daa0)) - (daa * (dbb0 - dba0)));
				scalar w = (num / div);
				t = (((dab0 - daa0) + (w * dab)) / daa);
			}
			vec3 P1 = P01 + t * T01;

			weights[(index + 1)] = w1;
			control_points[(index + 1)] = P1;
			index = (index + 2);
			if ((i < num_arcs))
			{
				P01 = P21;
				T01 = T21;
			}
		}

		int j = ((2 * num_arcs) + 1);
		knots[0] = 0.0;
		knots[j] = 1.0;
		knots[1] = 0.0;
		knots[(1 + j)] = 1.0;
		knots[2] = 0.0;
		knots[(2 + j)] = 1.0;
		switch (num_arcs)
		{
		case 2: {
			knots[3] = (knots[4] = (0.5));
		}
			  break;
		case 3: {
			knots[3] = (knots[4] = (0.33333333333333331));
			knots[5] = (knots[6] = (0.66666666666666663));
		}
			  break;
		case 4: {
			knots[3] = (knots[4] = (0.25));
			knots[5] = (knots[6] = (0.5));
			knots[7] = (knots[8] = (0.75));
		}
			  break;
		}

		RationalCurve crv;
		crv.m_degree = 2;
		crv.m_knots = knots;
		crv.m_control_points = control_points;
		crv.m_weights = weights;
		return crv;
	}
	inline void translational_sweep_surface(const RationalCurve& profile, const RationalCurve& rail, RationalSurface& srf)
	{
		int n = profile.m_control_points.size() - 1;
		int m = rail.m_control_points.size() - 1;
		std::vector<std::vector<vec3>> controlPoints(n + 1, std::vector<vec3>(m + 1));
		std::vector<std::vector<scalar>> weights(n + 1, std::vector<float>(m + 1));
		for (int i = 0; i <= n; i++)
		{
			for (int j = 0; j <= m; j++)
			{
				controlPoints[i][j] = rail.m_control_points[j] / rail.m_weights[j] + profile.m_control_points[i] / profile.m_weights[i];
				weights[i][j] = profile.m_weights[i] * rail.m_weights[j];
			}
		}

		srf.m_degree_u = profile.m_degree;
		srf.m_degree_v = rail.m_degree;
		srf.m_knots_u = profile.m_knots;
		srf.m_knots_v = rail.m_knots;
		srf.m_control_points = controlPoints;
		srf.m_weights = weights;
	}
	inline void loft_surface(const std::vector<RationalCurve>& sections, RationalSurface& srf, int custom_trajectory_degree = 0,
		const std::vector<scalar>& custom_trajectory_knot_vector = {})
	{

		int degree_max = 0;
		for (int k = 0; k < sections.size(); k++)
		{
			RationalCurve current = sections[k];
			if (degree_max < current.m_degree)
			{
				degree_max = current.m_degree;
			}
		}

		int size = sections.size();

		std::vector<RationalCurve> internals(size);
		std::vector<std::vector<vec3>> curves_control_points(size);
		std::vector<std::vector<float>> curves_weight(size);
		for (int k = 0; k < size; k++)
		{
			RationalCurve current = sections[k];
			if (degree_max > current.m_degree)
			{
				int times = degree_max - current.m_degree;
				RationalCurve tc;
				elevate_degree(current, times, tc);
				current.m_degree = degree_max;
				current.m_knots = tc.m_knots;
				current.m_control_points = tc.m_control_points;
				current.m_weights = tc.m_weights;
			}
			curves_control_points[k] = current.m_control_points;
			curves_weight[k] = current.m_weights;
			internals[k] = current;
		}

		int degree_u = degree_max;
		int degree_v = degree_max;

		std::vector<scalar> vl(size);
		if (custom_trajectory_knot_vector.size() == 0)
		{
			int K = size - 1;
			vl[0] = 0;
			vl[K] = 1;
			for (int k = 1; k <= K - 1; k++)
			{

				std::vector<vec3> current = curves_control_points[k];
				int n = current.size() - 1;
				scalar sum = 0.0;
				for (int i = 0; i <= n; i++)
				{
					scalar delta = (curves_control_points[k][i] / curves_weight[k][i] - curves_control_points[k - 1][i] / curves_weight[k - 1][i]).norm();

					std::vector<vec3> tempPoints(size);
					for (int j = 0; j <= K; j++)
					{
						tempPoints[j] = curves_control_points[j][i] / curves_weight[j][i];
					}

					scalar di = get_total_chord_length(tempPoints);
					sum += delta / di;
				}

				vl[k] = vl[k - 1] + (1.0 / (n + 1)) * sum;
			}
		}
		else
		{
			degree_v = custom_trajectory_degree;
			vl = custom_trajectory_knot_vector;
		}
		std::vector<scalar> knot_vector_v = average_knot_vector(degree_v, vl);
		std::vector<std::vector<vec3>> control_points;
		std::vector<std::vector<scalar>> control_points_weight;
		int column = curves_control_points[0].size();
		for (int c = 0; c < column; c++)
		{
			std::vector<vec3> temp(size);
			std::vector<scalar> weight_cache(size);
			for (int k = 0; k < size; k++)
			{
				vec3 cp = curves_control_points[k][c];
				scalar w = curves_weight[k][c];
				weight_cache[k] = w;
				temp[k] = cp / w;
			}
			RationalCurve tc;
			global_interpolation(degree_v, temp, tc, vl);
			for (int i = 0; i < tc.m_control_points.size(); i++)
			{
				vec3 cp = tc.m_control_points[i];
				scalar w = tc.m_weights[i];
				tc.m_control_points[i] = cp / w;
				tc.m_weights[i] = weight_cache[i];
			}
			control_points.emplace_back(tc.m_control_points);
			control_points_weight.emplace_back(tc.m_weights);
		}

		srf.m_degree_u = degree_u;
		srf.m_degree_v = degree_v;
		srf.m_knots_u = internals[0].m_knots;
		srf.m_knots_v = knot_vector_v;
		srf.m_control_points = control_points;
		srf.m_weights = control_points_weight;
	}

}// namespace nous::nurbs::util
