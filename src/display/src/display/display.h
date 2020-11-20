#ifndef DISPLAY_H
#define DISPLAY_H

#include <world/world.h>

#include <array>
#include <settings.hpp>
#include <string>
#include <thread>

#include "simulationSettings.h"


// This parent class handels all the savings/loadings and other non visual related tasks
// thouse are handled by the respecting child class which uses a lib or whatever to actually
// display things. This allowes changeing the engine which displays without needing to rewrite
// code regarding the functions apart from displaying things.
class Display : public util::Settings {
 public:
  Display();

  virtual ~Display();

 protected:
  // user interaktions
  [[nodiscard]] bool save();
  [[nodiscard]] bool saveAs(const std::string& file);
  void openFile(const std::string& file);

  const std::string& getCurrentFileName() { return current_world_file; }

  /*!
   * \brief Implement a popup displaying title and question with user yes no
   * option.
   * \param question The question displayed to the user.
   * \param title A title displayed over the question.
   * \return Returns the users decision.
   */
  virtual bool askYesNoQuestion(const std::string& question,
                                const std::string& title = "") = 0;

  /*!
   * \brief Implement a popup displaying title and information.
   * option.
   * \param text The information displayed to the user.
   * \param title A title displayed over the question.
   */
  virtual void popup_info(const std::string& text,
                          const std::string& title = "") = 0;

  /*!
   * \brief Implement a popup displaying title and warning.
   * option.
   * \param warning The warning displayed to the user.
   * \param title A title displayed over the question.
   */
  virtual void popup_warning(const std::string& warning,
                             const std::string& title = "") = 0;

  /*!
   * \brief Implement a popup displaying title and error.
   * option.
   * \param error The error message displayed to the user.
   * \param title A title displayed over the question.
   */
  virtual void popup_error(const std::string& error,
                           const std::string& title = "") = 0;

  /*!
   * \brief Implement a popup displaying about this software.
   * Implementation and call is up to child class.
   */
  virtual void about() = 0;

  /*!
   * \brief Display given string in head area of the window to
   * show which file is loaded.
   * \param file_path The path displayed to the user.
   */
  virtual void setWindowFilePath(const std::string& file_path) = 0;

  /*!
   * \brief Display status string in bottom area of the window to
   * show current information.
   * \param msg The information displayed to the user.
   * \param timeout_ms time in ms when the status shall disapear.
   * if timeout = 0, display until other status is pushed.
   */
  virtual void setStatus(const std::string& msg, int timeout_ms = 0) = 0;

  bool hasUnsavedChanges() const { return need_save; }

  [[nodiscard]] bool exitGracefully();

  const std::array<int, 4>& getDisplayProportions() const {
    return disp_pos_size;
  }
  int getDisplayPosX() const { return disp_pos_size[0]; }
  int getDisplayPosY() const { return disp_pos_size[1]; }
  int getDisplaySizeW() const { return disp_pos_size[2]; }
  int getDisplaySizeH() const { return disp_pos_size[3]; }

  void saveDisplayProperties(int x, int y, int w, int h) {
    disp_pos_size[0] = x;
    disp_pos_size[1] = y;
    disp_pos_size[2] = w;
    disp_pos_size[3] = h;
  }

  void saveDisplaySize(int w, int h) {
    disp_pos_size[2] = w;
    disp_pos_size[3] = h;
  }

  void saveDisplayPosition(int x, int y) {
    disp_pos_size[0] = x;
    disp_pos_size[1] = y;
  }

  unsigned int getColorDepth() const { return color_depth; }
  void saveColorDepth(unsigned int color_depth) {
    this->color_depth = color_depth;
  }

 private:
  void runSimulation();

  bool simulationIsRunning() { return simulation_thread.joinable(); }

  void stopSimulation();

  bool startSimulation();

  void setCurrentFile(const std::string& file);

  const std::string DEFAULT_FILE_NAME = std::string("new_world.evsm");
  std::string current_world_file = DEFAULT_FILE_NAME;
  SimulationSettings simulationSettings;
  World simulatedWorld;

  // simulation
  void initSimulation();
  std::thread simulation_thread;
  bool pause_simulation = false;
  bool stop_simulation = false;
  bool need_save = false;

  // SETTINGS
  std::array<int, 4> disp_pos_size = {{50, 50, 600, 400}};
  const std::string DISP_POS_SIZE = "display_x_y_w_h";
  unsigned int color_depth = 24;
  const std::string COLOR_DEPTH_ID = "ColorDepth";
};

#endif
