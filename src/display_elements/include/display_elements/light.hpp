#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <display_elements/shadows.hpp>
#include <functional>
#include <memory>
#include <settings.hpp>
#include <utils/eigen_conversations.hpp>
#include <utils/minmax.hpp>
#include <utils/sanitize.hpp>

class Light : public util::Settings {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  typedef std::function<void()> CallbackLightChange;

  Light(const std::string &source) : Settings(source) {
    putSettings();
    set(position, direction);
  }

  Light(const std::string &source, const Eigen::Vector3f &pos, const Eigen::Vector3f &dir)
      : Settings(source) {
    putSettings();
    set(pos, dir);
  }

  ~Light() { save(); }

  void set(const Eigen::Vector3f &pos, const Eigen::Vector3f &dir) {
    position = pos;
    if (dir.norm() > 0.0001) {
      // make sure direction is not 0,0,0
      direction = dir;
      direction.normalize();
    }

    pose = eigen_utils::getTransformation(pos, direction);
    lightSpaceMatrix = lightOrthProjection * pose.inverse(Eigen::TransformTraits::Isometry);
    /*
    // TODO use min max to find following values
    const float left = -10;
    const float right = 10;
    const float top = -10;
    const float bot = 10;
    const float near = 1;
    const float far = 100;
    eigen_utils::updateOrthogonalProjection(
        lightSpaceMatrix, left, right, bot, top, near, far);
    */

    callbackLightChange();
  }


  void debugShadowTexture() { shadow_ptr->drawDebug(); }

  bool hasShadow() { return shadow_ptr != nullptr; }

  void setShaddow() { shadow_ptr = std::make_shared<Shadows>(); }

  unsigned int getDepthMapTexture() { return shadow_ptr->getDepthMapTexture(); }

  unsigned int getDepthMapFrameBufferInt() {
    return shadow_ptr->getDepthMapFrameBufferInt();
  }

  void setPositionAndTarget(const Eigen::Vector3f &pos, const Eigen::Vector3f &target) {
    set(pos, target - pos);
  }

  void setPosition(const Eigen::Vector3f &pos) { set(pos, direction); }

  void setCallbackPositionChange(CallbackLightChange &func) {
    callbackLightChange = func;
  }

  const Eigen::Vector3f &getPosition() const { return position; }
  const Eigen::Vector3f &getDirection() const { return direction; }
  const Eigen::Vector3f &getAmbient() const { return ambient; }
  const Eigen::Vector3f &getColor() const { return color; }
  const Eigen::Isometry3f &getPose() const { return pose; }
  const Eigen::Projective3f &getLightOrthProjection() const {
    return lightOrthProjection;
  }
  const Eigen::Projective3f &getLightSpaceMatrix() const {
    return lightSpaceMatrix;
  }

 private:
  void putSettings() {
    put<float, NUM_COORDS>(position.x(), SETTING_POSITION_ID);
    put<float, NUM_COORDS>(direction.x(), SETTING_DIRECTION_ID);
    put<float, NUM_COLORS>(ambient.x(), SETTING_AMBIENT_COLOR_ID);
    put<float, NUM_COLORS>(color.x(), SETTING_COLOR_COLOR_ID);


    // todo temp
    put<float>(left, "left");
    put<float>(right, "right");
    put<float>(top, "top");
    put<float>(bot, "bot");
    put<float>(near, "near");
    put<float>(far, "far");

    sanitizeSettings();
  }

  void sanitizeSettings() {
    sane::normalized3D(direction, Eigen::Vector3f(DEFAULT_DIRECTION.data()));
    eigen_utils::clampElements(ambient, 0.f, 1.f);
    eigen_utils::clampElements(color, 0.f, 1.f);


    // TODO WO ANDERS HIN
    lightOrthProjection =
        eigen_utils::getOrthogonalProjection(left, right, bot, top, near, far);

    // strange rotation bug
    const Eigen::Vector3f rotate(0, M_PI, 0);
    const Eigen::Isometry3f R = eigen_utils::rpy2Isometry(rotate);
    lightSpaceMatrix =
        lightOrthProjection * R * pose.inverse(Eigen::TransformTraits::Isometry);
  }

  static constexpr int NUM_COLORS = 3;
  static constexpr int NUM_COORDS = 3;
  static constexpr const char *SETTING_POSITION_ID = "position_xyz";
  static constexpr const char *SETTING_DIRECTION_ID = "direction_xyz";
  static constexpr const char *SETTING_AMBIENT_COLOR_ID = "ambiente_rgb";
  static constexpr const char *SETTING_COLOR_COLOR_ID = "color_rgb";

  Eigen::Vector3f position = Eigen::Vector3f(0.f, 0.f, 2.f);
  Eigen::Vector3f direction = Eigen::Vector3f(DEFAULT_DIRECTION.data());
  static constexpr std::array<float, 3> DEFAULT_DIRECTION = {{0.f, 0.f, 1.f}};
  // brightness in shaddows
  Eigen::Vector3f ambient = Eigen::Vector3f(0.05f, 0.05f, 0.05f);
  // brightness direct hit
  Eigen::Vector3f color = Eigen::Vector3f(1.f, 1.f, 1.f);
  // brightness reflection
  Eigen::Isometry3f pose;
  Eigen::Projective3f lightSpaceMatrix;
  Eigen::Projective3f lightOrthProjection;

  float left = -30;
  float right = 30;
  float top = 30;
  float bot = -30;
  float near = 2;
  float far = 1000;

  CallbackLightChange callbackLightChange = ([] {});
  std::shared_ptr<Shadows> shadow_ptr;
};

#endif
