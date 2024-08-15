#pragma once
#include "nous_base/definitions.h"

#ifdef NOUS_NURBS_EXPORTS
#define NOUS_NURBS_API NOUS_DLL_EXPORT
#else
#define NOUS_NURBS_API NOUS_DLL_IMPORT
#endif
