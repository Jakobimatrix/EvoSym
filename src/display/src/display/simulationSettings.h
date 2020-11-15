#ifndef SIMULATION_SETTINGS
#define SIMULATION_SETTINGS

#include <strings.h>

#include <globals/globals.hpp>
#include <globals/macros.hpp>
#include <settings.hpp>


class SimulationSettings : public util::Settings {
 public:
  SimulationSettings()
      : Settings(Globals::getInstance().getPath2SimulationSettings()) {
    put<int>(target_fps, FPS_ID);
    put<int>(text_size, TEXT_SIZE_ID);
    WARNING("----------------------------");
  }

  double get_target_update_rate() {
    return 1. / static_cast<double>(target_fps);
  }

 private:
  int target_fps = 25;
  const std::string FPS_ID = "target_fps";

  int text_size = 25;
  const std::string TEXT_SIZE_ID = "text_size";
};

#endif
