#ifndef EIGEN_CONVERSATIONS
#define EIGEN_CONVERSATIONS

#include <Eigen/Geometry>

namespace eigen_utils {

using namespace Eigen;


inline Vector3d rotationMatrix2rpy(const Matrix3d &r) {
  return r.eulerAngles(2, 1, 0);
}

inline Vector4d vector2Affine(const Vector3d &v) {
  return Vector4d(v.x(), v.y(), v.z(), 1);
}

inline Vector4f vector2Affine(const Vector3f &v) {
  return Vector4f(v.x(), v.y(), v.z(), 1);
}

inline Vector3d affine2Vector(const Vector4d &v) {
  return Vector3d(v.x(), v.y(), v.z());
}

inline Vector3f affine2Vector(const Vector4f &v) {
  return Vector3f(v.x(), v.y(), v.z());
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

inline double quaternion2Yaw(const Quaterniond &q) {
  return std::atan2(2.0 * (q.z() * q.w() + q.x() * q.y()),
                    -1.0 + 2.0 * (q.w() * q.w() + q.x() * q.x()));
}

inline float quaternion2Yaw(const Quaternionf &q) {
  return std::atan2(2.f * (q.z() * q.w() + q.x() * q.y()),
                    -1.f + 2.f * (q.w() * q.w() + q.x() * q.x()));
}

inline double quaternion2Pitch(const Quaterniond &q) {
  return std::asin(2.0 * (q.y() * q.w() - q.z() * q.x()));
}

inline float quaternion2Pitch(const Quaternionf &q) {
  return std::asin(2.f * (q.y() * q.w() - q.z() * q.x()));
}

inline double quaternion2Roll(const Quaterniond &q) {
  return std::atan2(2.0 * (q.z() * q.y() + q.w() * q.x()),
                    1.0 - 2.0 * (q.x() * q.x() + q.y() * q.y()));
}

inline float quaternion2Roll(const Quaternionf &q) {
  return std::atan2(2.f * (q.z() * q.y() + q.w() * q.x()),
                    1.f - 2.f * (q.x() * q.x() + q.y() * q.y()));
}

inline Vector3d rotate(const Quaterniond &q, const Vector3d &abc) {
  Eigen::Quaterniond q_abc;
  q_abc.w() = 0;
  q_abc.vec() = abc;

  q_abc = q.inverse() * q_abc * q;
  return q_abc.vec();
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


inline Affine3d pose2Affine(const Vector3d &xyz, const Vector3d &rpy) {
  const Matrix3d r = rpy2RotationMatrix(rpy);
  Affine3d t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 1;
  return t;
}

inline Affine3f pose2Affine(const Vector3f &xyz, const Vector3f &rpy) {
  const Matrix3f r = rpy2RotationMatrix(rpy);
  Affine3f t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) = xyz;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 1;
  return t;
}

inline Affine3d quaternion2Affine(const Eigen::Quaterniond &q) {
  const Matrix3d r = q.toRotationMatrix();
  Affine3d t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) << 0, 0, 0;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 1;
  return t;
}

inline Affine3f quaternion2Affine(const Eigen::Quaternionf &q) {
  const Matrix3f r = q.toRotationMatrix();
  Affine3f t;
  t.matrix().block<3, 3>(0, 0) = r;
  t.matrix().block<3, 1>(0, 3) << 0, 0, 0;
  t.matrix().block<1, 4>(3, 0) << 0, 0, 0, 1;
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
