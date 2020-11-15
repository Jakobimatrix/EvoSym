#include "world.h"

#include <globals/macros.hpp>

World::World() { WARNING("----------------------------"); }

void World::update() {
  // todo do calculations
}

bool World::save(const std::string& file) {
  F_DEBUG("Saving world to %s.", file.c_str());
  return false;
}


bool World::load(const std::string& file) { return false; }
