// This file if from: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

#include <Eigen/Core>
#include <functional>
#include <globals/macros.hpp>
#include <iostream>
#include <settings.hpp>
#include <utils/eigen_conversations.hpp>
#include <utils/math.hpp>
#include <utils/sanitize.hpp>
#include <vector>


class Camera : public util::Settings {
 public:
  typedef std::function<void()> CallbackCameraChange;

  // constructor with vectors
  Camera(const std::string &source) : Settings(source) {
    initSettings();
    projection = eigen_utils::getPerspectiveProjection(
        lense_angle_rad, near_clipping, far_clipping, aspect_ratio);
    updateView();
  }

  ~Camera() { save(); }

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

  const Eigen::Vector3d &getPosition() const { return position; }

  const Eigen::Quaterniond &getAngles() const { return angles; }

  void setCameraRotationInverted(bool invert) { invert_rotation = invert; }

  // from viewers perspective fix roll movement. In Camera coordinates that is the rotation around z-Axis (yaw)
  void setFixCameraRoll(bool fix) { fix_camera_yaw = fix; }

  void setLenseAngleRad(double angle) {
    // focal length must be positive. Good values are between 30 and 90
    if (angle > math::deg2Rad(1.)) {
      lense_angle_rad = angle;
      updateProjection();
    }
  }

  double getLenseAngleRad() { return lense_angle_rad; }

  void setClippingDistance(double near, double far) {
    near_clipping = near;
    far_clipping = far;
    updateProjection();
  }

  void setAspectRatio(double ratio) {
    aspect_ratio = ratio;
    updateProjection();
  }

  const Eigen::Isometry3d &getViewMatrix() const { return view; }

  const Eigen::Projective3d &getProjectionMatrix() const { return projection; }

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

  void rotatePYaround(double pitch, double yaw, double distance) {
    const Eigen::Vector3d p =
        position + eigen_utils::rotate(angles, Eigen::Vector3d(0, 0, distance));
    rotatePYaround(pitch, yaw, p);
  }

  void rotatePYaround(double pitch, double yaw, const Eigen::Vector3d &p_world) {

    if (invert_rotation) {
      pitch *= -rotate_sensitivity;
      yaw *= -rotate_sensitivity;
    } else {
      pitch *= rotate_sensitivity;
      yaw *= rotate_sensitivity;
    }

    const Eigen::Vector3d rotation_input(yaw, pitch, 0);

    // we want to rotate the camera (on a circle) around a point given in world frame:
    // move camera to that point (circle center)
    // rotate camera using existing rotation around the cameras center
    // move camera back to the circles border
    const double circle_diameter = (p_world - position).norm();
    const Eigen::Vector3d move_to_center(0, 0, circle_diameter);
    const Eigen::Vector3d move_from_center(0, 0, -circle_diameter);

    moveXYZ(move_to_center, false);
    rotateRPY(rotation_input, false);
    moveXYZ(move_from_center);
  }

  void rollCamera(double roll) {
    if (fix_camera_yaw) {
      WARNING(
          "Roll is currently disabled. To roll use setFixCameraRoll(flase).");
    }
    const Eigen::Vector3d d_angles(0, 0, rotate_sensitivity * roll);
    rotateRPY(d_angles);
  }

  void shiftZ(double dz) {
    dz *= scroll_sensitivity;
    // zoom == move camera along Z-axis
    const Eigen::Vector3d move(0, 0, dz);
    moveXYZ(move);
  }

 private:
  void initSettings() {
    put(far_clipping, SETTING_CLIPPING_FAR_ID);
    put(near_clipping, SETTING_CLIPPING_NEAR_ID);
    put(lense_angle_rad, SETTING_FOV_ID);

    put(scroll_sensitivity, SETTING_SCROLL_SENS_ID);
    put(shift_sensitivity, SETTING_SHIFT_SENS_ID);
    put(rotate_sensitivity, SETTING_ROTATION_SENS_ID);
    put(invert_rotation, SETTING_INVERT_INPUT_ID);
    put(fix_camera_yaw, SETTING_FIX_YAW_ID);

    put<double, NUM_COORDS_3D>(position.x(), SETTING_POSITION_ID);
    // quaternion data is saved [x,y,z,w]
    put<double, NUM_COORDS_QUATANION>(angles.x(), SETTING_ANGLES_ID);
    sanitizeSettings();
  }

  void sanitizeSettings() {
    math::abs(far_clipping);
    math::abs(near_clipping);
    math::swapIf(near_clipping, far_clipping);

    std::clamp(lense_angle_rad, MIN_FOV_RAD, MAX_FOV_RAD);

    math::abs(scroll_sensitivity);
    math::abs(shift_sensitivity);
    math::abs(rotate_sensitivity);
    sane::normalizedQuad(angles, eigen_utils::getZeroRotation<double>());
  }

