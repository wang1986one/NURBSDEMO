
#include <Eigen/Core>

  /// Computes the centroid and enclosed volume of a closed mesh using a surface integral.
  /// 
  /// @param[in] V  #V by dim list of rest domain positions
  /// @param[in] F  #F by 3 list of triangle indices into V
  /// @param[out]   c  dim vector of centroid coordinates
  /// @param[out]   vol  total volume of solid.
  ///
  template <
    typename DerivedV, 
    typename DerivedF, 
    typename Derivedc, 
    typename Derivedvol>
   void centroid(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<Derivedc>& c,
    Derivedvol & vol);
  /// \overload
  template <
    typename DerivedV, 
    typename DerivedF, 
    typename Derivedc>
   void centroid(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<Derivedc>& c);


