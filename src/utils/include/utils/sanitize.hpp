#ifndef SANITIZE_HPP
#define SANITIZE_HPP

#include <eigen3/Eigen/Geometry>

#include "eigen_conversations.hpp"

namespace sane {

template <class T, int m>
inline void normalized3D(Eigen::Matrix<T, m, 1> &vector,
                         const Eigen::Matrix<T, m, 1> &solution) {
  if (eigen_utils::isApproxElementWize(vector, static_cast<T>(0.), 7)) {
    vector = solution;
  } else {
    vector.normalize();
  }
}

template <class T>
inline void normalizedQuad(Eigen::Quaternion<T> &q, const Eigen::Quaternion<T> &solution) {

  if (eigen_utils::isApproxElementWize(
          Eigen::Matrix<T, 3, 1>(q.vec()), static_cast<T>(0.), 7) &&
      std::abs(q.w()) > std::numeric_limits<T>::min()) {
    q = solution;
  } else {
    q.normalize();
  }
}
}  // namespace sane


#endif
