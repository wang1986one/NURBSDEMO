#pragma once
#include <Eigen/Dense>


  // Common colormap types. 
  enum ColorMapType
  {
    COLOR_MAP_TYPE_INFERNO = 0,
    COLOR_MAP_TYPE_JET = 1,
    COLOR_MAP_TYPE_MAGMA = 2,
    COLOR_MAP_TYPE_PARULA = 3,
    COLOR_MAP_TYPE_PLASMA = 4,
    COLOR_MAP_TYPE_VIRIDIS = 5,
    COLOR_MAP_TYPE_TURBO = 6,
    NUM_COLOR_MAP_TYPES = 7
  };
  /// Compute [r,g,b] values of the selected colormap for
  /// a given factor f between 0 and 1
  ///
  /// @param[in] c  colormap enum
  /// @param[in] f  factor determining color value as if 0 was min and 1 was max
  /// @param[out] rgb  red, green, blue value
  template <typename T>
   void colormap(const ColorMapType cm, const T f, T * rgb);
  /// Compute [r,g,b] values of the selected colormap for
  /// a given factor f between 0 and 1
  ///
  /// @param[in] c  colormap enum
  /// @param[in] f  factor determining color value as if 0 was min and 1 was max
  /// @param[out] r  red value
  /// @param[out] g  green value
  /// @param[out] b  blue value
  template <typename T>
   void colormap(const ColorMapType cm, const T f, T & r, T & g, T & b);
  /// Compute [r,g,b] values of the colormap palette for
  /// a given factor f between 0 and 1
  ///
  /// @param[in] palette  256 by 3 array of color values
  /// @param[in] x_in  factor determining color value as if 0 was min and 1 was max
  /// @param[out] r  red value
  /// @param[out] g  green value
  /// @param[out] b  blue value
  template <typename T>
   void colormap(
    const double palette[256][3], const T x_in, T & r, T & g, T & b);
  /// Compute [r,g,b] values of the colormap palette for
  /// a given factors between 0 and 1
  ///
  ///  @param[in] cm selected colormap palette to interpolate from
  ///  @param[in] Z  #Z list of factors
  ///  @param[in] normalize  whether to normalize Z to be tightly between [0,1]
  ///  @param[out] C  #C by 3 list of rgb colors
  template <typename DerivedZ, typename DerivedC>
   void colormap(
    const ColorMapType cm,
    const Eigen::MatrixBase<DerivedZ> & Z,
    const bool normalize,
    Eigen::PlainObjectBase<DerivedC> & C);
  /// Compute [r,g,b] values of the colormap palette for
  /// a given factors between `min_Z` and `max_Z`
  ///
  ///  @param[in] cm selected colormap palette to interpolate from
  ///  @param[in] Z  #Z list of factors
  ///  @param[in] min_z  value at "0"
  ///  @param[in] max_z  value at "1"
  ///  @param[out] C  #C by 3 list of rgb colors
  template <typename DerivedZ, typename DerivedC>
   void colormap(
    const ColorMapType cm,
    const Eigen::MatrixBase<DerivedZ> & Z,
    const double min_Z,
    const double max_Z,
    Eigen::PlainObjectBase<DerivedC> & C);


