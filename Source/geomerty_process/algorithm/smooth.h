#pragma once
#include"core/surface_mesh.h"

namespace Geomerty {

	void explicit_smoothing(SurfaceMesh& mesh, unsigned int iterations = 10,
		bool use_uniform_laplace = false);

	void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep = 0.001,
		unsigned int iterations = 1,
		bool use_uniform_laplace = false, bool rescale = true);
}