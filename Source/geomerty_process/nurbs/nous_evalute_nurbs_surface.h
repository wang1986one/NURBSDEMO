#pragma once
#include<tuple>
#include<vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "nous_evalute_nurbs_curve.h"
#include "nous_nurbs_curve.h"
#include "nous_nurbs_surface.h"
#include "nous_nurbs_util.h"
namespace Geomerty
{
	using vec3 = Eigen::Vector3<float>;
	using vec4 = Eigen::Vector4<float>;
	namespace nurbs
	{
		namespace util
		{

			inline bool get_surfacemesh_parameterization(const std::vector<std::vector<vec3>>& throughpoints, std::vector<float>& params_u,
				std::vector<float>& params_v)
			{
				int n = throughpoints.size();
				int m = throughpoints[0].size();

				std::vector<float> cds(std::max(n, m), 0.0);
				params_u.resize(n, 0.0);
				params_v.resize(m, 0.0);

				int num = m;

				for (int l = 0; l < m; l++)
				{
					float total = 0.0;
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
						float d = 0.0;
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
					float total = 0.0;
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
						float d = 0.0;
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


			inline vec3 surface_point(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u, const std::vector<float>& knots_v,
				const std::vector<std::vector<vec3>>& control_points, float u, float v)
			{

				// Initialize result to 0s
				vec3 point = vec3::Zero();

				// Find span and non-zero basis functions
				int span_u = find_span(degree_u, knots_u, u);
				int span_v = find_span(degree_v, knots_v, v);
				std::vector<float> nu = bspline_basis(degree_u, span_u, knots_u, u);
				std::vector<float> nv = bspline_basis(degree_v, span_v, knots_v, v);

				for (int l = 0; l <= degree_v; l++)
				{
					vec3 temp = vec3::Zero();
					for (int k = 0; k <= degree_u; k++)
					{
						temp += nu[k] * control_points[span_u - degree_u + k][span_v - degree_v + l];
					}

					point += nv[l] * temp;
				}
				return point;
			}

			inline vec4 surface_point(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u, const std::vector<float>& knots_v,
				const std::vector<std::vector<vec4>>& control_points, float u, float v)
			{

				// Initialize result to 0s
				vec4 point = vec4::Zero();

				// Find span and non-zero basis functions
				int span_u = find_span(degree_u, knots_u, u);
				int span_v = find_span(degree_v, knots_v, v);
				std::vector<float> nu = bspline_basis(degree_u, span_u, knots_u, u);
				std::vector<float> nv = bspline_basis(degree_v, span_v, knots_v, v);

				for (int l = 0; l <= degree_v; l++)
				{
					vec4 temp = vec4::Zero();
					for (int k = 0; k <= degree_u; k++)
					{
						temp += nu[k] * control_points[span_u - degree_u + k][span_v - degree_v + l];
					}

					point += nv[l] * temp;
				}
				return point;
			}

			inline std::vector<std::vector<vec3>> surface_derivatives(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u,
				const std::vector<float>& knots_v,
				const std::vector<std::vector<vec3>>& control_points, size_t num_ders, float u,
				float v)
			{

				std::vector<std::vector<vec3>> surf_ders(num_ders + 1, std::vector<vec3>(num_ders + 1, vec3::Zero()));

				// Set higher order derivatives to 0
				for (int k = degree_u + 1; k <= num_ders; k++)
				{
					for (int l = degree_v + 1; l <= num_ders; l++)
					{
						surf_ders[k][1] = vec3::Zero();
					}
				}
				// Find span and basis function derivatives
				float span_u = find_span(degree_u, knots_u, u);
				float span_v = find_span(degree_v, knots_v, v);
				std::vector<std::vector<float>> ders_u = bspline_der_basis(degree_u, span_u, knots_u, u, num_ders);
				std::vector<std::vector<float>> ders_v = bspline_der_basis(degree_v, span_v, knots_v, v, num_ders);

				// Number of non-zero derivatives is <= degree
				size_t du = num_ders > degree_u ? degree_u : num_ders;

				size_t dv = num_ders > degree_v ? degree_v : num_ders;

				std::vector<vec3> temp;
				temp.resize(degree_v + 1);
				// Compute derivatives
				for (int k = 0; k <= du; k++)
				{
					for (int s = 0; s <= degree_v; s++)
					{
						temp[s] = vec3::Zero();
						for (int r = 0; r <= degree_u; r++)
						{
							temp[s] += ders_u[k][r] * control_points[span_u - degree_u + r][span_v - degree_v + s];
						}
					}

					int dd = (num_ders - k) > dv ? dv : num_ders - k;

					for (int l = 0; l <= dd; l++)
					{
						for (int s = 0; s <= degree_v; s++)
						{
							surf_ders[k][1] += ders_v[l][s] * temp[s];
						}
					}
				}
				return surf_ders;
			}

			inline std::vector<std::vector<vec4>> surface_derivatives(size_t degree_u, size_t degree_v, const std::vector<float>& knots_u,
				const std::vector<float>& knots_v,
				const std::vector<std::vector<vec4>>& control_points, size_t num_ders, float u,
				float v)
			{

				std::vector<std::vector<vec4>> surf_ders(num_ders + 1, std::vector<vec4>(num_ders + 1, vec4::Zero()));

				// Set higher order derivatives to 0
				for (int k = degree_u + 1; k <= num_ders; k++)
				{
					for (int l = degree_v + 1; l <= num_ders; l++)
					{
						surf_ders[k][1] = vec4::Zero();
					}
				}

				// Find span and basis function derivatives
				float span_u = find_span(degree_u, knots_u, u);
				float span_v = find_span(degree_v, knots_v, v);
				std::vector<std::vector<float>> ders_u = bspline_der_basis(degree_u, span_u, knots_u, u, num_ders);
				std::vector<std::vector<float>> ders_v = bspline_der_basis(degree_v, span_v, knots_v, v, num_ders);

				// Number of non-zero derivatives is <= degree
				size_t du = num_ders > degree_u ? degree_u : num_ders;

				size_t dv = num_ders > degree_v ? degree_v : num_ders;

				std::vector<vec4> temp;
				temp.resize(degree_v + 1);
				// Compute derivatives
				for (int k = 0; k <= du; k++)
				{
					for (int s = 0; s <= degree_v; s++)
					{
						temp[s] = vec4::Zero();
						for (int r = 0; r <= degree_u; r++)
						{
							temp[s] += ders_u[k][r] * control_points[span_u - degree_u + r][span_v - degree_v + s];
						}
					}

					int dd = (num_ders - k) > dv ? dv : num_ders - k;

					for (int l = 0; l <= dd; l++)
					{
						for (int s = 0; s <= degree_v; s++)
						{
							surf_ders[k][l] += ders_v[l][s] * temp[s];
						}
					}
				}
				return surf_ders;
			}

			inline vec3 surface_point(const Surface& srf, float u, float v)
			{
				return surface_point(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, srf.m_control_points, u, v);
			}


			inline vec3 surface_point(const RationalSurface& srf, float u, float v)
			{
				// Compute homogenous coordinates of control points
				std::vector<std::vector<vec4>> cw(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));

				for (int i = 0; i < srf.m_control_points.size(); i++)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); j++)
					{
						cw[i][j] = vec4(srf.m_control_points[i][j].x() * srf.m_weights[i][j], srf.m_control_points[i][j].y() * srf.m_weights[i][j],
							srf.m_control_points[i][j].z() * srf.m_weights[i][j], srf.m_weights[i][j]);
					}
				}

