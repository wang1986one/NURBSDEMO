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
		Struct for representing a non-rational NURBS surface
		\tparam T Data type of control points and weights
		*/
		struct RationalSurface
		{
			size_t m_degree_u, m_degree_v;
			std::vector<scalar> m_knots_u, m_knots_v;
			std::vector<std::vector<vec3>> m_control_points;
			std::vector<std::vector<scalar>> m_weights;

			RationalSurface() = default;

			RationalSurface(size_t degree_u, size_t degree_v, const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v,
				const std::vector<std::vector<vec3>>& control_points, const std::vector<std::vector<scalar>>& weights)
				: m_degree_u(degree_u), m_degree_v(degree_v), m_knots_u(knots_u), m_knots_v(knots_v), m_control_points(control_points), m_weights(weights)
			{
			}
		};
		/**
		Struct for representing a B spline surface
		*/
		struct Surface : public RationalSurface
		{
			Surface() = default;
			Surface(size_t degree_u, size_t degree_v, const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v, std::vector<std::vector<vec3>> control_points)
				: RationalSurface(degree_u, degree_v, knots_u, knots_v, control_points,
					std::vector<std::vector<scalar>>(m_control_points.size(), std::vector<scalar>(m_control_points[0].size(), 1.0)))
			{
			}
		};
	}// namespace nurbs
}// namespace nous