#ifndef EIGEN_CONVERSATIONS
#define EIGEN_CONVERSATIONS

#include <Eigen/Geometry>

namespace eigen_utils {

using namespace Eigen;
inline Matrix3d rpy2RotationMatrix(const Vector3d &rpy) {

  Matrix3d m;
  m = AngleAxisd(rpy(2, 0), Vector3d::UnitZ()) *
      AngleAxisd(rpy(1, 0), Vector3d::UnitY()) *
      AngleAxisd(rpy(0, 0), Vector3d::UnitX());
  return m;
}

inline Matrix3f rpy2RotationMatrix(const Vector3f &rpy) {

  Matrix3f m;
  m = AngleAxisf(rpy(2, 0), Vector3f::UnitZ()) *
      AngleAxisf(rpy(1, 0), Vector3f::UnitY()) *
      AngleAxisf(rpy(0, 0), Vector3f::UnitX());
  return m;
}

inline Vector3d RotationMatrix2rpy(const Matrix3d &r) {
  return r.eulerAngles(2, 1, 0);
}

inline Vector3f RotationMatrix2rpy(const Matrix3f &r) {
  return r.eulerAngles(2, 1, 0);
}


inline Eigen::Affine3d rpy2Affine(const Vector3d &rpy) {
  Eigen::Affine3d rx =
      Eigen::Affine3d(Eigen::AngleAxisd(rpy(2, 0), Eigen::Vector3d(1, 0, 0)));
  Eigen::Affine3d ry =
      Eigen::Affine3d(Eigen::AngleAxisd(rpy(1, 0), Eigen::Vector3d(0, 1, 0)));
  Eigen::Affine3d rz =
      Eigen::Affine3d(Eigen::AngleAxisd(rpy(0, 0), Eigen::Vector3d(0, 0, 1)));
  return rz * ry * rx;
}

inline Eigen::Affine3f rpy2Affine(const Vector3f &rpy) {
  Eigen::Affine3f rx =
      Eigen::Affine3f(Eigen::AngleAxisf(rpy(2, 0), Eigen::Vector3f(1, 0, 0)));
  Eigen::Affine3f ry =
      Eigen::Affine3f(Eigen::AngleAxisf(rpy(1, 0), Eigen::Vector3f(0, 1, 0)));
  Eigen::Affine3f rz =
      Eigen::Affine3f(Eigen::AngleAxisf(rpy(0, 0), Eigen::Vector3f(0, 0, 1)));
  return rz * ry * rx;
}

inline Affine3d pose2Affine(const Vector3d &xyz, const Vector3d &rpy, double zoom) {
  const Eigen::Matrix3d r = rpy2RotationMatrix(rpy);
  Affine3d t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, zoom;
  return t;
}

inline Affine3f pose2Affine(const Vector3f &xyz, const Vector3f &rpy, float zoom) {
  const Eigen::Matrix3f r = rpy2RotationMatrix(rpy);
  Affine3f t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, zoom;
  return t;
}

inline void scaleAffine(Affine3d &a, double scale) {
  a.matrix().block<1, 4>(3, 0) << 0, 0, 0, scale;
}

inline void scaleAffine(Affine3f &a, float scale) {
  a.matrix().block<1, 4>(3, 0) << 0, 0, 0, scale;
}
}  // namespace eigen_utils

#endif
