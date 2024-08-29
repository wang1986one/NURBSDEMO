#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "nurbs/nurbs_curve.h"
#include "nurbs/nurbs_surface.h"

namespace Geomerty
{
	namespace nurbs
	{
		namespace util
		{
			using scalar = float;
			using vec3 = Eigen::Vector3<scalar>;



			inline void curve_read_obj(std::istream& is, size_t& deg, std::vector<scalar>& knots,
				std::vector<vec3>& ctrlPts, std::vector<scalar>& weights, bool& rational)
			{
				scalar knot_min = 0, knot_max = 1;
				std::vector<vec3> ctrl_pts_buf;
				std::vector<scalar> weights_buf;
				std::vector<int> indices;
				std::vector<scalar> temp_knots;

				std::string start, token, sline;
				std::istringstream ssline;

				struct ToParse
				{
					bool deg, cstype, curv, parm;
				};

				ToParse parsed;

				while (std::getline(is, sline))
				{
					if (sline.size() == 0)
					{
						continue;
					}
					ssline.str(sline);
					ssline >> start;
					if (start == "v")
					{
						std::vector<double> four_coords;
						four_coords.resize(4, 0.0);
						four_coords[3] = 1.0;
						int index = 0;
						while (ssline && index <= 3)
						{
							ssline >> four_coords[index++];
						}
						ctrl_pts_buf.emplace_back(four_coords[0], four_coords[1], four_coords[2]);
						weights_buf.push_back(four_coords[3]);
					}
					else if (start == "cstype")
					{
						std::string token1;
						ssline >> token1;
						if (token1 == "bspline")
						{
							rational = false;
							parsed.cstype = true;
						}
						else if (token1 == "rat")
						{
							std::string token2;
							ssline >> token2;
							if (token2 == "bspline")
							{
								rational = true;
								parsed.cstype = true;
							}
						}
					}
					else if (start == "deg")
					{
						ssline >> deg;
						parsed.deg = true;
					}
					else if (start == "curv")
					{
						ssline >> knot_min >> knot_max;
						while (ssline >> token)
						{
							if (token == "\\")
							{
								ssline.clear();
								getline(is, sline);
								ssline.str(sline);
							}
							else
							{
								indices.push_back(std::stof(token));
							}
						}
						parsed.curv = true;
					}
					else if (start == "parm")
					{
						ssline >> start;
						if (start == "u")
						{
							while (ssline >> token)
							{
								if (token == "\\")
								{
									ssline.clear();
									std::getline(is, sline);
									ssline.str(sline);
								}
								else
								{
									temp_knots.push_back(std::stof(token));
								}
							}
						}
						parsed.parm = true;
					}
					else if (start == "end")
					{
						break;
					}
					ssline.clear();
				}

				// Check if necessary data was available in stream
				if (!parsed.cstype)
				{
					throw std::runtime_error("'cstype bspline / cstype rat bspline' line missing in file");
				}
				if (!parsed.deg)
				{
					throw std::runtime_error("'deg' line missing/incomplete in file");
				}
				if (!parsed.curv)
				{
					throw std::runtime_error("'curv' line missing/incomplete in file");
				}
				if (!parsed.parm)
				{
					throw std::runtime_error("'parm' line missing/incomplete in file");
				}

				int num_knots = temp_knots.size();
				int num_cp = num_knots - deg - 1;

				ctrlPts.resize(num_cp);
				weights.resize(num_cp);
				size_t num = 0;
				for (int i = 0; i < num_cp; ++i)
				{
					assert(i < ctrlPts.size());
					ctrlPts[i] = ctrl_pts_buf[indices[num] - 1];
					weights[i] = weights_buf[indices[num] - 1];
					++num;
				}

				knots = temp_knots;
			}
			inline  RationalCurve curve_read_obj(const std::string& filename)
			{
				std::ifstream file(filename);
				if (!file)
				{
					throw std::runtime_error("File not found: " + filename);
				}

				RationalCurve crv;
				std::vector<vec3> control_points;
				bool rat;
				curve_read_obj(file, crv.m_degree, crv.m_knots, crv.m_control_points, crv.m_weights, rat);
				return crv;
			}
			inline void curve_write_obj(std::ostream& os, unsigned int degree, const std::vector<scalar>& knots,
				const std::vector<vec3>& ctrlPts, const std::vector<scalar>& weights,
				bool rational)
			{
				using std::endl;

				for (int i = 0; i < ctrlPts.size(); ++i)
				{
					os << "v " << ctrlPts[i][0] << " " << ctrlPts[i][1] << " " << ctrlPts[i][2] << " "
						<< weights[i] << endl;
				}

				int n_knots = knots.size();
				int n_cp = ctrlPts.size();

				if (!rational)
				{
					os << "cstype bspline" << endl;
				}
				else
				{
					os << "cstype rat bspline" << endl;
				}
				os << "deg " << degree << endl << "curv ";
				os << knots[degree] << " " << knots[n_knots - degree - 1];
				for (int i = 0; i < n_cp; ++i)
				{
					os << " " << i + 1;
				}
				os << endl << "parm u";
				for (auto knot : knots)
				{
					os << " " << knot;
				}
				os << endl << "end";
			}

