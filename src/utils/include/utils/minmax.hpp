#ifndef MIN_MAX
#define MIN_MAX

#include <Eigen/Geometry>
#include <algorithm>
#include <iostream>
#include <limits>
#include <utility>

#include "eigen_conversations.hpp"

namespace utils {

template <class T>
class MinMax {
 public:
  T min;
  T max;

  /*!
   * \brief Default constructor will initiate the stored boundaries
   * to the double limits max/min possible.
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
   * \return The copied of the MinMax class.
   */
  MinMax &operator=(const MinMax &minmax) {
    if (this != &minmax) {
      this->~MinMax();
      new (this) MinMax(minmax);
    }

    return *this;
  }

  /*!
   * \brief Calculates the difference between the stored min and max value.
   * \return The difference between the stored min and max value.
   */
  T distance() const { return max - min; }

  /*!
   * \brief Calculates the center between the stored min and max value.
   * \return The center.
   */
  T center() const { return min + distance() * 0.5; }

  /*!
   * \brief Checks if this class was initiated with values or with the default constructor.
   * \return True if at least one stored value is not the default value (limit).
   */
  bool isInitiated() const {
    return (min != std::numeric_limits<T>::min() || max != std::numeric_limits<T>::max())
               ? true
               : false;
  }

  /*!
   * \brief Setter for the max value
   * \param max The value to be set as max if the given value is greater than
   * this->min.
   * \return If the given value is greater than min value.
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
   * \return if the given value is below max value.
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
  void clamp(T &value) const { value = std::clamp(value, min, max); }
};

template <typename T>
class MinMax3d {
 public:
  std::array<MinMax<T>, 3> data;
  MinMax<T> &x = data[0];
  MinMax<T> &y = data[1];
  MinMax<T> &z = data[2];

  MinMax3d();

  MinMax3d(const Eigen::Matrix<T, 3, 1> &min, const Eigen::Matrix<T, 3, 1> &max) {
    set(min, max);
  }

  /*!
   * \brief Constructor
   * \param minmax Copies the given MinMax into this class.
   */
  MinMax3d(const MinMax3d &minmax3d) { data = minmax3d.data; }

  /*!
   * \brief Copyconstructor
   * \param minmax The MinMax3D to be copied.
   * \return The copied MinMax3D class.
   */
  MinMax3d &operator=(const MinMax3d &minMax3d) {
    if (this != &minMax3d) {
      this->~MinMax3D();
      new (this) MinMax(minMax3d);
    }

    return *this;
  }

  /*!
   * \brief Calculated if the given 3D Point is within this MinMax3d.
   * \param point The point to be checked if between stored boundaries.
   * \return True if the given point is inside the borders. False otherwise.
   */
  template <class T2 = T>
  bool isWithin(const Eigen::Matrix<T2, 3, 1> &point) const {
    return x.isBetween(point.x()) && y.isBetween(point.y()) && z.isBetween(point.z());
  }

  /*!
   * \brief Calculated if the given 3D Point is outside this MinMax3d.
   * \param point The point to be checked if outside stored boundaries.
   * \return True if the given point is outside the borders. False otherwise.
   */
  template <class T2 = T>
  bool isOutside(const Eigen::Matrix<T2, 3, 1> &point) const {
    return !isWithin(point);
  }

  /*!
   * \brief Checks if all dimensions in this MinMax3d are initialized.
   * \return True if none of the dimensins are equal to the std limits.
   */
  bool isInitiated() const {
    return x.isInitiated() && y.isInitiated() && z.isInitiated();
  }

  /*!
   * \brief Sets the boundaries given the two corner positions defining the boundaries.
   * \param min The minimal x,y,z values as a vector.
   * \param max The maximal x,y,z values as a vector.
   */
  void set(const Eigen::Matrix<T, 3, 1> &min, const Eigen::Matrix<T, 3, 1> &max) {
    x.set(min.x(), max.x());
    y.set(min.y(), max.y());
    z.set(min.z(), max.z());
  }

  /*!
   * \brief Returns the minimal values for all 3 dimensions in a vector.
   */
  Eigen::Matrix<T, 3, 1> min() const {
    return Eigen::Matrix<T, 3, 1>(x.min, y.min, z.min);
  }

  /*!
   * \brief Returns the maximal values for all 3 dimensions in a vector.
   */
  Eigen::Matrix<T, 3, 1> max() const {
    return Eigen::Matrix<T, 3, 1>(x.max, y.max, z.max);
  }

  /*!
   * \brief Returns the center point of this MinMax3d cube.
   */
  Eigen::Matrix<T, 3, 1> center() const {
    return Eigen::Matrix<T, 3, 1>(x.center(), y.center(), z.center());
  }

  /*!
   * \brief Given an vector of 3d points, set internal min and max values
   * for each dimension, such that every given point is inside this MinMax3d cube.
   * If the given vector is empty this MinMax3d will be uninitialized.
   * \param points A Vector of 3d Point which shall all be inside the boundaries.
   */
  void fit(const std::vector<Eigen::Matrix<T, 3, 1>, Eigen::aligned_allocator<Eigen::Matrix<T, 3, 1>>> &points) {
    Eigen::Matrix<T, 3, 1> min =
        Eigen::Matrix<T, 3, 1>(std::numeric_limits<T>::max(),
                               std::numeric_limits<T>::max(),
                               std::numeric_limits<T>::max());
    Eigen::Matrix<T, 3, 1> max =
        Eigen::Matrix<T, 3, 1>(std::numeric_limits<T>::min(),
                               std::numeric_limits<T>::min(),
                               std::numeric_limits<T>::min());

    for (const auto &p : points) {
      for (int i = 0; i < 3; i++) {
        min(i, 1) = std::min(min(i, 1), p(i, 1));
        max(i, 1) = std::max(max(i, 1), p(i, 1));
      }
    }
    set(min, max);
  }

  /*!
   * \brief Writes into a given vector all 8 Corner points of this MinMax3d
   * cube.
   * \param corners The vector to be written into.
   */
  void getCornerPositions(
      std::vector<Eigen::Matrix<T, 3, 1>, Eigen::aligned_allocator<Eigen::Matrix<T, 3, 1>>> corners) const {
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.min, y.min, z.min));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.max, y.min, z.min));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.min, y.max, z.min));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.max, y.max, z.min));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.min, y.min, z.max));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.max, y.min, z.max));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.min, y.max, z.max));
    corners.push_back(Eigen::Matrix<T, 3, 1>(x.max, y.max, z.max));
  }

  /*!
   * \brief Sets this MinMax3d Cube such that a given MinMax3d, which is rotated and translated,
   * fits the given MinMax3d cube inside.
   * \param isometry The isometry describeing the rotation and translation from the given cube rellative to this cube.
   * \param min_max_3d The cube which should fit inside this MinMax3d cube.
   */
  void fitOutside(const Eigen::Transform<T, 3, Eigen::Isometry> &isometry,
                  const MinMax3d min_max_3d) {
    if (!min_max_3d.isInitiated()) {
      return;
    }
    std::vector<Eigen::Matrix<T, 3, 1>, Eigen::aligned_allocator<Eigen::Matrix<T, 3, 1>>> corners(8);
    min_max_3d.getCornerPositions(corners);
    for (auto &p : corners) {
      p = isometry * p;
    }
    fit(corners);
  }
};
}  // namespace utils



#endif
