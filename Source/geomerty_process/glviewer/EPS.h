#pragma once
  /// Standard value for double epsilon
  const double DOUBLE_EPS    = 1.0e-14;
  /// Standard value for double epsilon²
  const double DOUBLE_EPS_SQ = 1.0e-28;
  /// Standard value for single epsilon
  const float FLOAT_EPS    = 1.0e-7f;
  /// Standard value for single epsilon²
  const float FLOAT_EPS_SQ = 1.0e-14f;
  /// Function returning EPS for corresponding type
  template <typename S_type>  S_type EPS();
  /// Function returning EPS_SQ for corresponding type
  template <typename S_type>  S_type EPS_SQ();
  // Template specializations for float and double
  template <>  float EPS<float>();
  template <>  double EPS<double>();
  template <>  float EPS_SQ<float>();
  template <>  double EPS_SQ<double>();
