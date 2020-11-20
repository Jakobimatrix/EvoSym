#include "display.h"

#include <globals/macros.hpp>

Display::Display()
    : Settings(Globals::getInstance().getPath2DisplaySettings()) {
  put<int, 4>(disp_pos_size[0], DISP_POS_SIZE);
  put<unsigned int>(color_depth, COLOR_DEPTH_ID);
}

Display::~Display() {
  try {
    util::Settings::save();
  } catch (...) {
    F_ERROR("Failed to write into Display Settings: %s",
            Globals::getInstance().getPath2DisplaySettings().c_str());
  }
}

void Display::initSimulation() { WARNING("TODO"); }

void Display::setCurrentFile(const std::string& file) {
  if (strcmp(file.c_str(), "") == 0) {
    current_world_file = DEFAULT_FILE_NAME;
  } else {
    current_world_file = file;
  }

  setWindowFilePath(current_world_file);
}

bool Display::save() {
  const bool is_simulating = simulationIsRunning();
  if (is_simulating) {
    stopSimulation();
  }

  setStatus("Save " + current_world_file);
  const bool save_success = simulatedWorld.save(current_world_file);
  if (!save_success) {
    const std::string failed_msg = "Failed to save " + current_world_file;
    setStatus(failed_msg);
    popup_error(failed_msg);
    return false;
  }
  if (is_simulating) {
    startSimulation();
  }
  return true;
}

bool Display::saveAs(const std::string& file) {
  setStatus("Save As ...");
  setCurrentFile(file);
  return save();
}

void Display::openFile(const std::string& file) {
  setStatus("Loading " + file + " ...");
  WARNING("TODO");
  setCurrentFile(file);
}

bool Display::startSimulation() {
  if (simulationIsRunning()) {
    return false;
  }
  stop_simulation = false;
  simulation_thread = std::thread(&Display::runSimulation, this);
  setStatus("Run simulation ...");
  return true;
}

void Display::stopSimulation() {
  if (simulationIsRunning()) {
    setStatus("Stopping simulation ...");
    stop_simulation = true;
    simulation_thread.join();
    stop_simulation = false;
    setStatus("Stopping stoppend.");
  }
}

void Display::runSimulation() {
  while (!stop_simulation) {
    simulatedWorld.update();
  }
}

bool Display::exitGracefully() {
  DEBUG("EXIT GRACEFULLY");
  stopSimulation();
  if (need_save) {
    const bool save_changes = askYesNoQuestion(
        "The current Simulation has unsaved Results. Do you whish to save them "
        "before exiting?",
        "Current Simulation progress has not been saved yet!");
    if (save_changes) {
      return save();
    }
  }
  return true;
}
