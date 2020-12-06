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

  void setCameraRotationInverted(bool invert) { invert_rotation = invert; }

  // from viewers perspective fix roll movement. In Camera coordinates that is the rotation around z-Axis (yaw)
  void setFixCameraRoll(bool fix) { fix_camera_yaw = fix; }

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
  void shiftXY(double xoffset, double yoffset) {
    const Eigen::Vector3d move(xoffset * shift_sensitivity, -yoffset * shift_sensitivity, 0);
    moveXYZ(move);
  }

  // rotate current camera pose
  void rotatePY(double pitch, double yaw) {
    if (invert_rotation) {
      pitch *= rotate_sensitivity;
      yaw *= rotate_sensitivity;
    } else {
      pitch *= -rotate_sensitivity;
      yaw *= -rotate_sensitivity;
    }
    // z- axis is parallel to the view of the camera
    rotateRPY(Eigen::Vector3d(yaw, pitch, 0));
  }

  void rollCamera(double roll) {
    const Eigen::Vector3d d_angles(0, 0, rotate_sensitivity * roll);
    rotateRPY(d_angles);
  }

  // processes input received from a mouse scroll-wheel event.
  void ProcessMouseScroll(double dscroll) {
    dscroll *= scroll_sensitivity;
    // zoom == move camera along Z-axis
    const Eigen::Vector3d move(0, 0, dscroll);
    moveXYZ(move);
  }

 private:
  void moveXYZ(const Eigen::Vector3d &xyz) {
    Eigen::Quaterniond d_rot_screen;
    d_rot_screen.w() = 0;
    d_rot_screen.vec() = xyz;

    const Eigen::Quaterniond d_shift_camera = angles.inverse() * d_rot_screen * angles;
    position += d_shift_camera.vec();
    updateView();
  }

  void rotateRPY(const Eigen::Vector3d &rpy) {
    Eigen::Quaterniond d_rot_screen;
    d_rot_screen.w() = 0;
    d_rot_screen.vec() = rpy;

    // Rotate the vector of rpy such that it is rotated like the current camera pose.
    const Eigen::Quaterniond d_rot_camera = angles.inverse() * d_rot_screen * angles;
    // Retrive the rotated rpy.
    const Eigen::Vector3d d_angles_screen = d_rot_camera.vec();
    // Add the rotated rpy angles to the camera rotation.
    angles = angles * eigen_utils::rpy2Quaternion(d_angles_screen);

    // Rotating around roll and pitch if there is already a roll and pitch != on
    // the cameras pose causes also a rotation around z (yaw). This might not be
    // wanted since that is like tilting your head (vision).
    if (fix_camera_yaw) {
      // In euler notation we rotate (left to right) Rx*Ry*Rz
      // The rotation is always around the unit vectors of the old Coordinate system.
      // Rz should be Identity matrix for we dont want to rotate around yaw.
      const Eigen::Matrix3d R = angles.toRotationMatrix();
      // const Eigen::Vector3d y_axis_c = R * Eigen::Vector3d::UnitY();
      const Eigen::Vector3d y_axis_c(R(0, 1), R(1, 1), R(2, 1));
      // If it is a purely rotation of Rx*Ry that would mean that the x_axis_c
      // of the camera frame alsways stays in the plane P {[0,1,0]',[0,0,1]'}.
      // If its not, we have an rotation about yaw.
      // Calculate the ange beween y_axis_c and P
      // Since P is constructed by unit vectors the projection of x_axis_c onto P is
      // const Eigen::Vector3d projection(0,x_axis_c.y(),x_axis_c.z());
      // Now the angle in question is the angle between x_axis_c and projection.
      // this can be done by dot product. We can simplyfy the dotproduct since projection.y = 0
      const double dot_product =
          y_axis_c.y() * y_axis_c.y() + y_axis_c.z() * y_axis_c.z();

      // One special case which numerically should not happen: dot_product = 0.
      // In that case cos_roll = 0/0. That happens if R(0, 1) = sin(roll)*cos(pitch) = 1
      // This corresponds to (roll = pi/2 && pitch = 0) || (roll = -pi/2 && pitch = pi)
      // in that case the rotation around y canot be distinguished from the rotation around z
      // Thats called gimbal lock. We just avoid this.
      if (!math::almost_equal(dot_product, 0., 7)) {

        // constexpr double norm_y_axis_c = 1;  // is always 1 doesnt matter how we rotate it
        // Funny: the norm of the projection is the sqrt of the dot product in our case.
        const double norm_projection = std::sqrt(dot_product);

        // const double cos_roll = dot_product / (norm_projection * norm_y_axis_c);
        double cos_roll = dot_product / (norm_projection);

        // numerical problems might occur, but as you can see cos_roll
        // should be always be smaller than 1
        if (cos_roll > 1) {
          cos_roll = 1.;
        }
        // If |cos_roll| > 1, a domain error occurs and NaN is returned.
        // cv == controlled variable
        double roll_cv = std::acos(cos_roll);

        // But this will always give an angle in [0,90].
        // To decide if the cos_angle is more or less than 90° we have to
        // see if the y component of y_axis_c is negative
        // This happens if fix_camera_yaw is false and the camera gets rotated
        // (since it can in that situation) more than 90 deg (Image is now wrong
        // way)
        roll_cv = (y_axis_c.y() < 0) ? roll_cv : 2 * M_PI - roll_cv;

        // To decide in which direction we have to roll back, we look at
        // the the y_axis_c. If it looks down (relative to world), we have to
        // rotate clockwize if it looks up, we have to rotate counter clock
        // wize. We decide this by checking the x value
        if (y_axis_c.x() > 0) {
          roll_cv = -roll_cv;
        }

        // just roll back by calling this function again
        fix_camera_yaw = false;  // make sure not to have an infinit loop.
        rotateRPY(Eigen::Vector3d(0, 0, roll_cv));
        fix_camera_yaw = true;  // set it back true
        // No need to update the view again.
        return;
      }
    }
    updateView();
  }

  void updateView() {
    const Eigen::Affine3d translation =
        eigen_utils::pose2Affine(position, Eigen::Vector3d::Zero());
    const Eigen::Affine3d rotation = eigen_utils::quaternion2Affine(angles);
    Eigen::Affine3d transformation = rotation * translation;
    transformation.inverse();
    view = transformation;
    callbackViewChange();
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

  double far_clipping = 10000.;
  double near_clipping = 0.001;
  double lense_angle_rad =
      math::deg2Rad(30.);  //  field-of-view something between 30 and 90 looks ok
  double aspect_ratio = 1;

  // camera options
  double scroll_sensitivity = 0.01;
  double shift_sensitivity = 0.001;
  double rotate_sensitivity = 0.001;
  bool invert_rotation = false;
  bool fix_camera_yaw = true;  // rotate around cameras z-axis

  CallbackCameraChange callbackViewChange = ([] {});
  CallbackCameraChange callbackProjectionChange = ([] {});
};
#endif
