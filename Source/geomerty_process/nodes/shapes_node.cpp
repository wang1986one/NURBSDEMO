#include "core/ServiceLocator.h"
#include"shapes_node.h"
#include "core/graph.h"
#include "panels/ControllerView.h"
#include "algorithm/mesh_triangulate.h"
#include "UI/Panels/PanelsManager.h"
using namespace Geomerty;
static Point centroid(const SurfaceMesh& mesh, Face f)
{
	Point c(0, 0, 0);
	Scalar n(0);
	for (auto v : mesh.vertices(f))
	{
		c += mesh.position(v);
		++n;
	}
	c /= n;
	return c;
}
static void dual(SurfaceMesh& mesh)
{
	// the new dualized mesh
	SurfaceMesh tmp;

	// remember new vertices per face
	auto fvertex = mesh.add_face_property<Vertex>("f:vertex");

	// add centroid for each face
	for (auto f : mesh.faces())
		fvertex[f] = tmp.add_vertex(centroid(mesh, f));

	// add new face for each vertex
	for (auto v : mesh.vertices())
	{
		std::vector<Vertex> vertices;
		for (auto f : mesh.faces(v))
			vertices.push_back(fvertex[f]);

		tmp.add_face(vertices);
	}

	// swap old and new meshes, don't copy properties
	mesh.assign(tmp);
}
static void project_to_unit_sphere(SurfaceMesh& mesh)
{
	for (auto v : mesh.vertices())
	{
		auto p = mesh.position(v);
		auto n = norm(p);
		mesh.position(v) = (1.0 / n) * p;
	}
}
static Geomerty::SurfaceMesh* tetrahedron()
{
	Geomerty::SurfaceMesh* tetrahedron_mesh = new Geomerty::SurfaceMesh();
	float a = 1.0f / 3.0f;
	float b = sqrt(8.0f / 9.0f);
	float c = sqrt(2.0f / 9.0f);
	float d = sqrt(2.0f / 3.0f);
	auto v0 = tetrahedron_mesh->add_vertex(Geomerty::Point(0, 0, 1));
	auto v1 = tetrahedron_mesh->add_vertex(Geomerty::Point(-c, d, -a));
	auto v2 = tetrahedron_mesh->add_vertex(Geomerty::Point(-c, -d, -a));
	auto v3 = tetrahedron_mesh->add_vertex(Geomerty::Point(b, 0, -a));
	tetrahedron_mesh->add_triangle(v0, v1, v2);
	tetrahedron_mesh->add_triangle(v0, v2, v3);
	tetrahedron_mesh->add_triangle(v0, v3, v1);
	tetrahedron_mesh->add_triangle(v3, v2, v1);
	return tetrahedron_mesh;
}
static Geomerty::SurfaceMesh* hexahedron()
{
	SurfaceMesh* hexahedron_mesh = new Geomerty::SurfaceMesh();
	float a = 1.0f / sqrt(3.0f);
	auto v0 = hexahedron_mesh->add_vertex(Point(-a, -a, -a));
	auto v1 = hexahedron_mesh->add_vertex(Point(a, -a, -a));
	auto v2 = hexahedron_mesh->add_vertex(Point(a, a, -a));
	auto v3 = hexahedron_mesh->add_vertex(Point(-a, a, -a));
	auto v4 = hexahedron_mesh->add_vertex(Point(-a, -a, a));
	auto v5 = hexahedron_mesh->add_vertex(Point(a, -a, a));
	auto v6 = hexahedron_mesh->add_vertex(Point(a, a, a));
	auto v7 = hexahedron_mesh->add_vertex(Point(-a, a, a));
	hexahedron_mesh->add_quad(v3, v2, v1, v0);
	hexahedron_mesh->add_quad(v2, v6, v5, v1);
	hexahedron_mesh->add_quad(v5, v6, v7, v4);
	hexahedron_mesh->add_quad(v0, v4, v7, v3);
	hexahedron_mesh->add_quad(v3, v7, v6, v2);
	hexahedron_mesh->add_quad(v1, v5, v4, v0);
	return hexahedron_mesh;
}
Geomerty::SurfaceMesh* octahedron()
{
	SurfaceMesh* octahedron_mesh = hexahedron();
	dual(*octahedron_mesh);
	project_to_unit_sphere(*octahedron_mesh);
	return octahedron_mesh;
}
SurfaceMesh* icosahedron()
{
	SurfaceMesh* icosahedron_mesh = new SurfaceMesh();
	float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
	float a = 1.0f;
	float b = 1.0f / phi;
	auto v1 = icosahedron_mesh->add_vertex(Point(0, b, -a));
	auto v2 = icosahedron_mesh->add_vertex(Point(b, a, 0));
	auto v3 = icosahedron_mesh->add_vertex(Point(-b, a, 0));
	auto v4 = icosahedron_mesh->add_vertex(Point(0, b, a));
	auto v5 = icosahedron_mesh->add_vertex(Point(0, -b, a));
	auto v6 = icosahedron_mesh->add_vertex(Point(-a, 0, b));
	auto v7 = icosahedron_mesh->add_vertex(Point(0, -b, -a));
	auto v8 = icosahedron_mesh->add_vertex(Point(a, 0, -b));
	auto v9 = icosahedron_mesh->add_vertex(Point(a, 0, b));
	auto v10 = icosahedron_mesh->add_vertex(Point(-a, 0, -b));
	auto v11 = icosahedron_mesh->add_vertex(Point(b, -a, 0));
	auto v12 = icosahedron_mesh->add_vertex(Point(-b, -a, 0));
	project_to_unit_sphere(*icosahedron_mesh);
	icosahedron_mesh->add_triangle(v3, v2, v1);
	icosahedron_mesh->add_triangle(v2, v3, v4);
	icosahedron_mesh->add_triangle(v6, v5, v4);
	icosahedron_mesh->add_triangle(v5, v9, v4);
	icosahedron_mesh->add_triangle(v8, v7, v1);
	icosahedron_mesh->add_triangle(v7, v10, v1);
	icosahedron_mesh->add_triangle(v12, v11, v5);
	icosahedron_mesh->add_triangle(v11, v12, v7);
	icosahedron_mesh->add_triangle(v10, v6, v3);
	icosahedron_mesh->add_triangle(v6, v10, v12);
	icosahedron_mesh->add_triangle(v9, v8, v2);
	icosahedron_mesh->add_triangle(v8, v9, v11);
	icosahedron_mesh->add_triangle(v3, v6, v4);
	icosahedron_mesh->add_triangle(v9, v2, v4);
	icosahedron_mesh->add_triangle(v10, v3, v1);
	icosahedron_mesh->add_triangle(v2, v8, v1);
	icosahedron_mesh->add_triangle(v12, v10, v7);
	icosahedron_mesh->add_triangle(v8, v11, v7);
	icosahedron_mesh->add_triangle(v6, v12, v5);
	icosahedron_mesh->add_triangle(v11, v9, v5);
	return icosahedron_mesh;
}
static SurfaceMesh* dodecahedron()
{
	SurfaceMesh* dodecahedron_mesh = icosahedron();
	dual(*dodecahedron_mesh);
	project_to_unit_sphere(*dodecahedron_mesh);
	return dodecahedron_mesh;
}
enum ShapeKind
{
	Tetrahedron,
	Hexahedron,
	Octahedron,
	Icosahedron,
	Dodecahedron
};

