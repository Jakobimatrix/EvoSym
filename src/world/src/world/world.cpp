#include "world.h"

#include <globals/globals.hpp>
#include <globals/macros.hpp>

World::World() {}


void World::init() {}

void World::update() {
  // todo do calculations
}

bool World::save(const std::string& file) {
  F_DEBUG("Saving world to %s.", file.c_str());
  return false;
}


bool World::load(const std::string& file) { return false; }

bool World::load_mesh(const std::string& mesh_file) {
  WARNING("TODO");
  return false;
}

void World::create_mesh() {
  const std::string path = Globals::getInstance().getAbsPath2Shaders();
  const std::string vs = path + "test_1.vs";
  const std::string fs = path + "test_1.fs";
  world_shader = std::make_shared<Shader>(Shader(vs.c_str(), fs.c_str()));



  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  world_mesh = std::make_shared<Mesh>(Mesh(vertices, indices, textures));
}
