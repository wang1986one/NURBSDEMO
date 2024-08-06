#pragma once

#include <filesystem>

#include "surface_mesh.h"

namespace Geomerty {

	void read_obj(SurfaceMesh& mesh, const std::filesystem::path& file);

} // namespace pmp