namespace Geomerty {
	void ShapeNode::InstallUi()
	{
		const char* items[] = { "Tetrahedron", "Hexahedron", "Octahedron", "Icosahedron", "Dodecahedron" };
		const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo("Shape Kind", combo_preview_value))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					SurfaceMesh* mesh = nullptr;
					ShapeKind kind = static_cast<ShapeKind>(item_current_idx);
					switch (kind)
					{
					case Tetrahedron:
						mesh = tetrahedron();
						break;
					case Hexahedron:
						mesh = hexahedron();
						break;
					case Octahedron:
						mesh = octahedron();
						break;
					case Icosahedron:
						mesh = icosahedron();
						break;
					case Dodecahedron:
						mesh = dodecahedron();
						break;
					default:
						break;
					}
					auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
					auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
					registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
					Present(input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	void ShapeNode::Init(Graph* graph)
	{
		Outputs.emplace_back(GetNextId(), "Output_Mesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };
	}
	void ShapeNode::Execute(ExetContex* ctx)
	{
		SurfaceMesh* mesh = nullptr;
		ShapeKind kind = static_cast<ShapeKind>(item_current_idx);
		switch (kind)
		{
		case Tetrahedron:
			mesh = tetrahedron();
			break;
		case Hexahedron:
			mesh = hexahedron();
			break;
		case Octahedron:
			mesh = octahedron();
			break;
		case Icosahedron:
			mesh = icosahedron();
			break;
		case Dodecahedron:
			mesh = dodecahedron();
			break;
		default:
			break;
		}
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		ctx->graph->registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
	}
	void ShapeNode::Present(opengl::glfw::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto mesh = registry[Outputs.back().index].Get<SurfaceMesh>();
		if (mesh != nullptr) {
			bool is_triangle = mesh->is_triangle_mesh();
			SurfaceMesh* present_mesh = mesh;
			if (!is_triangle) {
				present_mesh = new SurfaceMesh(*mesh);
				Geomerty::triangulate(*present_mesh);
			}
			Eigen::MatrixXd SV;
			auto& pos = present_mesh->positions();
			SV.resize(pos.size(), 3);
			for (int i = 0; i < pos.size(); i++) {
				SV.row(i) << pos[i][0], pos[i][1], pos[i][2];
			}
			Eigen::MatrixXi SF;
			SF.resize(present_mesh->faces_size(), 3);
			auto& faces = present_mesh->faces();
			int j = 0, x, y, z;
			for (auto f : faces) {
				auto it = present_mesh->vertices(f).begin();
				x = (*it++).idx();
				y = (*it++).idx();
				z = (*it++).idx();
				SF.row(j++) << x, y, z;
			}
			viewer.data(0).clear();
			viewer.data(0).set_mesh(SV, SF);
		}
	}
}