			inline void curve_write_obj(std::ostream& os, const RationalCurve& crv)
			{
				curve_write_obj(os, crv.m_degree, crv.m_knots, crv.m_control_points, crv.m_weights, true);
			}
			inline void curve_write_obj(const std::string& filename, const RationalCurve& crv) {
				std::ofstream fout(filename);
				curve_write_obj(fout, crv);
			}



			inline void surface_write_obj(std::ostream& os, unsigned int deg_u, unsigned int deg_v,
				const std::vector<scalar>& knots_u, const std::vector<scalar>& knots_v,
				const std::vector<std::vector<vec3>>& ctrlPts, const std::vector<std::vector<scalar>>& weights, bool rational)
			{

				using std::endl;

				if (ctrlPts.size() == 0 || ctrlPts[0].size() == 0)
				{
					return;
				}

				for (int j = 0; j < ctrlPts[0].size(); j++)
				{
					for (int i = 0; i < ctrlPts.size(); i++)
					{
						os << "v " << ctrlPts[i][j][0] << " " << ctrlPts[i][j][1] << " " << ctrlPts[i][j][2]
							<< " " << weights[i][j] << endl;
					}
				}

				int nknots_u = knots_u.size();
				int nknots_v = knots_v.size();

				int nCpU = ctrlPts.size();
				int nCpV = ctrlPts[0].size();

				if (!rational)
				{
					os << "cstype bspline" << endl;
				}
				else
				{
					os << "cstype rat bspline" << endl;
				}
				os << "deg " << deg_u << " " << deg_v << endl << "surf ";
				os << knots_u[deg_u] << " " << knots_u[nknots_u - deg_u - 1] << " " << knots_v[deg_v] << " "
					<< knots_v[nknots_v - deg_v - 1];
				for (int i = 0; i < nCpU * nCpV; i++)
				{
					os << " " << i + 1;
				}
				os << endl << "parm u";
				for (auto knot : knots_u)
				{
					os << " " << knot;
				}
				os << endl << "parm v";
				for (auto knot : knots_v)
				{
					os << " " << knot;
				}
				os << endl << "end";
			}
			inline void surface_write_obj(std::ostream& os, const RationalSurface& srf)
			{
				surface_write_obj(os, srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v,
					srf.m_control_points, srf.m_weights, true);
			}

			inline void surface_write_obj(const std::string& filename, const RationalSurface& srf)
			{
				std::ofstream fout(filename);
				surface_write_obj(fout, srf);
			}

