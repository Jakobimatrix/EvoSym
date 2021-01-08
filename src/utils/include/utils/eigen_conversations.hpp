#ifndef EIGEN_CONVERSATIONS
#define EIGEN_CONVERSATIONS

#include <assert.h>

#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <algorithm>
#include <map>

#include "math.hpp"

namespace eigen_utils {

using namespace Eigen;

typedef std::vector<Vector3f, aligned_allocator<Vector3f>> EigenStlVector3f;
typedef std::map<int, EigenStlVector3f, std::less<int>, Eigen::aligned_allocator<std::pair<const int, EigenStlVector3f>>> EigenStlMapedVector3f;


// access Transform and Matrix like
// Matrix4d M; M.matrix().block<num_rows, num_cols>(row, col) = ...;
// Its not defined for the basic classes.
// usage:
// Transform<T, 3, Isometry> t; getSubmatrix<num_rows, num_cols, row, col>(t.matrix()) = ...;
template <int m, int n, int x, int y, typename Derived>
Eigen::Block<Derived, m, n> getSubmatrix(MatrixBase<Derived> &data) {
  return Eigen::Block<Derived, m, n>(data.derived(), x, y);
}

template <typename Derived>
void inverteAffine3d(Eigen::MatrixBase<Derived> &m) {
  getSubmatrix<3, 3, 0, 0>(m).transposeInPlace();
  getSubmatrix<3, 1, 0, 3>(m) =
      -getSubmatrix<3, 3, 0, 0>(m) * getSubmatrix<3, 1, 0, 3>(m);
}

template <typename T, int m, int n>
void clampElements(Matrix<T, m, n> &matrix, T min, T max) {
  for (size_t nn = 0; nn < n; nn++) {
    for (size_t mm = 0; mm < m; mm++) {
      std::clamp(matrix(mm, nn), min, max);
    }
  }
}

template <typename T, int m, int n>
constexpr Matrix<T, m, n> Zero() {
  Matrix<T, m, n> ret;
  for (size_t nn = 0; nn < n; nn++) {
    for (size_t mm = 0; mm < m; mm++) {
      ret(mm, nn) = static_cast<T>(0);
    }
  }
  return ret;
}

/*!
 * \brief isApproxElementWize compares two equal sized matrices element wize
 * An alternative would be the Eigen::is_approx() which uses the Frobenius norm.
 * \param a Matrix 1 to compare
 * \param b Matrix 2 to compare
 * \param ulp (units in the last place of precision)
 * \return True if the two matrices are approximate equal
 */
template <typename T, int m, int n>
inline bool isApproxElementWize(const Eigen::Matrix<T, m, n> &a,
                                const Eigen::Matrix<T, m, n> &b,
                                int ulp) {
  for (size_t c = 0; c < m; c++) {
    for (size_t r = 0; r < n; r++) {
      if (!math::almost_equal(a(c, r), b(c, r), ulp)) {
        return false;
      }
    }
  }
  return true;
}

/*!
 * \brief isApproxElementWize Checks if matrix has the same given velue in every position.
 * \param a Matrix to compare
 * \param b one value
 * \param ulp (units in the last place of precision)
 * \return True if the two matrices are approximate equal
 */
template <typename T, int m, int n>
inline bool isApproxElementWize(const Eigen::Matrix<T, m, n> &a, T b, int ulp) {
  for (size_t c = 0; c < m; c++) {
    for (size_t r = 0; r < n; r++) {
      if (!math::almost_equal(a(c, r), b, ulp)) {
        return false;
      }
    }
  }
  return true;
}

/*!
 * \brief isApproxElementWize compares two equal sized matrices element wize
 * An alternative would be the Eigen::is_approx() which uses the Frobenius norm.
 * \param a Matrix 1 to compare
 * \param b Matrix 2 to compare
 * \param precision The diff elementwize must be smaller than that
 * \return True if the two matrices are approximate equal
 */
template <typename T, int m, int n>
inline bool isApproxElementWize(const Eigen::Matrix<T, m, n> &a,
                                const Eigen::Matrix<T, m, n> &b,
                                double precision) {
  for (size_t c = 0; c < m; c++) {
    for (size_t r = 0; r < n; r++) {
      if (std::abs(a(c, r) - b(c, r)) > precision) {
        return false;
      }
    }
  }
  return true;
}

/*!
 * \brief isApproxElementWize Checks if matrix has the same given velue in every
 * position.
 * \param a Matrix to compare
 * \param b one value
 * \param ulp (units in the last place of precision)
 * \param precision The diff elementwize must be
 * smaller than that
 * \return True if the two matrices are approximate equal
 */
template <typename T, int m, int n>
inline bool isApproxElementWize(const Eigen::Matrix<T, m, n> &a, T b, double precision) {
  for (size_t c = 0; c < m; c++) {
    for (size_t r = 0; r < n; r++) {
      if (std::abs(a(c, r) - b) > precision) {
        return false;
      }
    }
  }
  return true;
}

/*!
 * \brief Calculates the Normal of an triangle assuming a, b, c are given anticlockwize.
 * \param a The first point of the triangle.
 * \param b The second point of the triangle.
 * \param c The third point of the triangle.
 * \return the normal
 */
template <typename T>
inline Matrix<T, 3, 1> getTrianglesNormal(const Matrix<T, 3, 1> &a,
                                          const Matrix<T, 3, 1> &b,
                                          const Matrix<T, 3, 1> &c) {
  const Eigen::Vector3f edge1 = b - a;
  const Eigen::Vector3f edge2 = c - a;
  Matrix<T, 3, 1> normal = edge1.cross(edge2);
  normal.normalize();
  return normal;
}

template <typename T>
inline Matrix<T, 3, 1> rotationMatrix2ypr(const Matrix<T, 3, 3> &r) {
  return r.eulerAngles(2, 1, 0);
}

template <typename T>
inline Matrix<T, 3, 1> rotationMatrix2rpy(const Matrix<T, 3, 3> &r) {
  const Matrix<T, 3, 1> ypr = rotationMatrix2ypr(r);
  return Matrix<T, 3, 1>(ypr.z(), ypr.y(), ypr.x());
}

template <typename T>
inline Matrix<T, 4, 1> vector2Affine(const Matrix<T, 3, 1> &v) {
  return Matrix<T, 4, 1>(v.x(), v.y(), v.z(), 1);
}

template <typename T>
inline Matrix<T, 3, 1> affine2Vector(const Matrix<T, 4, 1> &v) {
  return Matrix<T, 3, 1>(v.x(), v.y(), v.z());
}

template <typename T>
inline Matrix<T, 3, 1> quaternion2rpy(const Quaternion<T> &q) {
  return rotationMatrix2rpy(q.toRotationMatrix());
}

template <typename T>
inline T quaternion2Yaw(const Quaternion<T> &q) {
  return std::atan2(
      static_cast<T>(2.0) * (q.z() * q.w() + q.x() * q.y()),
      -static_cast<T>(1.0) + static_cast<T>(2.0) * (q.w() * q.w() + q.x() * q.x()));
}

template <typename T>
inline T quaternion2Pitch(const Quaternion<T> &q) {
  return std::asin(static_cast<T>(2.0) * (q.y() * q.w() - q.z() * q.x()));
}


template <typename T>
inline T quaternion2Roll(const Quaternion<T> &q) {
  return std::atan2(
      static_cast<T>(2.0) * (q.z() * q.y() + q.w() * q.x()),
      static_cast<T>(1.0) - static_cast<T>(2.0) * (q.x() * q.x() + q.y() * q.y()));
}


template <typename T>
inline Matrix<T, 3, 1> rotate(const Quaternion<T> &q, const Matrix<T, 3, 1> &xyz) {
  Quaternion<T> q_xyz;
  q_xyz.w() = static_cast<T>(0);
  q_xyz.vec() = xyz;

  q_xyz = q.inverse() * q_xyz * q;
  return q_xyz.vec();
}

template <typename T>
inline Quaternion<T> getZeroRotation() {
  Quaternion<T> q;
  const T zero = static_cast<T>(0);
  q.vec() << zero, zero, zero;
  q.w() = static_cast<T>(1);
  return q;
}

template <typename T>
inline Quaternion<T> rpy2Quaternion(const Matrix<T, 3, 1> &rpy) {
  Quaternion<T> q;
  q = AngleAxis<T>(rpy(2, 0), Matrix<T, 3, 1>::UnitZ()) *
      AngleAxis<T>(rpy(1, 0), Matrix<T, 3, 1>::UnitY()) *
      AngleAxis<T>(rpy(0, 0), Matrix<T, 3, 1>::UnitX());
  return q;
}

template <typename T>
inline Quaternion<T> rotationMatrix2Quaternion(const Matrix<T, 3, 3> &r) {
  return rpy2Quaternion(rotationMatrix2ypr(r));
}

template <typename T>
inline Eigen::Matrix<T, 3, 3> rpy2RotationMatrix(const Eigen::Matrix<T, 3, 1> &rpy) {
  return rpy2Quaternion(rpy).toRotationMatrix().matrix();
}

template <typename T>
inline Transform<T, 3, Isometry> pose2Isometry(const Matrix<T, 3, 1> &xyz,
                                               const Matrix<T, 3, 3> &r) {
  Transform<T, 3, Isometry> t;
  getSubmatrix<3, 3, 0, 0>(t.matrix()) = r;
  getSubmatrix<3, 1, 0, 3>(t.matrix()) = xyz;
  getSubmatrix<1, 4, 3, 0>(t.matrix()) << 0, 0, 0, 1;
  return t;
}

template <typename T>
inline Transform<T, 3, Isometry> translation2Isometry(const Matrix<T, 3, 1> &xyz) {
  const Eigen::Matrix<T, 3, 3> r = Eigen::Matrix<T, 3, 3>::Identity();
  return pose2Isometry(xyz, r);
}

template <typename T>
inline Transform<T, 3, Isometry> rotationMatrix2Isometry(const Matrix<T, 3, 3> &r) {
  const Eigen::Matrix<T, 3, 1> xyz = Eigen::Matrix<T, 3, 1>::Zero();
  return pose2Isometry(xyz, r);
}

template <typename T>
inline Transform<T, 3, Isometry> pose2Isometry(const Matrix<T, 3, 1> &xyz,
                                               const Matrix<T, 3, 1> &rpy) {
  const Matrix<T, 3, 3> r = rpy2RotationMatrix(rpy);
  return pose2Isometry(xyz, r);
}

template <typename T>
inline Transform<T, 3, Isometry> pose2Isometry(const Matrix<T, 3, 1> &xyz,
                                               const Eigen::Quaternion<T> &q) {
  const Matrix<T, 3, 3> r = q.toRotationMatrix();
  const T zero = static_cast<T>(0);
  return pose2Isometry(xyz, r);
}

template <typename T>
inline Transform<T, 3, Isometry> quaternion2Isometry(const Eigen::Quaternion<T> &q) {
  const Matrix<T, 3, 3> r = q.toRotationMatrix();
  const T zero = static_cast<T>(0);
  const Matrix<T, 3, 1> xyz(zero, zero, zero);
  return pose2Isometry(xyz, r);
}

template <typename T>
inline Transform<T, 3, Isometry> rpy2Isometry(const Matrix<T, 3, 1> &rpy) {
  return quaternion2Isometry(rpy2Quaternion(rpy));
}


template <typename T>
inline void updateOrthogonalProjection(
    Transform<T, 3, Projective> &p, T left, T right, T bottom, T top, T near_clipping, T far_clipping) {
  p(0, 0) = (static_cast<T>(2) * near_clipping) / (right - left);
  p(1, 1) = (static_cast<T>(2) * far_clipping) / (top - bottom);
  p(2, 0) = (right + left) / (right - left);
  p(2, 1) = (top + bottom) / (top - bottom);
  p(2, 2) = far_clipping / (far_clipping - near_clipping);
  p(3, 2) = -(far_clipping * near_clipping) / (far_clipping - near_clipping);
}

template <typename T>
inline Transform<T, 3, Projective> getOrthogonalProjection(
    T left, T right, T bottom, T top, T near_clipping, T far_clipping) {

  Transform<T, 3, Projective> p;
  p.matrix() = Zero<T, 4, 4>();
  p(2, 3) = static_cast<T>(1.0);
  updateOrthogonalProjection(p, left, right, bottom, top, near_clipping, far_clipping);
  return p;
}

#include <iostream>
template <typename T>
inline Transform<T, 3, Isometry> getTransformation(const Matrix<T, 3, 1> &translation,
                                                   const Matrix<T, 3, 1> &view_direction) {

  Matrix<T, 3, 1> view_direction_normalized = view_direction;
  view_direction_normalized.normalize();

  const bool y_is_up = !math::almost_equal(
      std::abs(view_direction_normalized.y()), static_cast<T>(1), 2);

  // y-axis is up
  Matrix<T, 3, 1> up(0, 1, 0);
  if (!y_is_up) {
    // Z is up as a simple fix since a.cross(a) = 0,0,0
    up << 0, 0, 1;
  }

  // z-axis is view direction
  const Matrix<T, 3, 1> z = view_direction_normalized;
  // x-axis is orthogonal to z and y.
  Matrix<T, 3, 1> x = up.colwise().cross(z);
  x.normalize();

  // y-axis actually does not necessary look up directly but is probably tilted.
  Matrix<T, 3, 1> y = z.colwise().cross(x);
  y.normalize();

  Matrix<T, 3, 3> r;
  getSubmatrix<3, 1, 0, 0>(r.matrix()) = x;
  getSubmatrix<3, 1, 0, 1>(r.matrix()) = y;
  getSubmatrix<3, 1, 0, 2>(r.matrix()) = z;

  const Transform<T, 3, Isometry> R = rotationMatrix2Isometry(r);

  const Transform<T, 3, Isometry> TT = translation2Isometry(translation);

  return TT * R;
}

template <typename T>
inline void updatePerspectiveProjection(Transform<T, 3, Projective> &projection,
                                        T lense_angle_rad,
                                        T near_clipping,
                                        T far_clipping,
                                        T aspect_ratio) {
  // https://wiki.delphigl.com/index.php/glFrustum
  // crunch every visible vertex into [-1,1]^3 unit qube
  assert(abs(aspect_ratio - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

  const double tanHalfFovy = std::tan(lense_angle_rad / 2.0);

  projection(0, 0) = 1.0 / (aspect_ratio * tanHalfFovy);
  projection(1, 1) = 1.0 / tanHalfFovy;
  projection(2, 2) = (far_clipping + near_clipping) / (near_clipping - far_clipping);
  projection(2, 3) = (2 * far_clipping * near_clipping) / (near_clipping - far_clipping);
}

template <typename T>
inline Transform<T, 3, Projective> getPerspectiveProjection(T lense_angle_rad,
                                                            T near_clipping,
                                                            T far_clipping,
                                                            T aspect_ratio) {
  Transform<T, 3, Projective> projection;
  projection.matrix() = Zero<T, 4, 4>();
  updatePerspectiveProjection(
      projection, lense_angle_rad, near_clipping, far_clipping, aspect_ratio);
  projection(3, 2) = -1;
  return projection;
}

}  // namespace eigen_utils

#endif
