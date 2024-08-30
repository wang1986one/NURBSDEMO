#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Geomerty
{
	namespace nurbs
	{
		namespace util
		{
			using scalar = float;
			using vec3 = Eigen::Vector3<scalar>;
			using vec4 = Eigen::Vector4<scalar>;
			using AABB3 = Eigen::AlignedBox<scalar, 3>;
			inline constexpr scalar N_FLOAT_PI = 3.1415926535897932385f;
			inline constexpr scalar N_SCALAR_EPSILON = std::numeric_limits<scalar>::epsilon();
			/**
			 * Checks if the relation between degree, number of knots, and
			 * number of control points is valid
			 * @param[in] degree Degree
			 * @param[in] num_knots Number of knot values
			 * @param[in] num_ctrl_pts Number of control points
			 * @return Whether the relationship is valid
			 */
			inline bool is_valid_relation(const size_t degree, const size_t num_knots, const size_t num_ctrl_pts)
			{
				return (num_knots - degree - 1) == num_ctrl_pts;
			}

			/**
			 * Get the length of path consisting of a group of points
			 * @param[in] through_points.
			 * return the length of the path.
			 */
			inline scalar get_total_chord_length(const std::vector<vec3>& through_points)
			{
				int n = through_points.size();
				scalar length = 0.0f;
				for (int i = 1; i < n; i++)
				{
					length += (through_points[i] - through_points[i - 1]).norm();
				}
				return length;
			}
			/**
			 * Get average_knot_vector of params according to degree
			 * @param[in] params.
			 * return the average knotvector.
			 */
			inline std::vector<scalar> average_knot_vector(const size_t degree, const std::vector<scalar>& params)
			{
				if (params.size() <= 0 || degree < 1)
				{
					return std::vector<scalar>{};
				}

				std::vector<scalar> uk = params;
				int size = params.size();
				int n = size - 1;
				int m = n + degree + 1;

				std::vector<scalar> knot_vector(m + 1, 0.0);
				for (int i = m - degree; i < m + 1; i++)
				{
					knot_vector[i] = 1.0;
				}

				for (int j = 1; j <= n - degree; j++)
				{
					scalar sum = 0.0;
					for (int i = j; i <= j + degree - 1; i++)
					{
						sum += uk[i];
					}
					knot_vector[j + degree] = (1.0 / degree) * sum;
				}
				return knot_vector;
			}

			/**
			 * Get the parameterization of a group of points
			 * @param[in] through_points.
			 * return the result of parameterization.
			 */
			inline std::vector<scalar> get_chord_parameterization(const std::vector<vec3>& through_points)
			{
				if (through_points.empty())
				{
					return std::vector<scalar>{};
				}
				int size = through_points.size();
				int n = size - 1;

				std::vector<scalar> uk(size, 0.0);
				uk[n] = 1.0;

				scalar d = get_total_chord_length(through_points);
				for (int i = 1; i <= n - 1; i++)
				{
					uk[i] = uk[i - 1] + (through_points[i] - through_points[i - 1]).norm() / d;
				}
				return uk;
			}

			/**
			 * Compute the binomial coefficient (nCk) using the formula
			 * \product_{i=0}^k (n + 1 - i) / i
			 */
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
			/**
			 * Convert a point in cartesian coordinates to an d point in homogenous
			 * coordinates
			 * @param[in] pt Point in cartesian coordinates
			 * @param[in] w Weight
			 * @return Input point in homogenous coordinates
			 */
			inline vec4 cartesian_to_homogenous(const vec3& pt, scalar w)
			{
				vec4 hom_temp = (w * pt).homogeneous();
				hom_temp(3) = w;
				return hom_temp;
			}
			/**
			 * Convert 2D list of points in cartesian coordinates to homogenous coordinates
			 * @param[in] pts Points in cartesian coordinates
			 * @param[in] ws Weights
			 * @return Points in homogenous coordinates
			 */
			inline std::vector<std::vector<vec4>> cartesian_to_homogenous(const std::vector<std::vector<vec3>>& pts, const std::vector<std::vector<scalar>>& ws)
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

			/**
			 * Convert an nd point in homogenous coordinates to an (n-1)d point in cartesian
			 * coordinates by perspective division
			 * @param[in] pt Point in homogenous coordinates
			 * @return Point in cartesian coordinates
			 */
			inline vec3 homogenous_to_cartesian(const vec4& pt) { return pt.head<3>() / pt[3]; }
			/**
			 * Convert a list of nd points in homogenous coordinates to a list of (n-1)d points in cartesian
			 * coordinates by perspective division
			 * @param[in] ptsws Points in homogenous coordinates
			 * @param[out] pts Points in cartesian coordinates
			 * @param[out] ws Homogenous weights
			 */

			inline void homogenous_to_cartesian(const std::vector<vec4>& ptsws, std::vector<vec3>& pts, std::vector<scalar>& ws)
			{
				pts.clear();
				ws.clear();
				pts.reserve(ptsws.size());
				ws.reserve(ptsws.size());
				for (int i = 0; i < ptsws.size(); ++i)
				{
					const vec4& ptw_i = ptsws[i];
					homogenous_to_cartesian(ptw_i);
					pts.emplace_back(homogenous_to_cartesian(ptw_i));
					ws.emplace_back(ptw_i[3]);
				}
			}
			/**
			 * Convert a 2D list of nd points in homogenous coordinates to cartesian
			 * coordinates by perspective division
			 * @param[in] ptsws Points in homogenous coordinates
			 * @param[out] pts Points in cartesian coordinates
			 * @param[out] ws Homogenous weights
			 */
			inline void homogenous_to_cartesian(const std::vector<std::vector<vec4>>& ptsws, std::vector<std::vector<vec3>>& pts, std::vector<std::vector<scalar>>& ws)
			{
				pts = std::vector<std::vector<vec3>>(ptsws.size(), std::vector<vec3>(ptsws[0].size()));
				ws = std::vector<std::vector<scalar>>(ptsws.size(), std::vector<scalar>(ptsws[0].size()));
				for (int i = 0; i < ptsws.size(); ++i)
				{
					for (int j = 0; j < ptsws[0].size(); ++j)
					{
						const vec4& ptw_ij = ptsws[i][j];
						pts[i][j] = homogenous_to_cartesian(ptw_ij);
						ws[i][j] = ptw_ij[3];
					}
				}
			}

			/**
			 * Check if two numbers are close enough within eps
			 * @param[in] a First number
			 * @param[in] b Second number
			 * @return Whether the numbers are close w.r.t. the tolerance
			 */
			inline bool close(scalar a, scalar b) { return (abs(a - b) < N_SCALAR_EPSILON) ? true : false; }
			inline bool close(vec3 a, vec3 b) { return a.isApprox(b); }
			/*
			 * Check if two numbers are close enough in realtive scale
			 * @param[in] a First number
			 * @param[in] b Second number
			 * @return Whether the numbers are close in w.r.t. the tolerance
			 */
			inline bool is_almost_equal(scalar a, scalar b)
			{

				scalar eps = (abs(a) + abs(b) + 10) * N_SCALAR_EPSILON;
				scalar delta = a - b;
				return (-eps < delta) && (eps > delta);
			}
			inline bool is_almost_equal(vec3 a, vec3 b)
			{
				return is_almost_equal(a[0], b[0]) && is_almost_equal(a[1], b[1]) && is_almost_equal(a[2], b[2]);
			}

			/**
			 * Returns the multiplicity of the knot at index
			 * @tparam Type of knot values
			 * @param[in] knots Knot vector
			 * @param[in] knot_val Knot of interest
			 * @return Multiplicity (>= 0)
			 */
			inline size_t knot_multiplicity(const std::vector<scalar>& knots, scalar knot_val)
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
			/**
			 * Find the span of the given parameter in the knot vector.
			 * @param[in] degree Degree of the curve.
			 * @param[in] knots Knot vector of the curve.
			 * @param[in] u Parameter value.
			 * @return Span index into the knot vector such that (span - 1) < u <= span
			 */
			inline int find_span(const size_t degree, const std::vector<scalar>& knots, scalar u)
			{
				// index of last control point
				int n = static_cast<int>(knots.size()) - degree - 2;
				if (n < 0)
				{
					return -1;
				}

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

			/**
			 * Compute a single B-spline basis function
			 * @param[in] i The ith basis function to compute.
			 * @param[in] deg Degree of the basis function.
			 * @param[in] knots Knot vector corresponding to the basis functions.
			 * @param[in] u Parameter to evaluate the basis functions at.
			 * @return The value of the ith basis function at u.
			 */
			inline scalar bspline_one_basis(int i, size_t deg, const std::vector<scalar>& U, scalar u)
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
				std::vector<scalar> N;
				N.resize(deg + 1);
				for (int j = 0; j <= static_cast<int>(deg); j++)
				{
					N[j] = (u >= U[i + j] && u < U[i + j + 1]) ? 1.0 : 0.0;
				}
				// Compute triangular table
				for (int k = 1; k <= static_cast<int>(deg); k++)
				{
					scalar saved = (close(N[0], 0.0)) ? 0.0 : ((u - U[i]) * N[0]) / (U[i + k] - U[i]);
					for (int j = 0; j < static_cast<int>(deg) - k + 1; j++)
					{
						scalar Uleft = U[i + j + 1];
						scalar Uright = U[i + j + k + 1];
						if (close(N[j + 1], 0.0))
						{
							N[j] = saved;
							saved = 0.0;
						}
						else
						{
							scalar temp = N[j + 1] / (Uright - Uleft);
							N[j] = saved + (Uright - u) * temp;
							saved = (u - Uleft) * temp;
						}
					}
				}
				return N[0];
			}

			/**
			 * Compute all non-zero B-spline basis functions
			 * @param[in] deg Degree of the basis function.
			 * @param[in] span Index obtained from findSpan() corresponding the u and knots.
			 * @param[in] knots Knot vector corresponding to the basis functions.
			 * @param[in] u Parameter to evaluate the basis functions at.
			 * @return N Values of (deg+1) non-zero basis functions.
			 */
			inline std::vector<scalar> bspline_basis(size_t deg, int span, const std::vector<scalar>& knots, scalar u)
			{
				if (!(span > 0 && deg >= 1 && deg <= 9 && knots.size() > 0) || (u < knots.front() && (knots.front() - u) > N_SCALAR_EPSILON) ||
					(u > knots.back() && (u - knots.back()) > N_SCALAR_EPSILON))
				{
					return std::vector<scalar>{};
				}

				std::vector<scalar> N;
				N.resize(deg + 1, 0.0f);
				std::vector<scalar> left, right;
				left.resize(deg + 1, 0.0f);
				right.resize(deg + 1, 0.0f);
				scalar saved = 0.0, temp = 0.0;

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

			/**
			 * Compute all non-zero derivatives of B-spline basis functions
			 * @param[in] deg Degree of the basis function.
			 * @param[in] span Index obtained from findSpan() corresponding the u and knots.
			 * @param[in] knots Knot vector corresponding to the basis functions.
			 * @param[in] u Parameter to evaluate the basis functions at.
			 * @param[in] num_ders Number of derivatives to compute (num_ders <= deg)
			 * @return ders Values of non-zero derivatives of basis functions.
			 */
			inline std::vector<std::vector<scalar>> bspline_der_basis(size_t deg, int span, const std::vector<scalar>& knots, scalar u, int num_ders)
			{
				std::vector<scalar> left, right;
				left.resize(deg + 1, 0.0f);
				right.resize(deg + 1, 0.0f);
				scalar saved = 0.0f, temp = 0.0f;
				std::vector<std::vector<scalar>> ndu(deg + 1, std::vector<scalar>(deg + 1, 0.0f));
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

				std::vector<std::vector<scalar>> ders(num_ders + 1, std::vector<scalar>(deg + 1, 0.0f));

				for (int j = 0; j <= static_cast<int>(deg); j++)
				{
					ders[0][j] = ndu[j][deg];
				}

				std::vector<std::vector<scalar>> a(2, std::vector<scalar>(deg + 1, 0.0f));

				for (int r = 0; r <= static_cast<int>(deg); r++)
				{
					int s1 = 0;
					int s2 = 1;
					a[0][0] = 1.0;

					for (int k = 1; k <= num_ders; k++)
					{
						scalar d = 0.0f;
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

				scalar fac = static_cast<scalar>(deg);
				for (int k = 1; k <= num_ders; k++)
				{
					for (int j = 0; j <= static_cast<int>(deg); j++)
					{
						ders[k][j] *= fac;
					}
					fac *= static_cast<scalar>(deg - k);
				}

				return ders;
			}

			namespace internal
			{
				/*
				 *Evaluate the nearest point from  p to linesegment(v0,v1)
				 */
				inline scalar dist_point_line_segment(const vec3& p, const vec3& v0, const vec3& v1, vec3& nearest_point)
				{
					vec3 d1 = p - v0;
					vec3 d2 = v1 - v0;
					scalar t1 = d2.dot(d2);
					nearest_point = v0;
					// v0,v1 not very close
					if (t1 > std::numeric_limits<double>::lowest())
					{
						scalar t2 = d1.dot(d2) / t1;
						if (t2 > 1.0)
						{
							nearest_point = v1;
						}
						else if (t2 > 0.0)
						{
							nearest_point = v0 + d2 * t2;
						}
						else
						{
							nearest_point = v0;
						}
					}
					return (p - nearest_point).norm();
				}
				/*
				 * Evaluate the nearest point from p to triangle(v0,v1,v2)
				 */
				inline scalar dist_point_triangle(const vec3& p, const vec3& v0, const vec3& v1, const vec3& v2, vec3& nearest_point)
				{
					vec3 v0v1 = v1 - v0;
					vec3 v0v2 = v2 - v0;
					vec3 n = v0v1.cross(v0v2);// not normalized !
					scalar d = n.squaredNorm();

					// Check if the triangle is degenerated -> measure dist to line segments
					if (d < std::numeric_limits<double>::lowest())
					{
						vec3 query;
						vec3 nearest;

						auto distance = dist_point_line_segment(p, v0, v1, nearest);

						auto other = dist_point_line_segment(p, v1, v2, query);
						if (other < distance)
						{
							distance = other;
							nearest = query;
						}

						other = dist_point_line_segment(p, v2, v0, query);
						if (other < distance)
						{
							distance = other;
							nearest = query;
						}

						nearest_point = nearest;
						return distance;
					}

					scalar inv_d = 1.0 / d;
					vec3 v1v2 = v2;
					v1v2 -= v1;
					vec3 v0p = p;
					v0p -= v0;
					vec3 t = v0p.cross(n);
					scalar a = t.dot(v0v2) * -inv_d;
					scalar b = t.dot(v0v1) * inv_d;
					scalar s01, s02, s12;

					// Calculate the distance to an edge or a corner vertex
					if (a < 0)
					{
						s02 = v0v2.dot(v0p) / v0v2.squaredNorm();
						if (s02 < 0.0)
						{
							s01 = v0v1.dot(v0p) / v0v1.squaredNorm();
							if (s01 <= 0.0)
							{
								v0p = v0;
							}
							else if (s01 >= 1.0)
							{
								v0p = v1;
							}
							else
							{
								(v0p = v0) += (v0v1 *= s01);
							}
						}
						else if (s02 > 1.0)
						{
							s12 = v1v2.dot(p - v1) / v1v2.squaredNorm();
							if (s12 >= 1.0)
							{
								v0p = v2;
							}
							else if (s12 <= 0.0)
							{
								v0p = v1;
							}
							else
							{
								(v0p = v1) += (v1v2 *= s12);
							}
						}
						else
						{
							(v0p = v0) += (v0v2 *= s02);
						}
					}
					else if (b < 0.0)
					{
						// Calculate the distance to an edge or a corner vertex
						s01 = v0v1.dot(v0p) / v0v1.squaredNorm();
						if (s01 < 0.0)
						{
							s02 = v0v2.dot(v0p) / v0v2.squaredNorm();
							if (s02 <= 0.0)
							{
								v0p = v0;
							}
							else if (s02 >= 1.0)
							{
								v0p = v2;
							}
							else
							{
								(v0p = v0) += (v0v2 *= s02);
							}
						}
						else if (s01 > 1.0)
						{
							s12 = v1v2.dot(p - v1) / v1v2.squaredNorm();
							if (s12 >= 1.0)
							{
								v0p = v2;
							}
							else if (s12 <= 0.0)
							{
								v0p = v1;
							}
							else
							{
								(v0p = v1) += (v1v2 *= s12);
							}
						}
						else
						{
							(v0p = v0) += (v0v1 *= s01);
						}
					}
					else if (a + b > 1.0)
					{
						// Calculate the distance to an edge or a corner vertex
						s12 = v1v2.dot(p - v1) / v1v2.squaredNorm();
						if (s12 >= 1.0)
						{
							s02 = v0v2.dot(v0p) / v0v2.squaredNorm();
							if (s02 <= 0.0)
							{
								v0p = v0;
							}
							else if (s02 >= 1.0)
							{
								v0p = v2;
							}
							else
							{
								(v0p = v0) += (v0v2 *= s02);
							}
						}
						else if (s12 <= 0.0)
						{
							s01 = v0v1.dot(v0p) / v0v1.squaredNorm();
							if (s01 <= 0.0)
							{
								v0p = v0;
							}
							else if (s01 >= 1.0)
							{
								v0p = v1;
							}
							else
							{
								(v0p = v0) += (v0v1 *= s01);
							}
						}
						else
						{
							(v0p = v1) += (v1v2 *= s12);
						}
					}
					else
					{
						// Calculate the distance to an interior point of the triangle
						n *= n.dot(v0p) * inv_d;
						(v0p = p) -= n;
					}

					nearest_point = v0p;
					v0p -= p;
					return v0p.norm();
				}

				class TriangleKdTree
				{
				public:
					TriangleKdTree(const std::vector<vec3>& pos_arr, const std::vector<size_t>& id_arr, size_t max_faces = 10, size_t max_depth = 30);
					~TriangleKdTree() { delete root; }

					struct NearestNeighbor
					{
						scalar dist;
						vec3 nearest;
					};
					NearestNeighbor nearest(const vec3& p);

				private:
					struct Node
					{
						Node() = default;
						~Node()
						{
							delete faces;
							delete left;
							delete right;
						}
						unsigned char axis;
						scalar split;
						std::vector<size_t>* faces = nullptr;
						Node* left = nullptr;
						Node* right = nullptr;
					};
					void build_recurse(Node* node, size_t max_faces, size_t depth);

					void nearest_recurse(Node* node, const vec3& point, NearestNeighbor& data) const;
					const std::vector<size_t>& index_arr;
					const std::vector<vec3>& point_arr;
					Node* root;
				};
				inline TriangleKdTree::TriangleKdTree(const std::vector<vec3>& pos_arr, const std::vector<size_t>& id_arr, size_t max_faces, size_t max_depth)
					: point_arr(pos_arr), index_arr(id_arr)
				{
					int triangle_num = index_arr.size() / 3;
					root = new Node();
					root->faces = new std::vector<size_t>();
					root->faces->reserve(triangle_num);
					for (int id = 0; id < triangle_num; id++)
					{
						(*root->faces)[id] = id;
					}
					build_recurse(root, max_faces, max_depth);
				}
				inline TriangleKdTree::NearestNeighbor TriangleKdTree::nearest(const vec3& p)
				{
					NearestNeighbor data;
					data.dist = std::numeric_limits<float>::max();
					nearest_recurse(root, p, data);
					return data;
				}
				inline void TriangleKdTree::build_recurse(Node* node, size_t max_faces, size_t depth)
				{
					// stop at this level if must
					if ((depth == 0) || (node->faces->size() <= max_faces))
						return;

					// compute the bounding box of this node
					AABB3 bbox;
					for (const auto& triangle_id : *node->faces)
					{
						bbox.extend(point_arr[index_arr[triangle_id * 3]]);
						bbox.extend(point_arr[index_arr[triangle_id * 3 + 1]]);
						bbox.extend(point_arr[index_arr[triangle_id * 3 + 2]]);
					}

					// split longest side of bounding box
					vec3 bb = bbox.max() - bbox.min();
					int axis = bb[0] > bb[1] ? 0 : 1;
					axis = bb[axis] > bb[2] ? axis : 2;
					// split in the middle
					scalar split = bbox.center()[axis];

					// create children
					auto* left = new Node();
					left->faces = new std::vector<size_t>();
					left->faces->reserve(node->faces->size() / 2);
					auto* right = new Node();
					right->faces = new std::vector<size_t>();
					right->faces->reserve(node->faces->size() / 2);

					// partition for left and right child
					for (const auto& triangle_id : *node->faces)
					{
						bool l = false, r = false;

						if (point_arr[index_arr[3 * triangle_id]][axis] <= split)
							l = true;
						else
							r = true;
						if (point_arr[index_arr[3 * triangle_id + 1]][axis] <= split)
							l = true;
						else
							r = true;
						if (point_arr[index_arr[3 * triangle_id + 2]][axis] <= split)
							l = true;
						else
							r = true;

						if (l)
						{
							left->faces->push_back(triangle_id);
						}

						if (r)
						{
							right->faces->push_back(triangle_id);
						}
					}

					// stop here if a bad partition.
					if (left->faces->size() == node->faces->size() || right->faces->size() == node->faces->size())
					{
						// compact memory allocated by its parent.
						node->faces->shrink_to_fit();
						// delete new nodes
						delete left;
						delete right;
						// stop recursion
						return;
					}
					else
					{
						// not leaf node then free memory
						delete node->faces;
						node->faces = nullptr;
						// store internal data
						node->axis = axis;
						node->split = split;
						node->left = left;
						node->right = right;
						// recurse to children
						build_recurse(node->left, max_faces, depth - 1);
						build_recurse(node->right, max_faces, depth - 1);
					}
				}
				inline void TriangleKdTree::nearest_recurse(Node* node, const vec3& point, NearestNeighbor& data) const
				{
					// leaf node
					if (!node->left)
					{
						for (const auto& triangle_id : *node->faces)
						{
							vec3 n;
							auto d = dist_point_triangle(point, point_arr[index_arr[3 * triangle_id]], point_arr[index_arr[3 * triangle_id + 1]],
								point_arr[index_arr[3 * triangle_id + 2]], n);
							if (d < data.dist)
							{
								data.dist = d;
								data.nearest = n;
							}
						}
					}
					else
					{// not leaf node
						scalar dist = point[node->axis] - node->split;

						if (dist <= 0.0)
						{
							nearest_recurse(node->left, point, data);
							if (fabs(dist) < data.dist)
								nearest_recurse(node->right, point, data);
						}
						else
						{
							nearest_recurse(node->right, point, data);
							if (fabs(dist) < data.dist)
								nearest_recurse(node->left, point, data);
						}
					}
				}
			}// namespace internal


		}// namespace util

	}// namespace nurbs
}// namespace nous