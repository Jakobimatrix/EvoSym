#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <Eigen/Geometry>

struct Material {
  Eigen::Vector3f ambient;
  Eigen::Vector3f diffuse;
  Eigen::Vector3f specular;
  float shininess;
  bool initiated = false;
};

struct Chrome : public Material {
  Chrome() {
    ambient << 0.25f, 0.25f, 0.25f;
    diffuse << 0.4f, 0.4f, 0.4f;
    specular << 0.774597f, 0.774597f, 0.774597f;
    shininess = 0.6f;
    initiated = true;
  }
};


#endif
