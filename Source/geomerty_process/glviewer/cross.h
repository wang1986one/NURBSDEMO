#pragma
#include <Eigen/Core>

  /// Cross product; out = cross(a,b)
  ///
  /// @param[in] a  left 3d vector
  /// @param[in] b  right 3d vector
  /// @param[out] out  result 3d vector
   void cross( const double *a, const double *b, double *out);
  /// Computes rowwise cross product C = cross(A,B,2);
  ///
  /// @param[in] A  #A by 3 list of row-vectors
  /// @param[in] B  #A by 3 list of row-vectors
  /// @param[out] C  #A by 3 list of row-vectors
  template <
    typename DerivedA,
    typename DerivedB,
    typename DerivedC>
   void cross(
    const Eigen::PlainObjectBase<DerivedA> & A,
    const Eigen::PlainObjectBase<DerivedB> & B,
    Eigen::PlainObjectBase<DerivedC> & C);

