// This file if from: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

#include <Eigen/Core>
#include <globals/macros.hpp>
#include <iostream>
#include <utils/eigen_conversations.hpp>
#include <utils/math.hpp>
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
    Eigen::Affine3d T = eigen_utils::pose2Affine(position, angles, 1);
    T.inverse();
    eigen_utils::scaleAffine(T, std::exp(zoom));
    return T;
  }

  // shift current view-plane in x and y
  void ProcessShift(double xoffset, double yoffset) {
    const Eigen::Vector3d move(xoffset * shift_sensitivity, -yoffset * shift_sensitivity, 0);
    moveXYZ(move);
  }

  // rotate current camera pose
  void ProcessRotation(double xoffset, double yoffset) {
    // TODO use translate()
    // rotate
    // pre_rotate()

    xoffset *= rotate_sensitivity;
    yoffset *= rotate_sensitivity;

    const Eigen::Vector3d rotation(0, -xoffset, yoffset);
    rotateRPY(rotation);
  }

  // processes input received from a mouse scroll-wheel event.
  void ProcessMouseScroll(double dscroll) {
    dscroll *= scroll_sensitivity;
    zoom += dscroll;
  }

 public:
  double far_clipping = 1.;
  double near_clipping = 0.9;
  double lense_angle_rad = math::deg2Rad(30.);  // something between 30 and 90 looks ok

 private:
  void moveXYZ(const Eigen::Vector3d &xyz) { position += xyz; }

  void rotateRPY(const Eigen::Vector3d &rpy) { angles += rpy; }

  // camera Attributes
  Eigen::Vector3d position = Eigen::Vector3d::Zero();  // X,Y,Z
  Eigen::Vector3d angles = Eigen::Vector3d(0, 0, 0);   // R,P,Y
  double zoom = 1;


  // camera options
  double scroll_sensitivity = 0.01;
  double shift_sensitivity = 0.001;
  double rotate_sensitivity = 0.001;
};
#endif
