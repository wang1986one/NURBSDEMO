#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include<tuple>
#include "nous_evalute_nurbs_curve.h"
#include "nous_nurbs_curve.h"
#include "nous_nurbs_surface.h"
#include "nous_nurbs_util.h"

namespace nous
{
	namespace nurbs
	{
		namespace util
		{
			using scalar = float;
			/**
			 * Returns whether the surface is valid
			 * @tparam T Type of control point coordinates, knot values
			 * @param[in] degree_u Degree of surface along u-direction
			 * @param[in] degree_v Degree of surface along v-direction
			 * @param[in] knots_u Knot vector of surface along u-direction
			 * @param[in] knots_v Knot vector of surface along v-direction
			 * @param[in] control_points Control points grid of surface
			 * @return Whether valid
			 */
			inline bool surface_is_valid(const size_t degree_u, const size_t degree_v, const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v,
				const std::vector<std::vector<vec3>>& control_points)
			{
				if (degree_u < 1 || degree_u > 9 || degree_v < 1 || degree_v > 9)
				{
					return false;
				}
				if (!is_valid_relation(degree_u, knots_u.size(), control_points.size()) || !is_valid_relation(degree_v, knots_v.size(), control_points[0].size()))
				{
					return false;
				}

				return true;
			}
			/**
			 * Returns whether the rational surface is valid
			 * @tparam T Type of control point coordinates, knot values
			 * @param[in] srf RationalSurface object
			 * @return Whether valid
			 */
			inline bool surface_is_valid(const RationalSurface& srf)
			{
				if (srf.m_control_points.size() != srf.m_weights.size() || srf.m_control_points[0].size() != srf.m_weights[0].size())
					return false;
				return surface_is_valid(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, srf.m_control_points);
			}

			/**
			 * Evaluate parameterization for throughpoints on a surfacemesh
			 * @param[in] throughpoints points on a surfacemesh.
			 * @param[out] params_u in u-direction.
			 * @param[out] params_v in v-direction..
			 * @return parameterization result.
			 */
			inline bool get_surfacemesh_parameterization(const std::vector<std::vector<vec3>>& throughpoints, std::vector<scalar>& params_u, std::vector<scalar>& params_v)
			{

				int n = throughpoints.size();
				int m = throughpoints[0].size();

				std::vector<scalar> cds(std::max(n, m), 0.0);
				params_u.resize(n, 0.0);
				params_v.resize(m, 0.0);

				int num = m;

				for (int l = 0; l < m; l++)
				{
					scalar total = 0.0;
					for (int k = 1; k < n; k++)
					{
						cds[k] = (throughpoints[k][l] - throughpoints[k - 1][l]).norm();
						total += cds[k];
					}

					if (is_almost_equal(total, 0.0))
					{
						num--;
					}
					else
					{
						scalar d = 0.0;
						for (int k = 1; k < n; k++)
						{
							d += cds[k];
							params_u[k] = params_u[k] + d / total;
						}
					}
				}
				if (num == 0)
				{
					return false;
				}

				for (int k = 1; k < n - 1; k++)
				{
					params_u[k] = params_u[k] / num;
				}
				params_u[n - 1] = 1.0;

				num = n;

				for (int k = 0; k < n; k++)
				{
					scalar total = 0.0;
					for (int l = 1; l < m; l++)
					{
						cds[l] = (throughpoints[k][l] - throughpoints[k][l - 1]).norm();
						total += cds[l];
					}
					if (is_almost_equal(total, 0.0))
					{
						num--;
					}
					else
					{
						scalar d = 0.0;
						for (int l = 1; l < m; l++)
						{
							d += cds[l];
							params_v[l] += d / total;
						}
					}
				}

				if (num == 0)
				{
					return false;
				}
				for (int l = 1; l < m - 1; l++)
				{
					params_v[l] = params_v[l] / num;
				}
				params_v[m - 1] = 1.0;

				return true;
			}

