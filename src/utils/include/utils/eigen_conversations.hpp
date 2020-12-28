#ifndef EIGEN_CONVERSATIONS
#define EIGEN_CONVERSATIONS

#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <map>

namespace eigen_utils {

using namespace Eigen;

typedef std::vector<Vector3f, aligned_allocator<Vector3f>> EigenStlVector3f;
typedef std::map<int, EigenStlVector3f, std::less<int>, Eigen::aligned_allocator<std::pair<const int, EigenStlVector3f>>> EigenStlMapedVector3f;


// access Transform and Matrix like
// Matrix4d M; M.matrix().block<num_rows, num_cols>(row, col) = ...;
// Its not defined for the basic classes.
// usage:
// Transform<T, 3, Isometry> t; getSubmatrix<num_rows, num_cols, row, col>(t.matrix()) = ...;
template <size_t m, size_t n, size_t x, size_t y, typename Derived>
Eigen::Block<Derived, m, n> getSubmatrix(MatrixBase<Derived> &data) {
  return Eigen::Block<Derived, m, n>(data.derived(), x, y);
}

template <typename T, size_t m, size_t n>
constexpr Matrix<T, m, n> Zero() {
  Matrix<T, m, n> ret;
  for (size_t nn = 0; nn < n; nn++) {
    for (size_t mm = 0; mm < m; mm++) {
      ret(mm, nn) = static_cast<T>(0);
    }
  }
  return ret;
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

  Transform<T, 3, Projective> p = Transform<T, 3, Projective>::Zero();
  p(2, 3) = static_cast<T>(1.0);
  updateOrthogonalProjection(p, left, right, bottom, top, near_clipping, far_clipping);
  return p;
}

template <typename T>
inline Transform<T, 3, Isometry> getTransformation(const Matrix<T, 3, 1> &translation,
                                                   const Matrix<T, 3, 1> &view_direction) {
  // TODO IF view_direction == up

  // y-axis is up
  const Matrix<T, 3, 1> up(0, 1, 0);
  // z-axis is view direction
  Matrix<T, 3, 1> z = view_direction;
  z.normalize();
  // x-axis is orthogonal to z and y
  Matrix<T, 3, 1> x = z.colwise().cross(up);
  x.normalize();
  // y-axis actually does not necessary look up directly but is probably tilted
  const Matrix<T, 3, 1> y = x.colwise().cross(z);

  // Affine first rotates, than translates but within the rotated system.
  // So we need to figure out the new transformation in the rotated system.
  // We calculate how much of the old translation belongs to the new axis x,y,z
  // using dot product.

  const T tx = (x.cwiseProduct(translation)).sum();
  const T ty = (y.cwiseProduct(translation)).sum();
  const T tz = (z.cwiseProduct(translation)).sum();


  /*
  AXIS     TRANSFORM
  x  y  z  transl.
  1, 0, 0, tx
  0, 1, 0, ty,
  0, 0, 1, tz
  0, 0, 0, 1
  AXIS must be normalized and orthogonal
  */

  Transform<T, 3, Isometry> t;

  getSubmatrix<3, 1, 0, 0>(t.matrix()) = x;
  getSubmatrix<3, 1, 0, 1>(t.matrix()) = y;
  getSubmatrix<3, 1, 0, 2>(t.matrix()) = -z;
  getSubmatrix<3, 1, 0, 3>(t.matrix()) << -tx, -ty, tz;
  getSubmatrix<1, 4, 3, 0>(t.matrix()) << 0, 0, 0, 1;

  return t;
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

template <typename T>
inline Matrix<T, 3, 1> rotationMatrix2rpy(const Matrix<T, 3, 3> &r) {
  return r.eulerAngles(2, 1, 0);
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
  return q.toRotationMatrix().eulerAngles(2, 1, 0);
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
inline Matrix<T, 3, 1> rotate(const Quaternion<T> &q, const Matrix<T, 3, 1> &abc) {
  Quaternion<T> q_abc;
  q_abc.w() = static_cast<T>(0);
  q_abc.vec() = abc;

  q_abc = q.inverse() * q_abc * q;
  return q_abc.vec();
}

template <typename T>
inline Quaternion<T> getZeroRotation(const Matrix<T, 3, 1> &axis) {
  Quaternion<T> q;
  q.vec() = axis;
  q.w() = static_cast<T>(0);
  q.normalize();
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
inline Transform<T, 3, Isometry> pose2Isometry(const Matrix<T, 3, 1> &xyz,
                                               const Matrix<T, 3, 1> &rpy) {
  const Matrix<T, 3, 3> r = rpy2RotationMatrix(rpy);
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
inline Transform<T, 3, Isometry> rpy2Affine(const Vector3d &rpy) {
  return quaternion2Isometry(rpy2Quaternion(rpy));
}


}  // namespace eigen_utils

#endif
