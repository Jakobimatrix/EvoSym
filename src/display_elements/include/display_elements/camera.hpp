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
    angles << roll, pitch, yaw;
    updateView();
  }
  void setAngles(const Eigen::Vector3d &angles) {
    this->angles = angles;
    updateView();
  }

  void setPosition(double x, double y, double z) {
    position << x, y, z;
    updateView();
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

  double getZoom() const { return -zoom; }

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
    updateView();
  }

 private:
  void updateView() {
    const Eigen::Affine3d translation =
        eigen_utils::pose2Affine(position, Eigen::Vector3d::Zero(), 1);
    const Eigen::Affine3d rotation =
        eigen_utils::pose2Affine(Eigen::Vector3d::Zero(), angles, 1);
    Eigen::Affine3d transformation = rotation * translation;
    transformation.inverse();
    eigen_utils::scaleAffine(transformation, std::exp(zoom));
    view = transformation;
    callbackViewChange();
  }
  void moveXYZ(const Eigen::Vector3d &xyz) {
    position += xyz;
    updateView();
  }

  void rotateRPY(const Eigen::Vector3d &rpy) {
    angles += rpy;
    updateView();
  }

  void updateProjection() {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix
    assert(abs(aspect_ratio - std::numeric_limits<double>::epsilon()) >
           static_cast<double>(0));

    const double tanHalfFovy = tan(lense_angle_rad / 2.0);

    projection.matrix()(0, 0) = 1.0 / (aspect_ratio * tanHalfFovy);
    projection.matrix()(1, 1) = 1.0 / (tanHalfFovy);
    projection.matrix()(2, 2) = far_clipping / (far_clipping - near_clipping);
    projection.matrix()(2, 3) = 1;
    projection.matrix()(3, 2) =
        -(far_clipping * near_clipping) / (far_clipping - near_clipping);
    callbackProjectionChange();
  }

  // camera Attributes
  Eigen::Affine3d view = Eigen::Affine3d::Identity();
  Eigen::Affine3d projection = Eigen::Affine3d::Identity();
  Eigen::Vector3d position = Eigen::Vector3d::Zero();  // X,Y,Z
  Eigen::Vector3d angles = Eigen::Vector3d(0, 0, 0);   // R,P,Y
  double zoom = 1;

  double far_clipping = 1.;
  double near_clipping = 0.9;
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
