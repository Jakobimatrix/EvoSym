#ifndef MENU
#define MENU

#include <strings.h>

#include <globals/globals.hpp>
#include <settings.hpp>

class Menu : public util::Settings {
 public:
  Menu(const std::string& abs_path) : Settings(abs_path) {
    put<int>(text_size, TEXT_SIZE_ID);
  }

  ~Menu() { save(); }

 private:
  int text_size = 25;
  const std::string TEXT_SIZE_ID = "text_size";
};

class SimulationSettings : public Menu {
 public:
  SimulationSettings()
      : Menu(Globals::getInstance().getPath2SimulationSettings()) {
    put<int>(target_fps, FPS_ID);
  }


  double get_target_update_rate() {
    return 1. / static_cast<double>(target_fps);
  }

 private:
  int target_fps = 25;
  const std::string FPS_ID = "target_fps";
};

#endif
