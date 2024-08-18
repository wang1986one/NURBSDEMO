#pragma once

#include <filesystem>

#include "core/surface_mesh.h"

namespace Geomerty {

	void read_obj(SurfaceMesh& mesh, const std::filesystem::path& file);

} // namespace pmp