			/**
			 * Evaluate point on a nonrational NURBS surface
			 * @param[in] degree_u Degree of the given surface in u-direction.
			 * @param[in] degree_v Degree of the given surface in v-direction.
			 * @param[in] knots_u Knot vector of the surface in u-direction.
			 * @param[in] knots_v Knot vector of the surface in v-direction.
			 * @param[in] control_points Control points of the surface in a 2d array.
			 * @param[in] u Parameter to evaluate the surface at.
			 * @param[in] v Parameter to evaluate the surface at.
			 * @return point Resulting point on the surface at (u, v).
			 */
			template<typename T>
			inline T surface_point(const size_t degree_u, const size_t degree_v, const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v,
				const std::vector<std::vector<T>>& control_points, scalar u, scalar v)
			{
				// Initialize result to 0s
				if constexpr (is_eigen_mat_v<T>)
				{
					T point = T::Zero();
					// Find span and non-zero basis functions
					int span_u = find_span(degree_u, knots_u, u);
					int span_v = find_span(degree_v, knots_v, v);
					std::vector<scalar> nu = bspline_basis(degree_u, span_u, knots_u, u);
					std::vector<scalar> nv = bspline_basis(degree_v, span_v, knots_v, v);

					for (auto l : IndexRange(degree_v + 1))
					{
						T temp = T::Zero();
						for (auto k : IndexRange(degree_u + 1))
						{
							temp += nu[k] * control_points[span_u - degree_u + k][span_v - degree_v + l];
						}

						point += nv[l] * temp;
					}
					return point;
				}
				return T();
			}

			/**
			 * Evaluate derivatives on a non-rational NURBS surface
			 * @param[in] degree_u Degree of the given surface in u-direction.
			 * @param[in] degree_v Degree of the given surface in v-direction.
			 * @param[in] knots_u Knot vector of the surface in u-direction.
			 * @param[in] knots_v Knot vector of the surface in v-direction.
			 * @param[in] control_points Control points of the surface in a 2D array.
			 * @param[in] num_ders Number of times to differentiate
			 * @param[in] u Parameter to evaluate the surface at.
			 * @param[in] v Parameter to evaluate the surface at.
			 * @param[out] surf_ders Derivatives of the surface at (u, v).
			 */
			template<typename T>
			inline std::vector<std::vector<T>> surface_derivatives(const size_t degree_u, const size_t degree_v, const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v,
				const std::vector<std::vector<T>>& control_points, size_t num_ders, scalar u, scalar v)
			{
				if constexpr (is_eigen_mat_v<T>)
				{
					std::vector<std::vector<T>> surf_ders(num_ders + 1, std::vector<T>(num_ders + 1, T::Zero()));

					// Find span and basis function derivatives
					scalar span_u = find_span(degree_u, knots_u, u);
					scalar span_v = find_span(degree_v, knots_v, v);
					std::vector<std::vector<scalar>> ders_u = bspline_der_basis(degree_u, span_u, knots_u, u, num_ders);
					std::vector<std::vector<scalar>> ders_v = bspline_der_basis(degree_v, span_v, knots_v, v, num_ders);

					// Number of non-zero derivatives is <= degree
					size_t du = num_ders > degree_u ? degree_u : num_ders;

					size_t dv = num_ders > degree_v ? degree_v : num_ders;

					// Compute derivatives
					for (auto k : IndexRange(du + 1))
					{
						std::vector<T> temp;
						temp.resize(degree_v + 1, T::Zero());
						for (auto s : IndexRange(degree_v + 1))
						{
							for (auto r : IndexRange(degree_u + 1))
							{
								temp[s] += ders_u[k][r] * control_points[span_u - degree_u + r][span_v - degree_v + s];
							}
						}

						int dd = (num_ders - k) > dv ? dv : num_ders - k;

						for (auto l : IndexRange(dd + 1))
						{
							for (auto s : IndexRange(degree_v + 1))
							{
								surf_ders[k][l] += ders_v[l][s] * temp[s];
							}
						}
					}
					return surf_ders;
				}
				return std::vector<std::vector<T>>{};
			}

			/**
			 * Evaluate point on a non-rational NURBS surface
			 * @param[in] srf RationalSurface object
			 * @param[in] u Parameter to evaluate the surface at.
			 * @param[in] v Parameter to evaluate the surface at.
			 * @return Resulting point on the surface at (u, v).
			 */
			inline vec3 surface_point(const RationalSurface& srf, scalar u, scalar v)
			{
				if (!surface_is_valid(srf))
					return vec3::Zero();
				// Compute homogenous coordinates of control points
				std::vector<std::vector<vec4>> cw(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); i++)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); j++)
					{
						cw[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}
				// Compute point using homogenous coordinates
				vec4 point_w = surface_point(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, cw, u, v);
				// Convert back to cartesian coordinates
				return homogenous_to_cartesian(point_w);
			}