			inline void surfaceReadOBJ(std::istream& is, size_t& deg_u, size_t& deg_v,
				std::vector<scalar>& knots_u, std::vector<scalar>& knots_v,
				std::vector<std::vector<vec3>>& ctrlPts, std::vector<std::vector<scalar>>& weights, bool& rational)
			{
				scalar uknot_min = 0, uknot_max = 1;
				scalar vknot_min = 0, vknot_max = 1;

				std::vector<vec3> ctrl_pts_buf;
				std::vector<scalar> weights_buf;
				std::vector<int> indices;
				std::vector<scalar> temp_uknots;
				std::vector<scalar> temp_vknots;

				std::string start, token, sline;
				std::istringstream ssline;

				struct ToParse
				{
					bool deg, cstype, surf, parm;
				};

				ToParse parsed;

				while (std::getline(is, sline))
				{
					if (sline.size() == 0)
					{
						break;
					}
					ssline.str(sline);
					ssline >> start;
					if (start == "v")
					{
						std::vector<double> four_coords;
						four_coords.resize(4);
						four_coords[3] = 1.0;
						int index = 0;
						while (ssline && index <= 3)
						{
							ssline >> four_coords[index++];
						}
						ctrl_pts_buf.emplace_back(four_coords[0], four_coords[1], four_coords[2]);
						weights_buf.push_back(four_coords[3]);
					}
					else if (start == "cstype")
					{
						std::string token1;
						ssline >> token1;
						if (token1 == "bspline")
						{
							rational = false;
							parsed.cstype = true;
						}
						else if (token1 == "rat")
						{
							std::string token2;
							ssline >> token2;
							if (token2 == "bspline")
							{
								rational = true;
								parsed.cstype = true;
							}
						}
					}
					else if (start == "deg")
					{
						ssline >> deg_u >> deg_v;
						parsed.deg = true;
					}
					else if (start == "surf")
					{
						ssline >> uknot_min >> uknot_max >> vknot_min >> vknot_max;
						while (ssline >> token)
						{
							if (token == "\\")
							{
								ssline.clear();
								getline(is, sline);
								ssline.str(sline);
							}
							else
							{
								indices.push_back(std::stof(token));
							}
						}
						parsed.surf = true;
					}
					else if (start == "parm")
					{
						ssline >> start;
						if (start == "u")
						{
							while (ssline >> token)
							{
								if (token == "\\")
								{
									ssline.clear();
									std::getline(is, sline);
									ssline.str(sline);
								}
								else
								{
									temp_uknots.push_back(std::stof(token));
								}
							}
						}
						else if (start == "v")
						{
							while (ssline >> token)
							{
								if (token == "\\")
								{
									ssline.clear();
									std::getline(is, sline);
									ssline.str(sline);
								}
								else
								{
									temp_vknots.push_back(std::stof(token));
								}
							}
						}
						parsed.parm = true;
					}
					else if (start == "end")
					{
						break;
					}
					ssline.clear();
				}

				// Check if necessary data was available in stream
				if (!parsed.cstype)
				{
					throw std::runtime_error("'cstype bspline / cstype rat bspline' line missing in file");
				}
				if (!parsed.deg)
				{
					throw std::runtime_error("'deg' line missing/incomplete in file");
				}
				if (!parsed.surf)
				{
					throw std::runtime_error("'surf' line missing/incomplete in file");
				}
				if (!parsed.parm)
				{
					throw std::runtime_error("'parm' line missing/incomplete in file");
				}

				int num_knots_u = temp_uknots.size();
				int num_knots_v = temp_vknots.size();
				int num_cp_u = num_knots_u - deg_u - 1;
				int num_cp_v = num_knots_v - deg_v - 1;

				ctrlPts = std::vector<std::vector<vec3>>(num_cp_u, std::vector<vec3>(num_cp_v, vec3::Zero()));
				weights = std::vector<std::vector<scalar>>(num_cp_u, std::vector<scalar>(num_cp_v, 0.0));
				size_t num = 0;
				for (int j = 0; j < num_cp_v; ++j)
				{
					for (int i = 0; i < num_cp_u; ++i)
					{
						assert(i < ctrlPts.size() && j < ctrlPts[0].size());
						ctrlPts[i][j] = ctrl_pts_buf[indices[num] - 1];
						weights[i][j] = weights_buf[indices[num] - 1];
						++num;
					}
				}

				knots_u = temp_uknots;
				knots_v = temp_vknots;
			}
			inline  RationalSurface surfaceReadOBJ(const std::string& filename)
			{
				std::ifstream file(filename);
				if (!file)
				{
					throw std::runtime_error("File not found: " + filename);
				}

				RationalSurface srf;

				bool rat;
				surfaceReadOBJ(file, srf.m_degree_u, srf.m_degree_v, srf.m_knots_u, srf.m_knots_v,
					srf.m_control_points, srf.m_weights, rat);
				return srf;
			}


		}// namespace util

	}// namespace nurbs
}// namespace nous