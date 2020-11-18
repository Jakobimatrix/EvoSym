#ifndef LAYER
#define LAYER

#include <warning_guards/warning_guards.h>

#include <Eigen/Core>

THIRD_PARTY_HEADERS_BEGIN
#include <settings.hpp>
THIRD_PARTY_HEADERS_END

class Properties : public util::Settings {
 public:
};

class PhyscalProperties : public Properties {
  double height;
  double temperature;
  double density;

  Eigen::Vector3d density_gradient;
  Eigen::Vector3d temperature_gradient;
  Eigen::Vector3d mass_flow_gradient;
};

class NutritionProperties : public Properties {
  double plants;
};

enum LayerTyp { AIR, WATER, GROUND };

class Layer : public util::Settings {
 public:
 private:
  LayerTyp type;

  PhyscalProperties physical_properies;
};

class AirLayer : public Layer {};

class WaterLayer : public Layer {};

class GroundLayer : public Layer {};



#endif
