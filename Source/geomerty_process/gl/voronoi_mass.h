#pragma once
#include <Eigen/Core>

/// Compute the mass matrix entries for a given tetrahedral mesh (V,T) using the
/// "hybrid" voronoi volume of each vertex.
/// 
///  @param[in] V  #V by 3 list of vertex positions
///  @param[in] T  #T by 4 list of element indices into V
///  @param[out] M  #V list of mass matrix diagonal entries (will be positive as
///    long as tets are not degenerate)
///
template <
  typename DerivedV,
  typename DerivedT,
  typename DerivedM>
 void voronoi_mass(
    const Eigen::MatrixBase<DerivedV> & V,
    const Eigen::MatrixBase<DerivedT> & T,
    Eigen::PlainObjectBase<DerivedM> & M);