			/**
			 * Evaluate derivatives on a non-rational NURBS surface
			 * @param[in] degree_u Degree of the given surface in u-direction.
			 * @param[in] degree_v Degree of the given surface in v-direction.
			 * @param[in] knots_u Knot vector of the surface in u-direction.
			 * @param[in] knots_v Knot vector of the surface in v-direction.
			 * @param[in] control_points Control points of the surface in a 2D array.
			 * @param[in] num_ders Number of times to differentiate
			 * @param[in] u Parameter to evaluate the surface at.
			 * @param[in] v Parameter to evaluate the surface at.
			 * @return surf_ders Derivatives of the surface at (u, v).
			 */
			inline std::vector<std::vector<vec3>> surface_derivatives(const Surface& srf, int num_ders, scalar u, scalar v)
			{
				if (!surface_is_valid(srf))
				{
					return std::vector<std::vector<vec3>>{};
				}
				return surface_derivatives(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, srf.m_control_points, num_ders, u, v);
			}

			/**
			 * Evaluate derivatives on a rational NURBS surface
			 * @param[in] srf RationalSurface object
			 * @param[in] u Parameter to evaluate the surface at.
			 * @param[in] v Parameter to evaluate the surface at.
			 * @param[in] num_ders Number of times to differentiate
			 * @return Derivatives on the surface at parameter (u, v).
			 */
			inline std::vector<std::vector<vec3>> surface_derivatives(const RationalSurface& srf, int num_ders, scalar u, scalar v)
			{
				if (!surface_is_valid(srf))
				{
					return std::vector<std::vector<vec3>>{};
				}
				std::vector<std::vector<vec4>> homo_cp(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); i++)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); j++)
					{
						homo_cp[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}

				std::vector<std::vector<vec4>> homo_ders = surface_derivatives(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, homo_cp, num_ders, u, v);

				std::vector<std::vector<vec3>> aders(num_ders + 1, std::vector<vec3>(num_ders + 1));

				for (int i = 0; i < homo_ders.size(); i++)
				{
					for (int j = 0; j < homo_ders[0].size(); j++)
					{
						aders[i][j] = homo_ders[i][j].head<3>();
					}
				}

				std::vector<std::vector<vec3>> surf_ders(num_ders + 1, std::vector<vec3>(num_ders + 1));
				for (int k = 0; k < num_ders + 1; k++)
				{
					for (int l = 0; l < num_ders - k + 1; l++)
					{
						vec3 der = aders[k][l];

						for (int j = 1; j < l + 1; j++)
						{
							der -= binomial(l, j) * homo_ders[0][j].w() * surf_ders[k][l - j];
						}

						for (int i = 1; i < k + 1; i++)
						{
							der -= binomial(k, i) * homo_ders[i][0].w() * surf_ders[k - i][l];

							vec3 tmp = vec3::Zero();
							for (int j = 0; j < l + 1; j++)
							{
								tmp -= binomial(l, j) * homo_ders[i][j].w() * surf_ders[k - 1][l - j];
							}

							der -= binomial(k, i) * tmp;
						}

						der *= 1 / homo_ders[0][0].w();
						surf_ders[k][l] = der;
					}
				}
				return surf_ders;
			}

			/**
			 * Evaluate the two orthogonal tangents of a rational surface at the given
			 * parameters
			 * @param[in] srf Rational Surface object
			 * @param u Parameter in the u-direction
			 * @param v Parameter in the v-direction
			 * @return std::tuple with unit tangents along u- and v-directions
			 */
			inline std::tuple<vec3, vec3> surface_tangent(const RationalSurface& srf, scalar u, scalar v)
			{
				if (!surface_is_valid(srf))
				{
					return {};
				}
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 du = ptder[1][0];
				vec3 dv = ptder[0][1];
				scalar du_len = ptder[1][0].norm();
				scalar dv_len = ptder[0][1].norm();

				if (!close(du_len, 0.0f))
				{
					du /= du_len;
				}
				if (!close(dv_len, 0.0f))
				{
					dv /= dv_len;
				}
				return { du, dv };
			}

			/**
			 * Evaluate the normal of a rational surface at the given parameters
			 * @param[in] srf Rational Surface object
			 * @param u Parameter in the u-direction
			 * @param v Parameter in the v-direction
			 * @return Unit normal at of the surface at (u, v)
			 */
			inline vec3 surface_normal(const RationalSurface& srf, scalar u, scalar v)
			{
				if (!surface_is_valid(srf))
				{
					return vec3::Zero();
				}
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 n = ptder[0][1].cross(ptder[1][0]);
				scalar n_len = n.norm();
				if (!close(n_len, 0.0f))
				{
					n /= n_len;
				}
				return n;
			}
			/**
			 * Insert knots in the surface along one direction
			 * @param[in] degree Degree of the surface along which to insert knot
			 * @param[in] knots Knot vector
			 * @param[in] cp 2D array of control points
			 * @param[in] knot Knot value to insert
			 * @param[in] r Number of times to insert
			 * @param[in] along_u Whether inserting along u-direction
			 * @param[out] new_knots Updated knot vector
			 * @param[out] new_cp Updated control points
			 */
			template<typename T>
			inline void surface_knot_insert(size_t degree, const std::vector<scalar>& knots, const std::vector<std::vector<T>>& cp, scalar knot, size_t r, bool along_u,
				std::vector<scalar>& new_knots, std::vector<std::vector<T>>& new_cp)
			{

				int span = find_span(degree, knots, knot);
				size_t s = knot_multiplicity(knots, knot);
				// Knot multiplicity cannot be greater than degree
				if (s > degree)
				{
					return;
				}
				if (s == degree)
				{
					new_cp = cp;
					new_knots = knots;
					return;
				}
				if ((r + s) > degree)
				{
					r = degree - s;
				}

				// Create a new knot vector
				new_knots.resize(knots.size() + r);
				for (auto i : IndexRange(span + 1))
				{
					new_knots[i] = knots[i];
				}
				for (auto i : IndexRange<size_t>(1, r + 1))
				{
					new_knots[span + i] = knot;
				}
				for (auto i : IndexRange<size_t>(span + 1, knots.size()))
				{
					new_knots[i + r] = knots[i];
				}
				// Compute alpha
				std::vector<std::vector<scalar>> alpha(degree - s, std::vector<scalar>(r + 1, 0.0f));
				for (auto j : IndexRange<size_t>(1, r + 1))
				{
					int L = span - degree + j;
					for (auto i : IndexRange(degree - j - s + 1))
					{
						alpha[i][j] = (knot - knots[L + i]) / (knots[i + span + 1] - knots[L + i]);
					}
				}

				// Create a temporary container for affected control points per row/column
				std::vector<T> tmp(degree + 1);

				if (along_u)
				{

					// Create new control points with additional rows
					new_cp = std::vector<std::vector<T>>(cp.size() + r, std::vector<T>(cp[0].size()));

					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (auto col : IndexRange(cp[0].size()))
					{

						// Copy unaffected control points
						for (auto i : IndexRange(span - degree + 1))
						{
							new_cp[i][col] = cp[i][col];
						}
						for (auto i : IndexRange(span - s, cp.size()))
						{
							new_cp[i + r][col] = cp[i][col];
						}
						// Copy affected control points to temp array
						for (auto i : IndexRange(degree - s + 1))
						{
							tmp[i] = cp[span - degree + i][col];
						}
						// Insert knot
						for (auto j : IndexRange<size_t>(1, r + 1))
						{
							int L = span - degree + j;
							for (auto i : IndexRange(degree - j - s + 1))
							{
								scalar a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[L][col] = tmp[0];
							new_cp[span + r - j - s][col] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (auto i : IndexRange<size_t>(L + 1, span - s))
						{
							new_cp[i][col] = tmp[i - L];
						}
					}
				}
				else
				{
					// Create new control points with additional columns
					new_cp = std::vector<std::vector<T>>(cp.size(), std::vector<T>(cp[0].size() + r));
					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (auto row : IndexRange(cp.size()))
					{
						// Copy unaffected control points
						for (auto i : IndexRange(span - degree + 1))
						{
							new_cp[row][i] = cp[row][i];
						}
						for (auto i : IndexRange(span - s, cp[0].size()))
						{
							new_cp[row][i + r] = cp[row][i];
						}
						// Copy affected control points to temp array
						for (auto i : IndexRange(degree - s + 1))
						{
							tmp[i] = cp[row][span - degree + i];
						}
						// Insert knot
						for (auto j : IndexRange<size_t>(1, r + 1))
						{
							int L = span - degree + j;
							for (auto i : IndexRange(degree - j - s + 1))
							{
								scalar a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[row][L] = tmp[0];
							new_cp[row][span + r - j - s] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (auto i : IndexRange<size_t>(L + 1, span - s))
						{
							new_cp[row][i] = tmp[i - L];
						}
					}
				}
			}

			/**
			 * Split the surface into two along given parameter direction
			 * @param[in] degree Degree of surface along given direction
			 * @param[in] knots Knot vector of surface along given direction
			 * @param[in] control_points std::vector of control points
			 * @param[in] param Parameter to split curve
			 * @param[in] along_u Whether the direction to split along is the u-direction
			 * @param[out] left_knots Knots of the left part of the curve
			 * @param[out] left_control_points Control points of the left part of the curve
			 * @param[out] right_knots Knots of the right part of the curve
			 * @param[out] right_control_points Control points of the right part of the curve
			 */
			template<typename T>
			inline void surface_split(size_t degree, const std::vector<scalar>& knots, const std::vector<std::vector<T>>& control_points, scalar param, bool along_u,
				std::vector<scalar>& left_knots, std::vector<std::vector<T>>& left_control_points, std::vector<scalar>& right_knots,
				std::vector<std::vector<T>>& right_control_points)
			{
				std::vector<scalar> tmp_knots;
				std::vector<std::vector<T>> tmp_cp;

				int span = find_span(degree, knots, param);
				size_t r = degree - knot_multiplicity(knots, param);
				surface_knot_insert(degree, knots, control_points, param, r, along_u, tmp_knots, tmp_cp);

				left_knots.clear();
				right_knots.clear();

				int span_l = find_span(degree, tmp_knots, param) + 1;
				for (auto i : IndexRange(span_l))
				{
					left_knots.push_back(tmp_knots[i]);
				}
				left_knots.push_back(param);

				for (auto i : IndexRange(degree + 1))
				{
					right_knots.push_back(param);
				}
				for (auto i : IndexRange<size_t>(span_l, tmp_knots.size()))
				{
					right_knots.push_back(tmp_knots[i]);
				}

				int ks = span - degree + 1;
				if (along_u)
				{
					size_t ii = 0;
					left_control_points = std::vector<std::vector<T>>(ks + r, std::vector<T>(tmp_cp[0].size()));
					for (auto i : IndexRange(ks + r))
					{
						for (auto j : IndexRange(tmp_cp[0].size()))
						{
							left_control_points[ii / tmp_cp[0].size()][ii % tmp_cp[0].size()] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<T>>(tmp_cp.size() - ks - r + 1, std::vector<T>(tmp_cp[0].size()));
					for (auto i : IndexRange(ks + r - 1, tmp_cp.size()))
					{
						for (int j = 0; j < tmp_cp[0].size(); ++j)
						{
							right_control_points[ii / tmp_cp[0].size()][ii % tmp_cp[0].size()] = tmp_cp[i][j];
							ii++;
						}
					}
				}
				else
				{
					size_t ii = 0;
					left_control_points = std::vector<std::vector<T>>(tmp_cp.size(), std::vector<T>(ks + r));
					for (auto i : IndexRange(tmp_cp.size()))
					{
						for (auto j : IndexRange(ks + r))
						{
							left_control_points[ii / (ks + r)][ii % (ks + r)] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<T>>(tmp_cp.size(), std::vector<T>(tmp_cp[0].size() - ks - r + 1));
					for (auto i : IndexRange(tmp_cp.size()))
					{
						for (auto j : IndexRange(ks + r - 1, tmp_cp[0].size()))
						{
							right_control_points[ii / (tmp_cp[0].size() - ks - r + 1)][ii % (tmp_cp[0].size() - ks - r + 1)] = tmp_cp[i][j];
							ii++;
						}
					}
				}
			}

			/**
			 * Insert knots in the rational surface along u-direction
			 * @param[in] srf RationalSurface object
			 * @param[in] u Knot value to insert
			 * @param[in] repeat Number of times to insert
			 * @return New RationalSurface object after knot insertion
			 */
			inline RationalSurface surface_knot_insert_u(const RationalSurface& srf, scalar u, size_t repeat = 1)
			{
				if (!surface_is_valid(srf))
				{
					return RationalSurface();
				}
				RationalSurface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_v = srf.m_knots_v;

				// Original control points in homogenous coordinates
				std::vector<std::vector<vec4>> Cw = std::vector<std::vector<vec4>>(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); i++)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); j++)
					{
						Cw[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}

				// New knots and new homogenous control points after knot insertion
				std::vector<scalar> new_knots_u;
				std::vector<std::vector<vec4>> new_Cw;
				surface_knot_insert(srf.m_degree_u, srf.m_knots_u, Cw, u, repeat, true, new_srf.m_knots_u, new_Cw);

				// Convert back to cartesian coordinates
				new_srf.m_control_points = std::vector<std::vector<vec3>>(new_Cw.size(), std::vector<vec3>(new_Cw[0].size()));
				new_srf.m_weights = std::vector<std::vector<scalar>>(new_Cw.size(), std::vector<scalar>(new_Cw[0].size()));
				for (int i = 0; i < new_Cw.size(); i++)
				{
					for (int j = 0; j < new_Cw[0].size(); j++)
					{
						new_srf.m_control_points[i][j] = homogenous_to_cartesian(new_Cw[i][j]);
						new_srf.m_weights[i][j] = new_Cw[i][j].w();
					}
				}
				return new_srf;
			}

			/**
			 * Insert knots in the rational surface along v-direction
			 * @param[in] srf RationalSurface object
			 * @param[in] v Knot value to insert
			 * @param[in] repeat Number of times to insert
			 * @return New RationalSurface object after knot insertion
			 */
			inline RationalSurface surface_knot_insert_v(const RationalSurface& srf, scalar v, size_t repeat = 1)
			{
				assert(surface_is_valid(srf));
				RationalSurface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_u = srf.m_knots_u;
				// Original control points in homogenous coordinates
				std::vector<std::vector<vec4>> Cw = std::vector<std::vector<vec4>>(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); i++)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); j++)
					{
						Cw[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}

				// New knots and new homogenous control points after knot insertion
				std::vector<scalar> new_knots_v;
				std::vector<std::vector<vec4>> new_Cw;
				surface_knot_insert(srf.m_degree_v, srf.m_knots_v, Cw, v, repeat, false, new_srf.m_knots_v, new_Cw);

				// Convert back to cartesian coordinates
				new_srf.m_control_points = std::vector<std::vector<vec3>>(new_Cw.size(), std::vector<vec3>(new_Cw[0].size()));
				new_srf.m_weights = std::vector<std::vector<scalar>>(new_Cw.size(), std::vector<scalar>(new_Cw[0].size()));
				for (int i = 0; i < new_Cw.size(); i++)
				{
					for (int j = 0; j < new_Cw[0].size(); j++)
					{
						new_srf.m_control_points[i][j] = homogenous_to_cartesian(new_Cw[i][j]);
						new_srf.m_weights[i][j] = new_Cw[i][j].w();
					}
				}
				return new_srf;
			}

			/**
			 * Split a rational surface into two along u-direction
			 * @param[in] srf RationalSurface object
			 * @param[in] u Parameter along u-direction to split the surface
			 * @return std::tuple with first and second half of the surfaces
			 */
			inline std::tuple<RationalSurface, RationalSurface> surface_split_u(const RationalSurface& srf, scalar u)
			{
				assert(surface_is_valid(srf));
				RationalSurface left, right;
				left.m_degree_u = srf.m_degree_u;
				left.m_degree_v = srf.m_degree_v;
				left.m_knots_v = srf.m_knots_v;
				right.m_degree_u = srf.m_degree_u;
				right.m_degree_v = srf.m_degree_v;
				right.m_knots_v = srf.m_knots_v;

				// Compute homogenous coordinates of control points and weights
				std::vector<std::vector<vec4>> Cw = cartesian_to_homogenous(srf.m_control_points, srf.m_weights);

				// Split surface with homogenous coordinates
				std::vector<std::vector<vec4>> left_Cw, right_Cw;
				surface_split(srf.m_degree_u, srf.m_knots_u, Cw, u, true, left.m_knots_u, left_Cw, right.m_knots_u, right_Cw);

				homogenous_to_cartesian(left_Cw, left.m_control_points, left.m_weights);
				homogenous_to_cartesian(right_Cw, right.m_control_points, right.m_weights);

				return std::tuple<RationalSurface, RationalSurface>(std::move(left), std::move(right));
			}

			/**
			 * Split a rational surface into two along v-direction
			 * @param[in] srf RationalSurface object
			 * @param[in] v Parameter along v-direction to split the surface
			 * @return std::tuple with first and second half of the surfaces
			 */
			inline std::tuple<RationalSurface, RationalSurface> surface_split_v(const RationalSurface& srf, scalar v)
			{
				assert(surface_is_valid(srf));
				RationalSurface left, right;
				left.m_degree_u = srf.m_degree_u;
				left.m_degree_v = srf.m_degree_v;
				left.m_knots_u = srf.m_knots_u;
				right.m_degree_u = srf.m_degree_u;
				right.m_degree_v = srf.m_degree_v;
				right.m_knots_u = srf.m_knots_u;

				// Compute homogenous coordinates of control points and weights
				std::vector<std::vector<vec4>> Cw = cartesian_to_homogenous(srf.m_control_points, srf.m_weights);

				// Split surface with homogenous coordinates
				std::vector<std::vector<vec4>> left_Cw, right_Cw;
				surface_split(srf.m_degree_v, srf.m_knots_v, Cw, v, false, left.m_knots_v, left_Cw, right.m_knots_v, right_Cw);

				// Convert back to cartesian coordinates
				homogenous_to_cartesian(left_Cw, left.m_control_points, left.m_weights);
				homogenous_to_cartesian(right_Cw, right.m_control_points, right.m_weights);

				return std::tuple<RationalSurface, RationalSurface>(std::move(left), std::move(right));
			}

			/**
			 * Approximate a group of points with nubrs
			 * @param[in] through_points a group of points
			 * @param degree_u degree Parameter in the u-direction
			 * @param degree_v degree Parameter in the v-direction
			 * @param control_points_rows rows in the u-direction
			 * @param control_points_columns cols in the v-direction
			 * @param[out] surface RationalSurface
			 */
			inline bool global_approximation(const std::vector<std::vector<vec3>>& through_points, int degree_u, int degree_v, int control_points_rows,
				int control_points_columns, RationalSurface& surface)
			{
				int rows = control_points_rows;
				int n = rows - 1;
				int columns = control_points_columns;
				int m = columns - 1;

				std::vector<scalar> knot_vector_u;
				std::vector<scalar> knot_vector_v;

				Eigen::Matrix<vec3, -1, -1> temp_row_control_points(rows, columns);
				for (int i = 0; i < rows; i++)
				{
					RationalCurve tc;
					bool result = least_square_approximation(degree_u, through_points[i], columns, tc);
					if (!result)
						return false;
					temp_row_control_points.row(i) = Eigen::Matrix<vec3, -1, 1>::Map(tc.m_control_points.data(), tc.m_control_points.size());
					knot_vector_u = tc.m_knots;
				}

				Eigen::Matrix<vec3, -1, -1> temp_col_cpoints(rows, columns);
				for (int i = 0; i < columns; i++)
				{
					auto col_i = temp_row_control_points.col(i);
					RationalCurve tc;
					bool result = least_square_approximation(degree_v, std::vector<vec3>{ col_i.begin(), col_i.end() }, rows, tc);
					if (!result)
						return false;
					temp_col_cpoints.col(i) = Eigen::Matrix<vec3, 1, -1>::Map(tc.m_control_points.data(), tc.m_control_points.size());
					knot_vector_v = tc.m_knots;
				}
				surface.m_control_points.resize(rows);
				for (int i = 0; i < rows; i++)
				{
					auto row_i = temp_col_cpoints.row(i);
					surface.m_control_points[i] = std::vector<vec3>{ row_i.begin(), row_i.end() };
				}

				surface.m_degree_u = degree_u;
				surface.m_degree_v = degree_v;
				surface.m_knots_u.swap(knot_vector_u);
				surface.m_knots_v.swap(knot_vector_v);
				surface.m_weights = std::vector<std::vector<scalar>>(surface.m_control_points.size(), std::vector<scalar>(surface.m_control_points[0].size(), 1.0f));
				return true;
			}

			/**
			 * Approximate a group of points with nubrs
			 * @param[in] through_points a group of points
			 * @param degree_u degree Parameter in the u-direction
			 * @param degree_v degree Parameter in the v-direction
			 * @param[out] surface RationalSurface
			 */
			inline void global_interpolation(const std::vector<std::vector<vec3>>& throughpoints, int degreeU, int degreeV, RationalSurface& surface)
			{

				std::vector<scalar> uk;
				std::vector<scalar> vl;

				get_surfacemesh_parameterization(throughpoints, uk, vl);

				int rows = throughpoints.size();
				int cols = throughpoints[0].size();

				std::vector<std::vector<vec3>> controlpoints(rows, std::vector<vec3>(cols));
				std::vector<std::vector<scalar>> weights(rows, std::vector<scalar>(cols, 1.0f));
				std::vector<scalar> knotVectorU = average_knot_vector(degreeU, uk);
				std::vector<scalar> knotVectorV = average_knot_vector(degreeV, vl);

				for (int j = 0; j < cols; j++)
				{
					std::vector<vec3> temp(rows);
					for (int i = 0; i < rows; i++)
					{
						temp[i] = throughpoints[i][j];
					}
					RationalCurve tc;
					global_interpolation(degreeU, temp, tc);
					for (int i = 0; i < rows; i++)
					{
						controlpoints[i][j] = tc.m_control_points[i];
					}
				}

				for (int i = 0; i < rows; i++)
				{
					std::vector<vec3> temp(cols);
					for (int j = 0; j < cols; j++)
					{
						temp[j] = throughpoints[i][j];
					}
					RationalCurve tc;
					global_interpolation(degreeV, temp, tc);
					for (int j = 0; j < cols; j++)
					{
						controlpoints[i][j] = tc.m_control_points[j];
					}
				}
				surface.m_degree_u = degreeU;
				surface.m_degree_v = degreeV;
				surface.m_knots_u.swap(knotVectorU);
				surface.m_knots_v.swap(knotVectorV);
				surface.m_control_points = controlpoints;
				surface.m_weights.swap(weights);
			}
			/*
			 * sample a NURBS surface according to segments
			 */
			inline std::tuple<std::vector<vec3>, std::vector<size_t>> sample_nurbs_surface(const RationalSurface& srf, int segments)
			{
				// Sample the NURBS to get triangles
				std::vector<vec3> point_arr((segments + 1) * (segments + 1));
				std::vector<size_t> index_arr(segments * segments * 6);
				const float seg = 1.0f / segments;
				for (int iy = 0; iy <= segments; iy++)
				{
					for (int ix = 0; ix <= segments; ix++)
					{
						const float u = ix * seg;
						const float v = iy * seg;
						point_arr[ix + iy * (segments + 1)] = nous::nurbs::util::surface_point(srf, u, v);
					}
				}

				int index = 0;
				for (int iy = 0; iy < segments; iy++)
				{
					for (int ix = 0; ix < segments; ix++)
					{
						const int id = ix + iy * (segments + 1);
						index_arr[index++] = id + segments + 1;
						index_arr[index++] = id + segments + 2;
						index_arr[index++] = id + 1;
						index_arr[index++] = id + segments + 1;
						index_arr[index++] = id + 1;
						index_arr[index++] = id;
					}
				}
				return { point_arr, index_arr };
			}

			/*
			 * Evaluate the closest point to a NURBS.
			 */
			inline vec3 surface_closest_point(const RationalSurface& srf, const vec3& pos, int segments)
			{
				auto&& [point_arr, index_arr] = sample_nurbs_surface(srf, segments);
				// construct KdTree and query
				internal::TriangleKdTree kdtree(point_arr, index_arr);
				return kdtree.nearest(pos).nearest;
			}
			/*
			 * Evaluate bounding box on a nonrational NURBS surface
			 * @param[in] srf NURBS surface
			 * @param segments param for sample NURBS surface
			 * return the bounding box with given segments
			 */
			inline AABB3 surface_box(const RationalSurface& srf, int segments)
			{
				scalar u_min = std::numeric_limits<float>::max();
				scalar u_max = -std::numeric_limits<float>::max();
				scalar v_min = std::numeric_limits<float>::max();
				scalar v_max = -std::numeric_limits<float>::max();
				for (auto u_temp : srf.m_knots_u)
				{
					u_min = std::min(u_min, u_temp);
					u_max = std::max(u_max, u_temp);
				}
				for (auto v_temp : srf.m_knots_v)
				{
					v_min = std::min(v_min, v_temp);
					v_max = std::max(v_max, v_temp);
				}
				AABB3 box;
				for (int i = 0; i < segments + 1; i++)
				{
					scalar u = static_cast<scalar>(i) / static_cast<scalar>(segments);
					scalar norm_u = u_min + (u_max - u_min) * u;
					for (int j = 0; i < segments + 1; j++)
					{
						scalar v = static_cast<scalar>(j) / static_cast<scalar>(segments);
						scalar norm_v = v_min + (v_max - v_min) * v;
						box.extend(surface_point(srf, norm_u, norm_v));
					}
				}
				return box;
			}
		}// namespace util

	}// namespace nurbs
}// namespace nous