  void moveXYZ(const Eigen::Vector3d &xyz, bool update_view = true) {
    position += eigen_utils::rotate(angles, xyz);
    if (update_view) {
      updateView();
    }
  }

  void rotateRPY(const Eigen::Vector3d &rpy, bool update_view = true) {
    // Rotate the vector of rpy such that it is rotated like the current camera pose.
    const Eigen::Vector3d d_angles_screen = eigen_utils::rotate(angles, rpy);
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
      constexpr int unit_last_place_precision = 7;
      if (!math::almost_equal(dot_product, 0., unit_last_place_precision)) {

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
        roll_cv = (y_axis_c.y() > 0) ? roll_cv : 2 * M_PI - roll_cv;

        // To decide in which direction we have to roll back, we look at
        // the the y_axis_c. If it looks down (relative to world), we have to
        // rotate counter clockwize if it looks up, we have to rotate clock
        // wize. We decide this by checking the x value
        if (y_axis_c.x() < 0) {
          roll_cv = -roll_cv;
        }

        // If fore some reson you want y to be looking down all the time, switch
        // the > and < in the lats to if's

        // just roll back by calling this function again
        fix_camera_yaw = false;  // make sure not to have an infinit loop.
        rotateRPY(Eigen::Vector3d(0, 0, roll_cv), update_view);
        fix_camera_yaw = true;  // set it back true
        // No need to update the view again.
        return;
      }
    }
    if (update_view) {
      updateView();
    }
  }

  void updateView() {
    const Eigen::Vector3d zero_rotation(0, 0, 0);
    const Eigen::Isometry3d translation =
        eigen_utils::pose2Isometry(position, zero_rotation);
    const Eigen::Isometry3d rotation = eigen_utils::quaternion2Isometry(angles);
    Eigen::Isometry3d transformation = rotation * translation;
    transformation.inverse(Eigen::TransformTraits::Isometry);
    view = transformation;
    callbackViewChange();
  }

  void updateProjection() {
    // projection must have been initialized before calling this!
    eigen_utils::updatePerspectiveProjection(
        projection, lense_angle_rad, near_clipping, far_clipping, aspect_ratio);
    callbackProjectionChange();
  }

  // camera Attributes
  Eigen::Isometry3d view = Eigen::Isometry3d::Identity();
  Eigen::Projective3d projection;
  Eigen::Vector3d position = Eigen::Vector3d::Zero();                  // X,Y,Z
  Eigen::Quaterniond angles = eigen_utils::getZeroRotation<double>();  // vec,w

  double far_clipping = 500.;
  double near_clipping = 0.1;
  double lense_angle_rad = DEFAULT_FOV_RAD;
  // field-of-view something between 30 and 90 looks ok
  static constexpr double DEFAULT_FOV_RAD = 30. / 360. * M_PI;
  static constexpr double MIN_FOV_RAD = 1. / 360. * M_PI;
  static constexpr double MAX_FOV_RAD = 120. / 360. * M_PI;

  double aspect_ratio = 1;

  // camera options
  double scroll_sensitivity = 0.07;
  double shift_sensitivity = 0.001;
  double rotate_sensitivity = 0.001;
  bool invert_rotation = false;
  bool fix_camera_yaw = true;  // rotate around cameras z-axis

  CallbackCameraChange callbackViewChange = ([] {});
  CallbackCameraChange callbackProjectionChange = ([] {});

  static constexpr int NUM_COORDS_3D = 3;
  static constexpr int NUM_COORDS_QUATANION = 4;
  static constexpr const char *SETTING_CLIPPING_FAR_ID = "clipping_plane_far";
  static constexpr const char *SETTING_CLIPPING_NEAR_ID = "clipping_plane_near";
  static constexpr const char *SETTING_FOV_ID = "fov_rad";

  static constexpr const char *SETTING_SCROLL_SENS_ID = "sensitivity_scroll";
  static constexpr const char *SETTING_SHIFT_SENS_ID = "sensitivity_schift";
  static constexpr const char *SETTING_ROTATION_SENS_ID =
      "sensitivity_rotation";
  static constexpr const char *SETTING_INVERT_INPUT_ID = "invert_input";
  static constexpr const char *SETTING_FIX_YAW_ID = "yaw_fixed";

  static constexpr const char *SETTING_POSITION_ID = "pose_trans_xyz";
  static constexpr const char *SETTING_ANGLES_ID = "pose_rot_xyzw";
};
#endif
