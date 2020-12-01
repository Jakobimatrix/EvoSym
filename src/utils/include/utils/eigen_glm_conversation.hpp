#ifndef EIGEN_GLM_CONVERSATIONS
#define EIGEN_GLM_CONVERSATIONS

#include <Eigen/Geometry>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {

/*!
 * \brief Converts a Affine3 representation into a 4x4 glm matrix
 * \param affine The Affine representation
 * \return 4x4 glm matrix copied data from affine.
 */
inline glm::mat4 EigenAffine2GlmMat(const Eigen::Affine3d &affine) {
  return glm::make_mat4<double>(affine.data());
}

/*!
 * \brief Converts a Affine3 representation into a 4x4 glm matrix
 * \param affine The Affine representation
 * \return 4x4 glm matrix copied data from affine.
 */
inline glm::mat4 EigenAffine2GlmMat(const Eigen::Affine3f &affine) {
  return glm::make_mat4<float>(affine.data());
}

/*!
 * \brief Converts a 4x4 matrix given in glm to eigen affine 3d
 * \param mat The input matrix. Although it is not defined constant, I am not changeing the values. Its just necessary to access pointer.
 * \return Eigen::Affine3d copied data from mat.
 */
inline Eigen::Affine3d glmMat2EigenAffine(glm::mat4 &mat) {
  float *array_ptr = reinterpret_cast<float *>(glm::value_ptr(mat));

  Eigen::Affine3f affine;
  affine.matrix() = Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor> >(array_ptr);
  return affine.cast<double>();
}

}  // namespace utils

#endif
