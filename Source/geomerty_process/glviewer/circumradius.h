
#include <Eigen/Core>

  /// Compute the circumradius of each triangle in a mesh (V,F)
  ///
  /// @param[in] V  #V by dim list of mesh vertex positions
  /// @param[in] F  #F by 3 list of triangle indices into V
  /// @param[out] R  #F list of circumradius
  ///
  template <
    typename DerivedV, 
    typename DerivedF,
    typename DerivedR>
   void circumradius(
    const Eigen::MatrixBase<DerivedV> & V, 
    const Eigen::MatrixBase<DerivedF> & F,
    Eigen::PlainObjectBase<DerivedR> & R);
  /// Generic version 
  ///
  /// @param[in] V  #V by dim list of mesh vertex positions
  /// @param[in] T  #T by simplex-size list of simplex indices into V
  /// @param[out] R  #T list of circumradius
  /// @param[out] C  #T by dim list of circumcenter
  /// @param[out] B  #T by simplex-size list of barycentric coordinates of circumcenter
  template <
    typename DerivedV, 
    typename DerivedT,
    typename DerivedR,
    typename DerivedC,
    typename DerivedB>
   void circumradius(
    const Eigen::MatrixBase<DerivedV> & V, 
    const Eigen::MatrixBase<DerivedT> & T,
    Eigen::PlainObjectBase<DerivedR> & R,
    Eigen::PlainObjectBase<DerivedC> & C,
    Eigen::PlainObjectBase<DerivedB> & B);

