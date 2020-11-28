// This file if from: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <globals/macros.hpp>
#include <iostream>
#include <utils/eigen_conversations.hpp>
#include <vector>


class Camera {
 public:
  // constructor with vectors
  Camera() {}

  void setAngles(double roll, double pitch, double yaw) {
    angles << roll, pitch, yaw;
  }
  void setAngles(const Eigen::Vector3d &angles) { this->angles = angles; }

  void setPosition(double x, double y, double z) { position << x, y, z; }

  void setPosition(const Eigen::Vector3d &pos) { position = pos; }

  double getZoom() const { return -zoom; }

  // returns the view matrix calculated using Euler Angles and the Position
  Eigen::Affine3d GetViewMatrix() const {
    return eigen_utils::pose2Affine(position, angles).inverse();
  }

  // shift current view-plane in x and y
  void ProcessShift(double xoffset, double yoffset) {
    const Eigen::Vector3d move(-xoffset * shift_sensitivity, yoffset * shift_sensitivity, 0);
    moveXYZ(move);
  }

  // rotate current camera pose
  void ProcessRotation(double xoffset, double yoffset) {
    const Eigen::Vector3d rotation(
        rotate_sensitivity * yoffset, rotate_sensitivity * xoffset, 0);
    rotateRPY(rotation);
  }

  // processes input received from a mouse scroll-wheel event.
  void ProcessMouseScroll(double dscroll) {
    dscroll *= scroll_sensitivity;
    zoom += dscroll;
    // const Eigen::Vector3d move(0, 0, dscroll);
    // moveXYZ(move);
  }


 private:
  void moveXYZ(const Eigen::Vector3d &xyz) {
    const Eigen::Matrix3d r = eigen_utils::rpy2RotationMatrix(angles);
    const Eigen::Vector3d shift_vector = r * xyz;
    position += shift_vector;
  }

  void rotateRPY(const Eigen::Vector3d &rpy) {
    const Eigen::Matrix3d r = eigen_utils::rpy2RotationMatrix(angles);
    const Eigen::Vector3d rotate_vector = r * rpy;
    angles += rotate_vector;
  }

  // camera Attributes
  Eigen::Vector3d position = Eigen::Vector3d::Zero();  // X,Y,Z
  Eigen::Vector3d angles = Eigen::Vector3d::Zero();    // R,P,Y
  double zoom = 0;

  // camera options
  double scroll_sensitivity = 0.01;
  double shift_sensitivity = 0.001;
  double rotate_sensitivity = 0.001;
};
#endif
