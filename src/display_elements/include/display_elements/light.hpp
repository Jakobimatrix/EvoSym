#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <Eigen/Geometry>
#include <functional>
#include <settings.hpp>
#include <utils/eigen_conversations.hpp>
#include <utils/minmax.hpp>

class Light : public util::Settings {
 public:
  typedef std::function<void()> CallbackLightChange;

  Light(const std::string &source) : Settings(source) {
    putSettings();
    set(position, direction);
  }

  Light(const std::string &source, const Eigen::Vector3f &pos, const Eigen::Vector3f &dir)
      : Settings(source) {
    putSettings();
    position = pos;
    direction = dir;
    set(position, direction);
  }

  ~Light() { save(); }

  void set(const Eigen::Vector3f &pos, const Eigen::Vector3f &dir) {
    position = pos;
    direction = dir;
    direction.normalize();
    pose = eigen_utils::getTransformation(pos, direction);

    // TODO use min max to find following values
    const float left = -10;
    const float right = 10;
    const float top = -10;
    const float bot = 10;
    const float near = 1;
    const float far = 100;
    eigen_utils::updateOrthogonalProjection(projection, left, right, bot, top, near, far);

    callbackLightChange();
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

 private:
  void putSettings() {
    put<float, NUM_COORDS>(position.x(), SETTING_POSITION_ID);
    put<float, NUM_COORDS>(direction.x(), SETTING_DIRECTION_ID);
    put<float, NUM_COLORS>(ambient.x(), SETTING_AMBIENT_COLOR_ID);
    put<float, NUM_COLORS>(color.x(), SETTING_COLOR_COLOR_ID);
  }

  static constexpr int NUM_COLORS = 3;
  static constexpr int NUM_COORDS = 3;
  static constexpr const char *SETTING_POSITION_ID = "position_xyz";
  static constexpr const char *SETTING_DIRECTION_ID = "direction_xyz";
  static constexpr const char *SETTING_AMBIENT_COLOR_ID = "ambiente_rgb";
  static constexpr const char *SETTING_COLOR_COLOR_ID = "color_rgb";

  Eigen::Vector3f position = Eigen::Vector3f(0.f, 0.f, 2.f);
  Eigen::Vector3f direction = Eigen::Vector3f(0.f, 0.f, 1.f);
  // brightness in shaddows
  Eigen::Vector3f ambient = Eigen::Vector3f(0.05f, 0.05f, 0.05f);
  // brightness direct hit
  Eigen::Vector3f color = Eigen::Vector3f(1.f, 1.f, 1.f);
  // brightness reflection
  Eigen::Isometry3f pose;
  Eigen::Projective3f lightSpaceMatrix;
  Eigen::Projective3f projection;
  CallbackLightChange callbackLightChange = ([] {});
};

#endif
