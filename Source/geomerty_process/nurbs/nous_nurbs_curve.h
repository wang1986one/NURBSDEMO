#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>


namespace Geomerty
{
	using vec3 = Eigen::Vector3<float>;
	using vec4 = Eigen::Vector4<float>;
	namespace nurbs
	{
		/**
		Struct for holding a B-spline curve
		@tparam T Data type of control points and knots
		*/
		struct Curve
		{
			size_t m_degree;
			std::vector<float> m_knots;
			std::vector<vec3> m_control_points;

			Curve() = default;

			Curve(size_t degree, const std::vector<float>& knots, const std::vector<vec3>& control_points)
				: m_degree(degree), m_knots(knots), m_control_points(control_points)
			{
			}
		};
		/**
		Struct for holding a rational B-spline curve
		@tparam T Data type of control points and knots
		*/
		struct RationalCurve
		{
			size_t m_degree;
			std::vector<float> m_knots;
			std::vector<vec3> m_control_points;
			std::vector<float> m_weights;

			RationalCurve() = default;
			RationalCurve(const Curve& crv) : RationalCurve(crv, std::vector<float>(crv.m_control_points.size(), 1.0)) {}
			RationalCurve(const Curve& crv, const std::vector<float>& weights) : RationalCurve(crv.m_degree, crv.m_knots, crv.m_control_points, weights) {}
			RationalCurve(size_t degree, const std::vector<float>& knots, const std::vector<vec3>& control_points, const std::vector<float> weights)
				: m_degree(degree), m_knots(knots), m_control_points(control_points), m_weights(weights)
			{
			}
		};
	}// namespace nurbs
}// namespace nous