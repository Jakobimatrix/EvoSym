#ifndef MIN_MAX
#define MIN_MAX


namespace utils {
template <class T>

#include <iostream>
#include <limits>
#include <utility>

class MinMax {
 public:
  T min;
  T max;

  /*!
   * \brief Default constructor will initiate the stored boundaries to the double limits max/min possible.
   */
  MinMax() {
    min = std::numeric_limits<T>::min();
    max = std::numeric_limits<T>::max();
  }

  /*!
   * \brief Constructor
   * \param min Value to be written as lower boundary.
   * \param max Value to be written as upper boundary
   */
  MinMax(T min, T max) { set(min, max); }

  /*!
   * \brief Constructor
   * \param minmax Copies the given MinMax into this class.
   */
  MinMax(const MinMax &minmax) {
    min = minmax.min;
    max = minmax.max;
  }

  /*!
   * \brief Copyconstructor
   * \param minmax The MinMax to be copied.
   * \return The copied MinMax class.
   */
  MinMax &operator=(const MinMax &minmax) {
    if (this != &minmax) {
      this->~MinMax();
      new (this) MinMax(minmax);
    }

    return *this;
  }

  /*!
   * \brief Calculates the difference betwiin the stored min and max value.
   * \return the difference between the stored min and max value.
   */
  T distance() const { return max - min; }

  /*!
   * \brief Calculates the center between the stored min and max value.
   * \return the center between min and max stored in this class.
   */
  T center() const { return min + distance() * 0.5; }

  /*!
   * \brief Checks if this class was initiated with values or with the default constructor.
   * \return True if at least one stored value is not the default value (double limit).
   */
  bool isInitiated() const {
    return (min != std::numeric_limits<T>::min() || max != std::numeric_limits<T>::max())
               ? true
               : false;
  }

  /*!
   * \brief Setter for the max value
   * \param max The value to be set as max if the given value is over this->min.
   * \return if the given value is over this->min value.
   */
  bool setMax(T max) {
    if (max >= min) {
      this->max = max;
      return true;
    }
    return false;
  }

  /*!
   * \brief Setter for the min value
   * \param min The value to be set as min if the given value is below
   * this->min.
   * \return if the given value is below this->max value.
   */
  bool setMin(double min) {
    if (min <= max) {
      this->min = min;
      return true;
    }
    return false;
  }

  /*!
   * \brief Sets both min and max values. If max < min both are swaped
   * \param max The value to be set as max
   * \param max The value to be set as min
   */
  void set(T min, T max) {
    if (min > max) {
      this->min = max;
      this->max = min;
    } else {
      this->max = max;
      this->min = min;
    }
  }

  /*!
   * \brief Calculated if the given value val is between the stored
   * boundaries.
   * \param val The value to be checked if between stored boundaries.
   * \return True if this->min > val > this->max.
   */
  template <class T2 = T>
  bool isBetween(T2 val) const {
    return (min <= val && val <= max) ? true : false;
  }

  /*!
   * \brief clamps the given value between the stored boundaries.
   * \param value The value to be clamped.
   */
  void clamp(T &value) const {
    value = boost::algorithm::clamp(value, min, max);
  }
};
}  // namespace utils



#endif
