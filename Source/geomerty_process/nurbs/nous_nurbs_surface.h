#pragma once

// #include "nous_nurbs_array2.h"
namespace Geomerty
{
	namespace nurbs
	{

		/**
		Struct for representing a NURBS surface
		\tparam T Data type of control points and weights
		*/
		struct Surface
		{
			size_t m_degree_u, m_degree_v;
			std::vector<float> m_knots_u, m_knots_v;
			std::vector<std::vector<vec3>> m_control_points;

			Surface() = default;

			Surface(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u, const std::vector<float>& knots_v,
				std::vector<std::vector<vec3>> control_points)
				: m_degree_u(degree_u), m_degree_v(degree_v), m_knots_u(knots_u), m_knots_v(knots_v), m_control_points(control_points)
			{
			}
		};

		/**
		Struct for representing a non-rational NURBS surface
		\tparam T Data type of control points and weights
		*/
		struct RationalSurface
		{
			size_t m_degree_u, m_degree_v;
			std::vector<float> m_knots_u, m_knots_v;
			std::vector<std::vector<vec3>> m_control_points;
			std::vector<std::vector<float>> m_weights;

			RationalSurface() = default;
			RationalSurface(const Surface& srf, const std::vector<std::vector<float>>& weights)
				: m_degree_u(srf.m_degree_u), m_degree_v(srf.m_degree_v), m_knots_u(srf.m_knots_u), m_knots_v(srf.m_knots_v),
				m_control_points(srf.m_control_points), m_weights(weights)
			{
			}

			RationalSurface(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u, const std::vector<float>& knots_v,
				const std::vector<std::vector<vec3>>& control_points, const std::vector<std::vector<float>>& weights)
				: m_degree_u(degree_u), m_degree_v(degree_v), m_knots_u(knots_u), m_knots_v(knots_v), m_control_points(control_points), m_weights(weights)
			{
			}
		};
	}// namespace nurbs
}// namespace nous