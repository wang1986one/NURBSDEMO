#pragma once
#define IGL_TICTOC_LAMBDA \
      const auto & tictoc = []() \
      { \
        static double t_start = igl::get_seconds(); \
        double diff = igl::get_seconds()-t_start; \
        t_start += diff; \
        return diff; \
      };


  /// Current time in seconds 
  ///
  /// @return the current time in seconds since epoch
  /// 
  /// #### Example:
  /// \code{cpp}
  ///    const auto & tictoc = []()
  ///    {
  ///      static double t_start = igl::get_seconds();
  ///      double diff = igl::get_seconds()-t_start;
  ///      t_start += diff;
  ///      return diff;
  ///    };
  ///    tictoc();
  ///    ... // part 1
  ///    cout<<"part 1: "<<tictoc()<<endl;
  ///    ... // part 2
  ///    cout<<"part 2: "<<tictoc()<<endl;
  ///    ... // etc
  /// \endcode
 double get_seconds();


