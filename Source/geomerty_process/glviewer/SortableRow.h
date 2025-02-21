#pragma once

// Simple class to contain a rowvector which allows rowwise sorting and
// reordering
#include <Eigen/Core>


  /// A row of things that can be sorted against other rows
  /// @tparam T  should be a vector/matrix/array that implements .size(), and operator(int i)
  template <typename T>
  class SortableRow
  {
    public:
      /// The data
      T data;
    public:
      /// Default constructor
      SortableRow():data(){};
      /// Constructor
      /// @param[in] data  the data
      SortableRow(const T & data):data(data){};
      /// Less than comparison
      /// @param[in] that  the other row
      /// @returns true if this row is less than that row
      bool operator<(const SortableRow & that) const
      {
        // Lexicographical
        int minc = (this->data.size() < that.data.size()?
            this->data.size() : that.data.size());
        // loop over columns
        for(int i = 0;i<minc;i++)
        {
          if(this->data(i) == that.data(i))
          {
            continue;
          }
          return this->data(i) < that.data(i);
        }
        // All characters the same, comes done to length
        return this->data.size()<that.data.size();
      };
      /// Equality comparison
      /// @param[in] that  the other row
      /// @returns true if this row is equal to that row
      bool operator==(const SortableRow & that) const
      {
        if(this->data.size() != that.data.size())
        {
          return false;
        }
        for(int i = 0;i<this->data.size();i++)
        {
          if(this->data(i) != that.data(i))
          {
            return false;
          }
        }
        return true;
      };
      /// Inequality comparison
      /// @param[in] that  the other row
      /// @returns true if this row is not equal to that row
      bool operator!=(const SortableRow & that) const
      {
        return !(*this == that);
      };
  };
