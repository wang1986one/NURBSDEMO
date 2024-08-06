#include "read_mesh.h"
#include "exceptions.h"
namespace Geomerty {

	void read_obj(SurfaceMesh& mesh, const std::filesystem::path& file)
	{
		std::array<char, 200> s;
		float x, y, z;
		std::vector<Vertex> vertices;
		std::vector<TexCoord> all_tex_coords;
		std::vector<int>
			halfedge_tex_idx;
		HalfedgeProperty<TexCoord> tex_coords =
			mesh.halfedge_property<TexCoord>("h:tex");
		bool with_tex_coord = false;


		FILE* in = fopen(file.string().c_str(), "r");
		if (!in)
			throw IOException("Failed to open file: " + file.string());


		memset(s.data(), 0, 200);


		while (in && !feof(in) && fgets(s.data(), 200, in))
		{

			if (s[0] == '#' || isspace(s[0]))
				continue;

			else if (strncmp(s.data(), "v ", 2) == 0)
			{
				if (sscanf(s.data(), "v %f %f %f", &x, &y, &z))
				{
					mesh.add_vertex(Point(x, y, z));
				}
			}

			else if (strncmp(s.data(), "vn ", 3) == 0)
			{
				if (sscanf(s.data(), "vn %f %f %f", &x, &y, &z))
				{
					// problematic as it can be either a vertex property when interpolated
					// or a halfedge property for hard edges
				}
			}

			else if (strncmp(s.data(), "vt ", 3) == 0)
			{
				if (sscanf(s.data(), "vt %f %f", &x, &y))
				{
					all_tex_coords.emplace_back(x, y);
				}
			}

			else if (strncmp(s.data(), "f ", 2) == 0)
			{
				int component(0);
				bool end_of_vertex(false);
				char* p0, * p1(s.data() + 1);

				vertices.clear();
				halfedge_tex_idx.clear();

				while (*p1 == ' ')
					++p1;

				while (p1)
				{
					p0 = p1;

					while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' ' &&
						*p1 != '\0')
						++p1;

					if (*p1 != '/')
					{
						end_of_vertex = true;
					}

					if (*p1 != '\0')
					{
						*p1 = '\0';
						p1++;
					}

					if (*p1 == '\0' || *p1 == '\n')
					{
						p1 = nullptr;
					}

					if (*p0 != '\0')
					{
						switch (component)
						{
						case 0:
						{
							int idx = atoi(p0);
							if (idx < 0)
								idx = mesh.n_vertices() + idx + 1;
							vertices.emplace_back(idx - 1);
							break;
						}
						case 1:
						{
							int idx = atoi(p0) - 1;
							halfedge_tex_idx.push_back(idx);
							with_tex_coord = true;
							break;
						}
						case 2:
							break;
						}
					}
					++component;
					if (end_of_vertex)
					{
						component = 0;
						end_of_vertex = false;
					}
				}

				Face f;
				try
				{
					f = mesh.add_face(vertices);
				}
				catch (const TopologyException& e)
				{
					std::cerr << e.what();
				}

				if (with_tex_coord && f.is_valid())
				{
					auto h_fit = mesh.halfedges(f);
					auto h_end = h_fit;
					unsigned v_idx = 0;
					do
					{
						tex_coords[*h_fit] =
							all_tex_coords.at(halfedge_tex_idx.at(v_idx));
						++v_idx;
						++h_fit;
					} while (h_fit != h_end);
				}
			}
			memset(s.data(), 0, 200);
		}

		if (!with_tex_coord)
		{
			mesh.remove_halfedge_property(tex_coords);
		}
		fclose(in);
	}
}