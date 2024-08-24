#pragma once
#include "core/surface_mesh.h"
namespace Geomerty {

	void harmonic_parameterization(SurfaceMesh& mesh,
		bool use_uniform_weights = false);
	void lscm_parameterization(SurfaceMesh& mesh);
}