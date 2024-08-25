#pragma once

#include <filesystem>

#include "core/surface_mesh.h"

namespace Geomerty {

	void read_obj(SurfaceMesh& mesh, const std::filesystem::path& file);
	void read_off(SurfaceMesh& mesh, const std::filesystem::path& file);
	void read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
		const bool has_texcoords, const bool has_colors,
		char* first_line);
	void read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
		const bool has_texcoords, const bool has_colors,
		const std::filesystem::path& file);

} // namespace pmp