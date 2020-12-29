#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <Eigen/Geometry>

struct Material {
  // how strong the material emits light
  Eigen::Vector3f self_glow;
  // how strong each (rgb) will be reflected
  Eigen::Vector3f diffuse;
  // how strong the reflection of shiny object is
  Eigen::Vector3f specular;
  // how large the reflection/light spot becomes
  float shininess;
  bool initiated = false;
};

struct Chrome : public Material {
  Chrome() {
    self_glow << 0.0f, 0.0f, 0.0f;
    diffuse << 1.0f, 1.0f, 1.0f;
    specular << 1.0f, 1.0f, 1.0f;
    shininess = 64.0f;
    initiated = true;
  }
};

struct PureColor : public Material {
  PureColor() {
    self_glow << 1.f, 1.f, 1.f;
    diffuse << 0.f, 0.f, 0.f;
    specular << 0.f, 0.f, 0.f;
    shininess = 0.0f;
    initiated = true;
  }
};


#endif
