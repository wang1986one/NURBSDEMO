#pragma once
#include<vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
namespace Geomerty
{
	namespace nurbs
	{
		using scalar = float;
		using vec3 = Eigen::Vector3<scalar>;
		using vec4 = Eigen::Vector4<scalar>;
		/**
		Struct for holding a rational B-spline curve
		@tparam T Data type of control points and knots weights
		*/
		struct RationalCurve
		{
			size_t m_degree;
			std::vector<scalar> m_knots;
			std::vector<vec3> m_control_points;
			std::vector<scalar> m_weights;

			RationalCurve() = default;
			RationalCurve(size_t degree, const std::vector<scalar>& knots, const std::vector<vec3>& control_points, const std::vector<scalar> weights)
				: m_degree(degree), m_knots(knots), m_control_points(control_points), m_weights(weights)
			{
			}
		};
		/**
		Struct for holding a B-spline curve
		@tparam T Data type of control points and knots
		*/
		struct Curve : public RationalCurve
		{
			Curve() = default;

			Curve(size_t degree, const std::vector<scalar>& knots, const std::vector<vec3>& control_points)
				: RationalCurve(degree, knots, control_points, std::vector<scalar>(m_control_points.size(), 1.0))
			{
			}
		};
	}// namespace nurbs
}// namespace nous