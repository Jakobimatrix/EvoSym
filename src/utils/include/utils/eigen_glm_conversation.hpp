#ifndef EIGEN_GLM_CONVERSATIONS
#define EIGEN_GLM_CONVERSATIONS

#include <Eigen/Geometry>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {

/*!
 * \brief Converts a Eigen 4x4 representation into a 4x4 glm matrix
 * \param m The Eigen matrix representation
 * \return 4x4 glm matrix copied data from eigen.
 */
template <typename T>
inline glm::mat4 EigenMat42GlmMat4(const Eigen::Matrix<T, 4, 4> &m) {
  return glm::make_mat4<T>(m.data());
}

/*!
 * \brief Converts a Eigen Vector3 representation into a glm vec3
 * \param glm vec The Eigen vector
 * \return glm vec3 copied data from affine.
 */
template <typename T>
inline glm::vec3 EigenVec32GlmVec3(const Eigen::Matrix<T, 3, 1> &vec) {
  return glm::make_vec3<T>(vec.data());
}

/*!
 * \brief Converts a 4x4 matrix given in glm to eigen 4x4 matrix
 * \param mat The input matrix. Although it is not defined constant, I am not changeing the values. Its just necessary to access pointer.
 * \return Eigen::Matrix4 copied data from mat.
 */
template <typename T>
inline Eigen::Matrix<T, 4, 4> glmMat42EigenMat4(glm::mat4 &mat) {
  float *array_ptr = reinterpret_cast<T *>(glm::value_ptr(mat));
  return Eigen::Map<Eigen::Matrix<T, 4, 4, Eigen::RowMajor> >(array_ptr);
}

}  // namespace utils

#endif
