#include<glad/glad.h>
#include "core/graph.h"
#include "core/ServiceLocator.h"
#include "nodes/parametrization_node.h"
#include "algorithm/parametrization.h"
#include "algorithm/mesh_triangulate.h"
namespace Geomerty {
	void Parametrization_Node::InstallUi()
	{
		bool flag = false;
		flag |= ImGui::Checkbox("Use Uniform lapace", &use_uniform);
	}
	void Parametrization_Node::Init(Graph* graph)
	{
		Inputs.emplace_back(GetNextId(), "InputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Input);
		Inputs.back().Node = this;
		Outputs.emplace_back(GetNextId(), "OutputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };

	}
	void Parametrization_Node::Execute(ExetContex* ctx)
	{
		auto InputMesh = ctx->inputs[0]->Get<SurfaceMesh>();
		if (InputMesh) {
			SurfaceMesh* mesh = new SurfaceMesh(*InputMesh);
			harmonic_parameterization(*mesh, use_uniform);
			ctx->graph->registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
			std::cout << "Parametrization_mesh" << std::endl;
		}
	}
	void Parametrization_Node::Present(Geomerty::Viewer& viewer)
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
			// get properties
			auto tex = mesh->vertex_property<TexCoord>("v:tex").vector();
			Eigen::MatrixXd UV;
			UV.resize(pos.size(), 2);
			for (int i = 0; i < tex.size(); i++) {
				UV.row(i) << tex[i][0], tex[i][1];
			}
			viewer.data(0).show_texture = true;
			viewer.data(0).set_uv(UV);
			std::string vert =
				R"(#version 150
			  in vec3 position;
			  in vec3 normal;
			  in vec4 Ka;
			  in vec4 Kd;
			  in vec4 Ks;
			  in vec2 texcoord;
              uniform vec2 ratio;
			  void main()
			  {
               
				gl_Position =  vec4 (1.0+(texcoord.x-1.0)/ratio.x-ratio.y/ratio.x,texcoord.y-ratio.y,-1.0, 1.0); //proj * view * vec4(position, 1.0);
			  }
			)";
			std::string frag =
				R"(#version 150
			  out vec4 outColor;
			  void main()
			  {
                 outColor= vec4(1.0,0.72,0.50,1.0);
			  }
			)";

			viewer.data(0).meshgl.AddShader(vert, "", frag, [](GLuint program_id, Geomerty::MeshGL& mesh_gl, ViewerCore& view_core) {
				auto w = view_core.viewport[2];
				auto h = view_core.viewport[3];
				GLint ratioi = glGetUniformLocation(program_id, "ratio");
				glUniform2f(ratioi, w / h, 10.0f / h);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, 3 * mesh_gl.F_vbo.rows(), GL_UNSIGNED_INT, 0);
				glDisable(GL_POLYGON_OFFSET_FILL);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				});
		}
	}
}