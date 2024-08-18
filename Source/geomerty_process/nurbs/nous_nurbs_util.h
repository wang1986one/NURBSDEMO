#pragma once
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Geomerty
{
	using vec3 = Eigen::Vector3<float>;
	using vec4 = Eigen::Vector4<float>;
	const float N_SCALAR_EPSILON = 0.00001f;
	namespace nurbs
	{
		namespace util
		{
			template<typename T> inline void transpose(const std::vector<std::vector<T>>& matrix, std::vector<std::vector<T>>& transposed)
			{
				std::vector<T> temp;

				for (int i = 0; i < matrix[0].size(); i++)
				{
					for (int j = 0; j < matrix.size(); j++)
					{
						temp.emplace_back(matrix[j][i]);
					}
					transposed.emplace_back(temp);
					temp.clear();
				}
			}

			template<typename T> inline std::vector<T> get_column(const std::vector<std::vector<T>>& matrix, int columnIndex)
			{
				int size = matrix.size();
				std::vector<T> result(size);
				for (int i = 0; i < size; i++)
				{
					result[i] = matrix[i][columnIndex];
				}
				return result;
			}

			inline std::vector<std::vector<float>> matrix_multiply(std::vector<std::vector<float>>& left, std::vector<std::vector<float>>& right)
			{
				int m = left.size();
				int n = left[0].size();
				int p = right[0].size();

				std::vector<std::vector<float>> result(m, std::vector<float>(p, 0.0f));
				for (int i = 0; i < m; i++)
				{
					for (int j = 0; j < p; j++)
					{
						for (int k = 0; k < n; k++)
						{
							result[i][j] += left[i][k] * right[k][j];
						}
					}
				}
				return result;
			}

			inline std::vector<std::vector<float>> solve_linear_system(const std::vector<std::vector<float>>& matrix,
				const std::vector<std::vector<float>>& right)
			{
				std::vector<std::vector<float>> result(matrix.size(), std::vector<float>(right[0].size()));

				Eigen::MatrixXf m(matrix.size(), matrix[0].size());
				for (int i = 0; i < matrix.size(); i++)
				{
					m.row(i) = Eigen::VectorXf::Map(&matrix[i][0], matrix[i].size());
				}

				Eigen::MatrixXf r(right.size(), right[0].size());
				for (int i = 0; i < right.size(); i++)
				{
					r.row(i) = Eigen::VectorXf::Map(&right[i][0], right[i].size());
				}

				Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> solve = m.lu().solve(r);
				for (int col = 0; col < solve.cols(); ++col)
				{
					for (int row = 0; row < solve.rows(); ++row)
					{
						result[row][col] = solve(row, col);
					}
				}
				return result;
			}

			inline float get_total_chord_length(const std::vector<vec3>& through_points)
			{
				int n = through_points.size();
				float length = 0.0f;
				for (int i = 1; i < n; i++)
				{
					length += (through_points[i] - through_points[i - 1]).norm();
				}
				return length;
			}

			inline std::vector<float> average_knot_vector(int degree, const std::vector<float>& params)
			{

				std::vector<float> uk = params;
				int size = params.size();
				int n = size - 1;
				int m = n + degree + 1;

				std::vector<float> knot_vector(m + 1, 0.0);
				for (int i = m - degree; i <= m; i++)
				{
					knot_vector[i] = 1.0;
				}

				for (int j = 1; j <= n - degree; j++)
				{
					float sum = 0.0;
					for (int i = j; i <= j + degree - 1; i++)
					{
						sum += uk[i];
					}
					knot_vector[j + degree] = (1.0 / degree) * sum;
				}
				return knot_vector;
			}

			inline std::vector<float> get_chord_parameterization(const std::vector<vec3>& through_points)
			{
				int size = through_points.size();
				int n = size - 1;

				std::vector<float> uk(size, 0.0);
				uk[n] = 1.0;

				float d = get_total_chord_length(through_points);
				for (int i = 1; i <= n - 1; i++)
				{
					uk[i] = uk[i - 1] + (through_points[i] - through_points[i - 1]).norm() / d;
				}
				return uk;
			}

			inline size_t binomial(size_t n, size_t k)
			{
				size_t result = 1;
				if (k > n)
				{
					return 0;
				}
				for (size_t i = 1; i <= k; ++i)
				{
					result *= (n + 1 - i);
					result /= i;
				}
				return result;
			}

			inline vec4 cartesian_to_homogenous(const vec3& pt, float w) { return vec4(pt[0] * w, pt[1] * w, pt[2] * w, w); }

			inline std::vector<std::vector<vec4>> cartesian_to_homogenous(const std::vector<std::vector<vec3>>& pts,
				const std::vector<std::vector<float>>& ws)
			{
				std::vector<std::vector<vec4>> Cw(pts.size(), std::vector<vec4>(pts[0].size()));
				for (int i = 0; i < pts.size(); ++i)
				{
					for (int j = 0; j < pts[0].size(); ++j)
					{
						Cw[i][j] = cartesian_to_homogenous(pts[i][j], ws[i][j]);
					}
				}
				return Cw;
			}

			inline vec3 homogenous_to_cartesian(const vec4& pt) { return vec3(pt[0] / pt[3], pt[1] / pt[3], pt[2] / pt[3]); }


			inline void homogenous_to_cartesian(const std::vector<vec4>& ptsws, std::vector<vec3>& pts, std::vector<float>& ws)
			{
				pts.clear();
				ws.clear();
				pts.reserve(ptsws.size());
				ws.reserve(ptsws.size());
				for (int i = 0; i < ptsws.size(); ++i)
				{
					const vec4& ptw_i = ptsws[i];
					pts.push_back(vec3(ptw_i[0] / ptw_i[3], ptw_i[1] / ptw_i[3], ptw_i[2] / ptw_i[3]));
					ws.push_back(ptw_i[3]);
				}
			}

			inline void homogenous_to_cartesian(const std::vector<std::vector<vec4>>& ptsws, std::vector<std::vector<vec3>>& pts,
				std::vector<std::vector<float>>& ws)
			{
				pts = std::vector<std::vector<vec3>>(ptsws.size(), std::vector<vec3>(ptsws[0].size()));
				ws = std::vector<std::vector<float>>(ptsws.size(), std::vector<float>(ptsws[0].size()));
				for (int i = 0; i < ptsws.size(); ++i)
				{
					for (int j = 0; j < ptsws[0].size(); ++j)
					{
						const vec4& ptw_ij = ptsws[i][j];
						float w_ij = ptw_ij[3];
						pts[i][j] = vec3(ptw_ij[0] / w_ij, ptw_ij[1] / w_ij, ptw_ij[2] / w_ij);
						ws[i][j] = w_ij;
					}
				}
			}


			inline bool close(float a, float b) { return (abs(a - b) < N_SCALAR_EPSILON) ? true : false; }
			inline bool close(vec3 a, vec3 b) { return close(a[0], b[0]) && close(a[1], b[1]) && close(a[2], b[2]); }

			inline bool is_almost_equal(float a, float b)
			{

				float eps = (abs(a) + std::abs(b) + 10) * N_SCALAR_EPSILON;
				float delta = a - b;
				return (-eps < delta) && (eps > delta);
			}
			inline bool is_almost_equal(vec3 a, vec3 b)
			{
				return is_almost_equal(a[0], b[0]) && is_almost_equal(a[1], b[1]) && is_almost_equal(a[2], b[2]);
			}
			inline size_t knot_multiplicity(const std::vector<float>& knots, float knot_val)
			{

				size_t mult = 0;
				for (auto knot : knots)
				{
					if (abs(knot_val - knot) < N_SCALAR_EPSILON)
					{
						++mult;
					}
				}
				return mult;
			}

			inline int find_span(size_t degree, const std::vector<float>& knots, float u)
			{
				// index of last control point
				int n = static_cast<int>(knots.size()) - degree - 2;
				assert(n >= 0);

				// For values of u that lies outside the domain
				if (u >= knots[n + 1])
				{
					return n;
				}
				if (u <= knots[degree])
				{
					return degree;
				}

				// Binary search
				// TODO: Replace this with std::lower_bound
				int low = degree;
				int high = n + 1;
				int mid = (low + high) / 2;
				while (u < knots[mid] || u >= knots[mid + 1])
				{
					if (u < knots[mid])
					{
						high = mid;
					}
					else
					{
						low = mid;
					}
					mid = (low + high) / 2;
				}
				return mid;
			}

			inline float bspline_one_basis(int i, size_t deg, const std::vector<float>& U, float u)
			{
				int m = static_cast<int>(U.size()) - 1;
				// Special case
				if ((i == 0 && close(u, U[0])) || (i == m - deg - 1 && close(u, U[m])))
				{
					return 1.0;
				}
				// Local property ensures that basis function is zero outside span
				if (u < U[i] || u >= U[i + deg + 1])
				{
					return 0.0;
				}
				// Initialize zeroth-degree functions
				std::vector<float> N;
				N.resize(deg + 1);
				for (int j = 0; j <= static_cast<int>(deg); j++)
				{
					N[j] = (u >= U[i + j] && u < U[i + j + 1]) ? 1.0 : 0.0;
				}
				// Compute triangular table
				for (int k = 1; k <= static_cast<int>(deg); k++)
				{
					float saved = (close(N[0], 0.0)) ? 0.0 : ((u - U[i]) * N[0]) / (U[i + k] - U[i]);
					for (int j = 0; j < static_cast<int>(deg) - k + 1; j++)
					{
						float Uleft = U[i + j + 1];
						float Uright = U[i + j + k + 1];
						if (close(N[j + 1], 0.0))
						{
							N[j] = saved;
							saved = 0.0;
						}
						else
						{
							float temp = N[j + 1] / (Uright - Uleft);
							N[j] = saved + (Uright - u) * temp;
							saved = (u - Uleft) * temp;
						}
					}
				}
				return N[0];
			}

			inline std::vector<float> bspline_basis(size_t deg, int span, const std::vector<float>& knots, float u)
			{
				std::vector<float> N;
				N.resize(deg + 1, 0.0f);
				std::vector<float> left, right;
				left.resize(deg + 1, 0.0f);
				right.resize(deg + 1, 0.0f);
				float saved = 0.0, temp = 0.0;

				N[0] = 1.0;

				for (int j = 1; j <= static_cast<int>(deg); j++)
				{
					left[j] = (u - knots[span + 1 - j]);
					right[j] = knots[span + j] - u;
					saved = 0.0;
					for (int r = 0; r < j; r++)
					{
						temp = N[r] / (right[r + 1] + left[j - r]);
						N[r] = saved + right[r + 1] * temp;
						saved = left[j - r] * temp;
					}
					N[j] = saved;
				}
				return N;
			}

			inline std::vector<std::vector<float>> bspline_der_basis(size_t deg, int span, const std::vector<float>& knots, float u, int num_ders)
			{
				std::vector<float> left, right;
				left.resize(deg + 1, 0.0f);
				right.resize(deg + 1, 0.0f);
				float saved = 0.0f, temp = 0.0f;
				std::vector<std::vector<float>> ndu(deg + 1, std::vector<float>(deg + 1, 0.0f));
				ndu[0][0] = 1.0f;

				for (int j = 1; j <= static_cast<int>(deg); j++)
				{
					left[j] = u - knots[span + 1 - j];
					right[j] = knots[span + j] - u;
					saved = 0.0;

					for (int r = 0; r < j; r++)
					{
						// Lower triangle
						ndu[j][r] = right[r + 1] + left[j - r];
						temp = ndu[r][j - 1] / ndu[j][r];
						// Upper triangle
						ndu[r][j] = saved + right[r + 1] * temp;
						saved = left[j - r] * temp;
					}

					ndu[j][j] = saved;
				}

				std::vector<std::vector<float>> ders(num_ders + 1, std::vector<float>(deg + 1, 0.0f));

				for (int j = 0; j <= static_cast<int>(deg); j++)
				{
					ders[0][j] = ndu[j][deg];
				}

				std::vector<std::vector<float>> a(2, std::vector<float>(deg + 1, 0.0f));

				for (int r = 0; r <= static_cast<int>(deg); r++)
				{
					int s1 = 0;
					int s2 = 1;
					a[0][0] = 1.0;

					for (int k = 1; k <= num_ders; k++)
					{
						float d = 0.0f;
						int rk = r - k;
						int pk = deg - k;
						int j1 = 0;
						int j2 = 0;

						if (r >= k)
						{
							a[s2][0] = a[s1][0] / ndu[pk + 1][rk];
							d = a[s2][0] * ndu[rk][pk];
						}

						if (rk >= -1)
						{
							j1 = 1;
						}
						else
						{
							j1 = -rk;
						}

						if (r - 1 <= pk)
						{
							j2 = k - 1;
						}
						else
						{
							j2 = deg - r;
						}

						for (int j = j1; j <= j2; j++)
						{
							a[s2][j] = (a[s1][j] - a[s1][j - 1]) / ndu[pk + 1][rk + 1];
							d += a[s2][j] * ndu[rk + j][pk];
						}

						if (r <= pk)
						{
							a[s2][k] = -a[s1][k - 1] / ndu[pk + 1][r];
							d += a[s2][k] * ndu[r][pk];
						}

						ders[k][r] = d;

						int temp = s1;
						s1 = s2;
						s2 = temp;
					}
				}

				float fac = static_cast<float>(deg);
				for (int k = 1; k <= num_ders; k++)
				{
					for (int j = 0; j <= static_cast<int>(deg); j++)
					{
						ders[k][j] *= fac;
					}
					fac *= static_cast<float>(deg - k);
				}

				return ders;
			}

		}// namespace util

	}// namespace nurbs
}// namespace nous