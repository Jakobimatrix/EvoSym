#ifndef EIGEN_CONVERSATIONS
#define EIGEN_CONVERSATIONS

#include <Eigen/Geometry>

namespace eigen_utils {

using namespace Eigen;


inline Vector3d rotationMatrix2rpy(const Matrix3d &r) {
  return r.eulerAngles(2, 1, 0);
}

inline Vector3f rotationMatrix2rpy(const Matrix3f &r) {
  return r.eulerAngles(2, 1, 0);
}

inline Vector3d quaternion2rpy(const Quaterniond &q) {
  return q.toRotationMatrix().eulerAngles(2, 1, 0);
}

inline Vector3f quaternion2rpy(const Quaternionf &q) {
  return q.toRotationMatrix().eulerAngles(2, 1, 0);
}

inline Quaterniond getZeroRotation(const Vector3d &axis) {
  Quaterniond q;
  q.vec() = axis;
  q.w() = 0;
  q.normalize();
  return q;
}

inline Quaterniond rpy2Quaternion(const Vector3d &rpy) {
  Quaterniond q;
  q = AngleAxisd(rpy(2, 0), Vector3d::UnitZ()) *
      AngleAxisd(rpy(1, 0), Vector3d::UnitY()) *
      AngleAxisd(rpy(0, 0), Vector3d::UnitX());
  return q;
}

inline Eigen::Quaternionf rpy2Quaternion(const Vector3f &rpy) {
  Quaternionf q;
  q = AngleAxisf(rpy(2, 0), Vector3f::UnitZ()) *
      AngleAxisf(rpy(1, 0), Vector3f::UnitY()) *
      AngleAxisf(rpy(0, 0), Vector3f::UnitX());
  return q;
}

inline Matrix3d rpy2RotationMatrix(const Vector3d &rpy) {
  return rpy2Quaternion(rpy).toRotationMatrix();
}

inline Matrix3f rpy2RotationMatrix(const Vector3f &rpy) {
  return rpy2Quaternion(rpy).toRotationMatrix();
}


inline Affine3d pose2Affine(const Vector3d &xyz, const Vector3d &rpy, double zoom = 1) {
  const Matrix3d r = rpy2RotationMatrix(rpy);
  Affine3d t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, zoom;
  return t;
}

inline Affine3f pose2Affine(const Vector3f &xyz, const Vector3f &rpy, float zoom = 1) {
  const Matrix3f r = rpy2RotationMatrix(rpy);
  Affine3f t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, zoom;
  return t;
}

inline Affine3d quaternion2Affine(const Eigen::Quaterniond &q) {
  const Matrix3d r = q.toRotationMatrix();
  Affine3d t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) << 0, 0, 0;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 0;
  return t;
}

inline Affine3f quaternion2Affine(const Eigen::Quaternionf &q) {
  const Matrix3f r = q.toRotationMatrix();
  Affine3f t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) << 0, 0, 0;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 0;
  return t;
}

inline Affine3d rpy2Affine(const Vector3d &rpy) {
  return quaternion2Affine(rpy2Quaternion(rpy));
}

inline Affine3f rpy2Affine(const Vector3f &rpy) {
  return quaternion2Affine(rpy2Quaternion(rpy));
}

inline void scaleAffine(Affine3d &a, double scale) {
  a.matrix().block<1, 4>(3, 0) << 0, 0, 0, scale;
}

inline void scaleAffine(Affine3f &a, float scale) {
  a.matrix().block<1, 4>(3, 0) << 0, 0, 0, scale;
}
}  // namespace eigen_utils

#endif
