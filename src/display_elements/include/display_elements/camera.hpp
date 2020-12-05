// This file if from: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

#include <Eigen/Core>
#include <functional>
#include <globals/macros.hpp>
#include <iostream>
#include <utils/eigen_conversations.hpp>
#include <utils/math.hpp>
#include <vector>


class Camera {
 public:
  typedef std::function<void()> CallbackCameraChange;

  // constructor with vectors
  Camera() {
    updateProjection();
    updateView();
  }

  void addCallbackViewChange(CallbackCameraChange &func) {
    callbackViewChange = func;
  }

  void addCallbackPerspectiveChange(CallbackCameraChange &func) {
    callbackProjectionChange = func;
  }

  void setAngles(double roll, double pitch, double yaw) {
    setAngles(Eigen::Vector3d(roll, pitch, yaw));
  }

  void setAngles(const Eigen::Vector3d &angles) {
    this->angles = eigen_utils::rpy2Quaternion(angles);
    updateView();
  }

  void setPosition(double x, double y, double z) {
    setPosition(Eigen::Vector3d(x, y, z));
  }

  void setPosition(const Eigen::Vector3d &pos) {
    position = pos;
    updateView();
  }

  void setLenseAngleRad(double angle) {
    lense_angle_rad = angle;
    updateProjection();
  }

  void setClippingDistance(double near, double far) {
    near_clipping = near;
    far_clipping = far;
    updateProjection();
  }

  void setAspectRatio(double ratio) {
    aspect_ratio = ratio;
    updateProjection();
  }

  // returns the view matrix calculated using Euler Angles and the Position
  Eigen::Affine3d getViewMatrix() const { return view; }

  Eigen::Affine3d getProjectionMatrix() const { return projection; }

  // shift current view-plane in x and y
  void ProcessShift(double xoffset, double yoffset) {
    const Eigen::Vector3d move(xoffset * shift_sensitivity, -yoffset * shift_sensitivity, 0);
    moveXYZ(move);
  }

  // rotate current camera pose
  void ProcessRotation(double xoffset, double yoffset) {

    xoffset *= rotate_sensitivity;
    yoffset *= rotate_sensitivity;

    const Eigen::Vector3d d_angles(-yoffset, -xoffset, 0);


    // const Eigen::Quaterniond d_rot_screen = eigen_utils::rpy2Quaternion(d_angles);

    Eigen::Quaterniond d_rot_screen;
    d_rot_screen.w() = 0;
    d_rot_screen.vec() = d_angles;

    const Eigen::Quaterniond d_rot_camera = angles.inverse() * d_rot_screen * angles;
    // const Eigen::Quaterniond d_rot_camera = angles * d_rot_screen * angles.inverse();
    const Eigen::Vector3d d_angles_screen = d_rot_camera.vec();
    angles = angles * eigen_utils::rpy2Quaternion(d_angles_screen);

    updateView();
  }

  // processes input received from a mouse scroll-wheel event.
  void ProcessMouseScroll(double dscroll) {
    dscroll *= scroll_sensitivity;
    const Eigen::Vector3d move(0, 0, dscroll);
    moveXYZ(move);
    updateView();
  }

 private:
  void updateView() {
    const Eigen::Affine3d translation =
        eigen_utils::pose2Affine(position, Eigen::Vector3d::Zero());
    const Eigen::Affine3d rotation = eigen_utils::quaternion2Affine(angles);
    Eigen::Affine3d transformation = rotation * translation;
    transformation.inverse();
    view = transformation;
    callbackViewChange();
  }

  void moveXYZ(const Eigen::Vector3d &xyz) {
    position += angles.toRotationMatrix() * xyz;
    updateView();
  }

  void rotateRPY(const Eigen::Vector3d &rpy) {
    // const double pitch = angles(1);
    // const Eigen::Matrix3d r =
    //    eigen_utils::rpy2RotationMatrix(Eigen::Vector3d(0, 0, pitch));
    // angles += r * rpy;
    // angles += rpy;
    updateView();
  }

  void updateProjection() {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix

    // crunch every visible vertex into [-1,1]^3 unit qube

    assert(abs(aspect_ratio - std::numeric_limits<double>::epsilon()) >
           static_cast<double>(0));

    const double tanHalfFovy = tan(lense_angle_rad / 2.0);

    projection.matrix()(0, 0) = 1.0 / (aspect_ratio * tanHalfFovy);
    projection.matrix()(1, 1) = 1.0 / (tanHalfFovy);
    projection.matrix()(2, 2) = far_clipping / (far_clipping - near_clipping);
    projection.matrix()(2, 3) = 1;
    projection.matrix()(3, 2) =
        -(far_clipping * near_clipping) / (far_clipping - near_clipping);

    projection = Eigen::Affine3d::Identity();
    callbackProjectionChange();
  }

  // camera Attributes
  Eigen::Affine3d view = Eigen::Affine3d::Identity();
  Eigen::Affine3d projection = Eigen::Affine3d::Identity();
  Eigen::Vector3d position = Eigen::Vector3d::Zero();  // X,Y,Z
  Eigen::Quaterniond angles = eigen_utils::getZeroRotation(Eigen::Vector3d(0, 0, 1));  // vec,w

  double far_clipping = 1.;
  double near_clipping = 0;
  double lense_angle_rad =
      math::deg2Rad(30.);  //  field-of-view something between 30 and 90 looks ok
  double aspect_ratio = 1;

  // camera options
  double scroll_sensitivity = 0.01;
  double shift_sensitivity = 0.001;
  double rotate_sensitivity = 0.001;

  CallbackCameraChange callbackViewChange = ([] {});
  CallbackCameraChange callbackProjectionChange = ([] {});
};
#endif