				// Compute point using homogenous coordinates
				vec4 pointw = surface_point(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, cw, u, v);

				// Convert back to cartesian coordinates
				return vec3(pointw.x() / pointw.w(), pointw.y() / pointw.w(), pointw.z() / pointw.w());
			}


			inline std::vector<std::vector<vec3>> surface_derivatives(const Surface& srf, int num_ders, float u, float v)
			{
				return surface_derivatives(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, srf.m_control_points, num_ders, u, v);
			}

			inline std::vector<std::vector<vec3>> surface_derivatives(const RationalSurface& srf, int num_ders, float u, float v)
			{

				std::vector<std::vector<vec4>> homo_cp(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));

				for (int i = 0; i < srf.m_control_points.size(); ++i)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); ++j)
					{
						homo_cp[i][j] = vec4(srf.m_control_points[i][j].x() * srf.m_weights[i][j], srf.m_control_points[i][j].y() * srf.m_weights[i][j],
							srf.m_control_points[i][j].z() * srf.m_weights[i][j], srf.m_weights[i][j]);
					}
				}

				std::vector<std::vector<vec4>> homo_ders =
					surface_derivatives(srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v, homo_cp, num_ders, u, v);

				std::vector<std::vector<vec3>> aders(num_ders + 1, std::vector<vec3>(num_ders + 1));

				for (int i = 0; i < homo_ders.size(); ++i)
				{
					for (int j = 0; j < homo_ders[0].size(); ++j)
					{
						aders[i][j] = vec3(homo_ders[i][j].x(), homo_ders[i][j].y(), homo_ders[i][j].z());
					}
				}

				std::vector<std::vector<vec3>> surf_ders(num_ders + 1, std::vector<vec3>(num_ders + 1));
				for (int k = 0; k < num_ders + 1; ++k)
				{
					for (int l = 0; l < num_ders - k + 1; ++l)
					{
						vec3 der = aders[k][1];

						for (int j = 1; j < l + 1; ++j)
						{
							der -= binomial(l, j) * homo_ders[0][j].w() * surf_ders[k][l - j];
						}

						for (int i = 1; i < k + 1; ++i)
						{
							der -= binomial(k, i) * homo_ders[i][0].w() * surf_ders[k - i][l];

							vec3 tmp = vec3::Zero();
							for (int j = 1; j < l + 1; ++j)
							{
								tmp -= binomial(l, j) * homo_ders[i][j].w() * surf_ders[k - 1][l - j];
							}

							der -= binomial(k, i) * tmp;
						}

						der *= 1 / homo_ders[0][0].w();
						surf_ders[k][1] = der;
					}
				}
				return surf_ders;
			}


			inline std::tuple<vec3, vec3> surface_tangent(const Surface& srf, float u, float v)
			{
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 du = ptder[1][0];
				vec3 dv = ptder[0][1];
				float du_len = ptder[1][0].norm();
				float dv_len = ptder[0][1].norm();
				if (!close(du_len, 0.0f))
				{
					du /= du_len;
				}
				if (!close(dv_len, 0.0f))
				{
					dv /= dv_len;
				}

				return std::tuple<vec3, vec3>(std::move(du), std::move(dv));
			}

			inline std::tuple<vec3, vec3> surface_tangent(const RationalSurface& srf, float u, float v)
			{
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 du = ptder[1][0];
				vec3 dv = ptder[0][1];
				float du_len = ptder[1][0].norm();
				float dv_len = ptder[0][1].norm();

				if (!close(du_len, 0.0f))
				{
					du /= du_len;
				}
				if (!close(dv_len, 0.0f))
				{
					dv /= dv_len;
				}
				return std::tuple<vec3, vec3>(du, dv);
			}


			inline vec3 surface_normal(const Surface& srf, float u, float v)
			{
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 n = ptder[0][1].cross(ptder[1][0]);
				float n_len = n.norm();
				if (!close(n_len, 0.0f))
				{
					n /= n_len;
				}
				return n;
			}

			inline vec3 surface_normal(const RationalSurface& srf, float u, float v)
			{
				std::vector<std::vector<vec3>> ptder = surface_derivatives(srf, 1, u, v);
				vec3 n = ptder[0][1].cross(ptder[1][0]);
				float n_len = n.norm();
				if (!close(n_len, 0.0f))
				{
					n /= n_len;
				}
				return n;
			}

			inline void surface_knot_insert(size_t degree, const std::vector<float>& knots, const std::vector<std::vector<vec3>>& cp, float knot, size_t r,
				bool along_u, std::vector<float>& new_knots, std::vector<std::vector<vec3>>& new_cp)
			{
				int span = find_span(degree, knots, knot);
				size_t s = knot_multiplicity(knots, knot);
				assert(s <= degree);// Knot multiplicity cannot be greater than degree
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
				for (int i = 0; i <= span; ++i)
				{
					new_knots[i] = knots[i];
				}
				for (int i = 1; i <= r; ++i)
				{
					new_knots[span + i] = knot;
				}
				for (int i = span + 1; i < knots.size(); ++i)
				{
					new_knots[i + r] = knots[i];
				}
				// Compute alpha
				std::vector<std::vector<float>> alpha(degree - s, std::vector<float>(r + 1, 0.0f));
				for (int j = 1; j <= r; ++j)
				{
					int L = span - degree + j;
					for (int i = 0; i <= degree - j - s; ++i)
					{
						alpha[i][j] = (knot - knots[L + i]) / (knots[i + span + 1] - knots[L + i]);
					}
				}

				// Create a temporary container for affected control points per row/column
				std::vector<vec3> tmp(degree + 1);

				if (along_u)
				{

					// Create new control points with additional rows
					new_cp = std::vector<std::vector<vec3>>(cp.size() + r, std::vector<vec3>(cp[0].size()));

					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (int col = 0; col < cp[0].size(); ++col)
					{

						// Copy unaffected control points
						for (int i = 0; i <= span - degree; ++i)
						{
							new_cp[i][col] = cp[i][col];
						}
						for (int i = span - s; i < cp.size(); ++i)
						{
							new_cp[i + r][col] = cp[i][col];
						}
						// Copy affected control points to temp array
						for (int i = 0; i < degree - s + 1; ++i)
						{
							tmp[i] = cp[span - degree + i][col];
						}
						// Insert knot
						for (int j = 1; j <= r; ++j)
						{
							int L = span - degree + j;
							for (int i = 0; i <= degree - j - s; ++i)
							{
								float a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[L][col] = tmp[0];
							new_cp[span + r - j - s][col] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (int i = L + 1; i < span - s; ++i)
						{
							new_cp[i][col] = tmp[i - L];
						}
					}
				}
				else
				{
					// Create new control points with additional columns
					new_cp = std::vector<std::vector<vec3>>(cp.size(), std::vector<vec3>(cp[0].size() + r));
					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (int row = 0; row < cp.size(); ++row)
					{
						// Copy unaffected control points
						for (int i = 0; i <= span - degree; ++i)
						{
							new_cp[row][i] = cp[row][i];
						}
						for (int i = span - s; i < cp[0].size(); ++i)
						{
							new_cp[row][i + r] = cp[row][i];
						}
						// Copy affected control points to temp array
						for (int i = 0; i < degree - s + 1; ++i)
						{
							tmp[i] = cp[row][span - degree + i];
						}
						// Insert knot
						for (int j = 1; j <= r; ++j)
						{
							int L = span - degree + j;
							for (int i = 0; i <= degree - j - s; ++i)
							{
								float a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[row][L] = tmp[0];
							new_cp[row][span + r - j - s] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (int i = L + 1; i < span - s; ++i)
						{
							new_cp[row][i] = tmp[i - L];
						}
					}
				}
			}
			inline void surface_knot_insert(size_t degree, const std::vector<float>& knots, const std::vector<std::vector<vec4>>& cp, float knot, size_t r,
				bool along_u, std::vector<float>& new_knots, std::vector<std::vector<vec4>>& new_cp)
			{
				int span = find_span(degree, knots, knot);
				size_t s = knot_multiplicity(knots, knot);
				assert(s <= degree);// Knot multiplicity cannot be greater than degree
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
				for (int i = 0; i <= span; ++i)
				{
					new_knots[i] = knots[i];
				}
				for (int i = 1; i <= r; ++i)
				{
					new_knots[span + i] = knot;
				}
				for (int i = span + 1; i < knots.size(); ++i)
				{
					new_knots[i + r] = knots[i];
				}
				// Compute alpha
				std::vector<std::vector<float>> alpha(degree - s, std::vector<float>(r + 1, 0.0f));
				for (int j = 1; j <= r; ++j)
				{
					int L = span - degree + j;
					for (int i = 0; i <= degree - j - s; ++i)
					{
						alpha[i][j] = (knot - knots[L + i]) / (knots[i + span + 1] - knots[L + i]);
					}
				}

				// Create a temporary container for affected control points per row/column
				std::vector<vec4> tmp(degree + 1);

				if (along_u)
				{

					// Create new control points with additional rows
					new_cp = std::vector<std::vector<vec4>>(cp.size() + r, std::vector<vec4>(cp[0].size()));

					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (int col = 0; col < cp[0].size(); ++col)
					{

						// Copy unaffected control points
						for (int i = 0; i <= span - degree; ++i)
						{
							new_cp[i][col] = cp[i][col];
						}
						for (int i = span - s; i < cp.size(); ++i)
						{
							new_cp[i + r][col] = cp[i][col];
						}
						// Copy affected control points to temp array
						for (int i = 0; i < degree - s + 1; ++i)
						{
							tmp[i] = cp[span - degree + i][col];
						}
						// Insert knot
						for (int j = 1; j <= r; ++j)
						{
							int L = span - degree + j;
							for (int i = 0; i <= degree - j - s; ++i)
							{
								float a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[L][col] = tmp[0];
							new_cp[span + r - j - s][col] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (int i = L + 1; i < span - s; ++i)
						{
							new_cp[i][col] = tmp[i - L];
						}
					}
				}
				else
				{
					// Create new control points with additional columns
					new_cp = std::vector<std::vector<vec4>>(cp.size(), std::vector<vec4>(cp[0].size() + r));
					// Update control points
					// Each row is a u-isocurve, each col is a v-isocurve
					for (int row = 0; row < cp.size(); ++row)
					{
						// Copy unaffected control points
						for (int i = 0; i <= span - degree; ++i)
						{
							new_cp[row][i] = cp[row][i];
						}
						for (int i = span - s; i < cp[0].size(); ++i)
						{
							new_cp[row][i + r] = cp[row][i];
						}
						// Copy affected control points to temp array
						for (int i = 0; i < degree - s + 1; ++i)
						{
							tmp[i] = cp[row][span - degree + i];
						}
						// Insert knot
						for (int j = 1; j <= r; ++j)
						{
							int L = span - degree + j;
							for (int i = 0; i <= degree - j - s; ++i)
							{
								float a = alpha[i][j];
								tmp[i] = (1 - a) * tmp[i] + a * tmp[i + 1];
							}
							new_cp[row][L] = tmp[0];
							new_cp[row][span + r - j - s] = tmp[degree - j - s];
						}
						int L = span - degree + r;
						for (int i = L + 1; i < span - s; ++i)
						{
							new_cp[row][i] = tmp[i - L];
						}
					}
				}
			}

			inline void surface_split(size_t degree, const std::vector<float>& knots, const std::vector<std::vector<vec3>>& control_points, float param,
				bool along_u, std::vector<float>& left_knots, std::vector<std::vector<vec3>>& left_control_points,
				std::vector<float>& right_knots, std::vector<std::vector<vec3>>& right_control_points)
			{
				std::vector<float> tmp_knots;
				std::vector<std::vector<vec3>> tmp_cp;

				int span = find_span(degree, knots, param);
				size_t r = degree - knot_multiplicity(knots, param);
				surface_knot_insert(degree, knots, control_points, param, r, along_u, tmp_knots, tmp_cp);

				left_knots.clear();
				right_knots.clear();

				int span_l = find_span(degree, tmp_knots, param) + 1;
				for (int i = 0; i < span_l; ++i)
				{
					left_knots.push_back(tmp_knots[i]);
				}
				left_knots.push_back(param);

				for (int i = 0; i < degree + 1; ++i)
				{
					right_knots.push_back(param);
				}
				for (int i = span_l; i < tmp_knots.size(); ++i)
				{
					right_knots.push_back(tmp_knots[i]);
				}

				int ks = span - degree + 1;
				if (along_u)
				{
					size_t ii = 0;
					left_control_points = std::vector<std::vector<vec3>>(ks + r, std::vector<vec3>(tmp_cp[0].size()));
					for (int i = 0; i < ks + r; ++i)
					{
						for (int j = 0; j < tmp_cp[0].size(); ++j)
						{
							left_control_points[ii / tmp_cp[0].size()][ii % tmp_cp[0].size()] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<vec3>>(tmp_cp.size() - ks - r + 1, std::vector<vec3>(tmp_cp[0].size()));
					for (int i = ks + r - 1; i < tmp_cp.size(); ++i)
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
					left_control_points = std::vector<std::vector<vec3>>(tmp_cp.size(), std::vector<vec3>(ks + r));
					for (int i = 0; i < tmp_cp.size(); ++i)
					{
						for (int j = 0; j < ks + r; ++j)
						{
							left_control_points[ii / (ks + r)][ii % (ks + r)] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<vec3>>(tmp_cp.size(), std::vector<vec3>(tmp_cp[0].size() - ks - r + 1));
					for (int i = 0; i < tmp_cp.size(); ++i)
					{
						for (int j = ks + r - 1; j < tmp_cp[0].size(); ++j)
						{
							right_control_points[ii / (tmp_cp[0].size() - ks - r + 1)][ii % (tmp_cp[0].size() - ks - r + 1)] = tmp_cp[i][j];
							ii++;
						}
					}
				}
			}
			inline void surface_split(size_t degree, const std::vector<float>& knots, const std::vector<std::vector<vec4>>& control_points, float param,
				bool along_u, std::vector<float>& left_knots, std::vector<std::vector<vec4>>& left_control_points,
				std::vector<float>& right_knots, std::vector<std::vector<vec4>>& right_control_points)
			{
				std::vector<float> tmp_knots;
				std::vector<std::vector<vec4>> tmp_cp;

				int span = find_span(degree, knots, param);
				size_t r = degree - knot_multiplicity(knots, param);
				surface_knot_insert(degree, knots, control_points, param, r, along_u, tmp_knots, tmp_cp);

				left_knots.clear();
				right_knots.clear();

				int span_l = find_span(degree, tmp_knots, param) + 1;
				for (int i = 0; i < span_l; ++i)
				{
					left_knots.push_back(tmp_knots[i]);
				}
				left_knots.push_back(param);

				for (int i = 0; i < degree + 1; ++i)
				{
					right_knots.push_back(param);
				}
				for (int i = span_l; i < tmp_knots.size(); ++i)
				{
					right_knots.push_back(tmp_knots[i]);
				}

				int ks = span - degree + 1;
				if (along_u)
				{
					size_t ii = 0;
					left_control_points = std::vector<std::vector<vec4>>(ks + r, std::vector<vec4>(tmp_cp[0].size()));
					for (int i = 0; i < ks + r; ++i)
					{
						for (int j = 0; j < tmp_cp[0].size(); ++j)
						{
							left_control_points[ii / tmp_cp[0].size()][ii % tmp_cp[0].size()] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<vec4>>(tmp_cp.size() - ks - r + 1, std::vector<vec4>(tmp_cp[0].size()));
					for (int i = ks + r - 1; i < tmp_cp.size(); ++i)
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
					left_control_points = std::vector<std::vector<vec4>>(tmp_cp.size(), std::vector<vec4>(ks + r));
					for (int i = 0; i < tmp_cp.size(); ++i)
					{
						for (int j = 0; j < ks + r; ++j)
						{
							left_control_points[ii / (ks + r)][ii % (ks + r)] = tmp_cp[i][j];
							ii++;
						}
					}
					ii = 0;
					right_control_points = std::vector<std::vector<vec4>>(tmp_cp.size(), std::vector<vec4>(tmp_cp[0].size() - ks - r + 1));
					for (int i = 0; i < tmp_cp.size(); ++i)
					{
						for (int j = ks + r - 1; j < tmp_cp[0].size(); ++j)
						{
							right_control_points[ii / (tmp_cp[0].size() - ks - r + 1)][ii % (tmp_cp[0].size() - ks - r + 1)] = tmp_cp[i][j];
							ii++;
						}
					}
				}
			}

			inline Surface surface_knot_insert_u(const Surface& srf, float u, size_t repeat = 1)
			{
				Surface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_v = srf.m_knots_v;
				surface_knot_insert(new_srf.m_degree_u, srf.m_knots_u, srf.m_control_points, u, repeat, true, new_srf.m_knots_u, new_srf.m_control_points);
				return new_srf;
			}

			inline RationalSurface surface_knot_insert_u(const RationalSurface& srf, float u, size_t repeat = 1)
			{
				RationalSurface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_v = srf.m_knots_v;

				// Original control points in homogenous coordinates
				std::vector<std::vector<vec4>> Cw =
					std::vector<std::vector<vec4>>(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); ++i)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); ++j)
					{
						Cw[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}

				// New knots and new homogenous control points after knot insertion
				std::vector<float> new_knots_u;
				std::vector<std::vector<vec4>> new_Cw;
				surface_knot_insert(srf.m_degree_u, srf.m_knots_u, Cw, u, repeat, true, new_srf.m_knots_u, new_Cw);

				// Convert back to cartesian coordinates
				new_srf.m_control_points = std::vector<std::vector<vec3>>(new_Cw.size(), std::vector<vec3>(new_Cw[0].size()));
				new_srf.m_weights = std::vector<std::vector<float>>(new_Cw.size(), std::vector<float>(new_Cw[0].size()));
				for (int i = 0; i < new_Cw.size(); ++i)
				{
					for (int j = 0; j < new_Cw[0].size(); ++j)
					{
						new_srf.m_control_points[i][j] = homogenous_to_cartesian(new_Cw[i][j]);
						new_srf.m_weights[i][j] = new_Cw[i][j].w();
					}
				}
				return new_srf;
			}

			inline Surface surface_knot_insert_v(const Surface& srf, float v, size_t repeat = 1)
			{
				Surface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_u = srf.m_knots_u;
				// New knots and new control points after knot insertion
				surface_knot_insert(srf.m_degree_v, srf.m_knots_v, srf.m_control_points, v, repeat, false, new_srf.m_knots_v, new_srf.m_control_points);
				return new_srf;
			}

			inline RationalSurface surface_knot_insert_v(const RationalSurface& srf, float v, size_t repeat = 1)
			{
				RationalSurface new_srf;
				new_srf.m_degree_u = srf.m_degree_u;
				new_srf.m_degree_v = srf.m_degree_v;
				new_srf.m_knots_u = srf.m_knots_u;
				// Original control points in homogenous coordinates
				std::vector<std::vector<vec4>> Cw =
					std::vector<std::vector<vec4>>(srf.m_control_points.size(), std::vector<vec4>(srf.m_control_points[0].size()));
				for (int i = 0; i < srf.m_control_points.size(); ++i)
				{
					for (int j = 0; j < srf.m_control_points[0].size(); ++j)
					{
						Cw[i][j] = cartesian_to_homogenous(srf.m_control_points[i][j], srf.m_weights[i][j]);
					}
				}

				// New knots and new homogenous control points after knot insertion
				std::vector<float> new_knots_v;
				std::vector<std::vector<vec4>> new_Cw;
				surface_knot_insert(srf.m_degree_v, srf.m_knots_v, Cw, v, repeat, false, new_srf.m_knots_v, new_Cw);

				// Convert back to cartesian coordinates
				new_srf.m_control_points = std::vector<std::vector<vec3>>(new_Cw.size(), std::vector<vec3>(new_Cw[0].size()));
				new_srf.m_weights = std::vector<std::vector<float>>(new_Cw.size(), std::vector<float>(new_Cw[0].size()));
				for (int i = 0; i < new_Cw.size(); ++i)
				{
					for (int j = 0; j < new_Cw[0].size(); ++j)
					{
						new_srf.m_control_points[i][j] = homogenous_to_cartesian(new_Cw[i][j]);
						new_srf.m_weights[i][j] = new_Cw[i][j].w();
					}
				}
				return new_srf;
			}

			inline std::tuple<Surface, Surface> surface_split_u(const Surface& srf, float u)
			{
				Surface left, right;
				left.m_degree_u = srf.m_degree_u;
				left.m_degree_v = srf.m_degree_v;
				left.m_knots_v = srf.m_knots_v;
				right.m_degree_u = srf.m_degree_u;
				right.m_degree_v = srf.m_degree_v;
				right.m_knots_v = srf.m_knots_v;
				surface_split(srf.m_degree_u, srf.m_knots_u, srf.m_control_points, u, true, left.m_knots_u, left.m_control_points, right.m_knots_u,
					right.m_control_points);
				return std::tuple<Surface, Surface>(std::move(left), std::move(right));
			}

			inline std::tuple<RationalSurface, RationalSurface> surface_split_u(const RationalSurface& srf, float u)
			{
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

				// Convert back to cartesian coordinates
				/*
					inline void homogenousToCartesian(const std::vector<std::vector<vec4>>& ptsws, std::vector<std::vector<vec3>>& pts,
													  std::vector<std::vector<float>>& ws)
					*/
				homogenous_to_cartesian(left_Cw, left.m_control_points, left.m_weights);
				homogenous_to_cartesian(right_Cw, right.m_control_points, right.m_weights);

				return std::tuple<RationalSurface, RationalSurface>(std::move(left), std::move(right));
			}

			inline std::tuple<Surface, Surface> surface_split_v(const Surface& srf, float v)
			{
				Surface left, right;
				left.m_degree_u = srf.m_degree_u;
				left.m_degree_v = srf.m_degree_v;
				left.m_knots_u = srf.m_knots_u;
				right.m_degree_u = srf.m_degree_u;
				right.m_degree_v = srf.m_degree_v;
				right.m_knots_u = srf.m_knots_u;
				surface_split(srf.m_degree_v, srf.m_knots_v, srf.m_control_points, v, false, left.m_knots_v, left.m_control_points, right.m_knots_v,
					right.m_control_points);
				return std::tuple<Surface, Surface>(std::move(left), std::move(right));
			}

			inline std::tuple<RationalSurface, RationalSurface> surface_split_v(const RationalSurface& srf, float v)
			{
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

			inline bool global_approximation(const std::vector<std::vector<vec3>>& through_points, int degree_u, int degree_v,
				int control_points_rows, int control_points_columns, RationalSurface& surface)
			{

				int rows = control_points_rows;
				int n = rows - 1;
				int columns = control_points_columns;
				int m = columns - 1;

				std::vector<float> knot_vector_u;
				std::vector<float> knot_vector_v;

				std::vector<std::vector<vec3>> temp_control_points;
				for (int i = 0; i < rows; i++)
				{
					RationalCurve tc;
					bool result = least_square_approximation(degree_u, through_points[i], rows, tc);
					if (!result)
						return false;
					std::vector<vec3> points = tc.m_control_points;
					temp_control_points.emplace_back(points);
					knot_vector_u = tc.m_knots;
				}

				std::vector<std::vector<vec3>> control_points;
				std::vector<std::vector<vec3>> tPoints;
				for (int i = 0; i < columns; i++)
				{
					std::vector<vec3> c = get_column(temp_control_points, i);
					RationalCurve tc;
					bool result = least_square_approximation(degree_v, c, columns, tc);
					if (!result)
						return false;
					std::vector<vec3> points = tc.m_control_points;
					tPoints.emplace_back(points);
					knot_vector_v = tc.m_knots;
				}
				transpose(tPoints, control_points);
				surface.m_degree_u = degree_u;
				surface.m_degree_v = degree_v;
				surface.m_knots_u = knot_vector_u;
				surface.m_knots_v = knot_vector_v;
				surface.m_control_points = control_points;
				surface.m_weights = std::vector<std::vector<float>>(control_points.size(), std::vector<float>(control_points[0].size(), 1.0f));
				return true;
			}
			inline void global_interpolation(const std::vector<std::vector<vec3>>& throughpoints, int degreeU, int degreeV, RationalSurface& surface)
			{

				std::vector<float> uk;
				std::vector<float> vl;

				get_surfacemesh_parameterization(throughpoints, uk, vl);

				int rows = throughpoints.size();
				int cols = throughpoints[0].size();

				std::vector<std::vector<vec3>> controlpoints(rows, std::vector<vec3>(cols));
				std::vector<std::vector<float>> weights(rows, std::vector<float>(cols, 1.0f));
				std::vector<float> knotVectorU = average_knot_vector(degreeU, uk);
				std::vector<float> knotVectorV = average_knot_vector(degreeV, vl);

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
				surface.m_knots_u = knotVectorU;
				surface.m_knots_v = knotVectorV;
				surface.m_control_points = controlpoints;
				surface.m_weights = weights;
			}

		}// namespace util

	}// namespace nurbs
}// namespace nous