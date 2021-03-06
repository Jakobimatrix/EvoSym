#ifndef GLOBALS
#define GLOBALS

#include <string.h>

#include <filesystem>  // if using compiler < c++17 use <experimental/filesystem ans std::experimental::filesystem
#include <stdexcept>
#include <vector>


class Globals {
 private:
  Globals() { init_paths(); }
  // Stop the compiler generating methods of copy the object
  Globals(Globals const& copy);             // Not Implemented
  Globals& operator=(Globals const& copy);  // Not Implemented

 public:
  /*!
   * \brief private Get the one instance of the class
   * \return A reference to the one existing instance of this class.
   */
  static const Globals& getInstance() {
    static Globals instance;
    return instance;
  }

 private:
  void init_paths() {
    namespace fs = std::filesystem;

    const fs::path current_path = fs::current_path();
    auto path_it = current_path.begin();
    fs::path abs_path_to_base = fs::path("");
    for (; path_it != current_path.end(); path_it++) {
      abs_path_to_base.append(path_it->c_str());
      if (strcmp(path_it->c_str(), REPRO_FOLDER_NAME.c_str()) == 0) {
        break;
      }
    }

    absolute_path_to_base = abs_path_to_base.c_str() + PATH_SEPERATOR;
    absolute_path_to_executable = current_path.c_str() + PATH_SEPERATOR;
    absolute_path_to_resources = absolute_path_to_base + RESOURCES_FOLDER_NAME + PATH_SEPERATOR;
    absolute_path_to_shaders = absolute_path_to_base + SHADERS_FOLDER_NAME + PATH_SEPERATOR;
    absolute_path_to_save_files = absolute_path_to_base + SAVE_FOLDER_NAME + PATH_SEPERATOR;
    absolute_path_to_settings = absolute_path_to_base + SETTINGS_FOLDER_NAME + PATH_SEPERATOR;

    if (!fs::exists(absolute_path_to_resources)) {
      std::runtime_error("The expected Path " + absolute_path_to_resources +
                         " to " + RESOURCES_FOLDER_NAME + " does not exist.");
    }
    // if the above exists, the general path is correct!
    // just create the folders if the below path do not exist.

    if (!fs::exists(absolute_path_to_save_files)) {
      fs::create_directory(absolute_path_to_save_files);
    }
    if (!fs::exists(absolute_path_to_settings)) {
      fs::create_directory(absolute_path_to_settings);
    }
  }

 public:
  std::string getPath2SimulationSettings() const {
    return absolute_path_to_settings + FILE_NAME_SIM_SETTINGS;
  }

  std::string getPath2LightSettings() const {
    return absolute_path_to_settings + FILE_NAME_LIGHT_SETTINGS;
  }

  std::string getPath2CameraSettings() const {
    return absolute_path_to_settings + FILE_NAME_CAMERA_SETTINGS;
  }

  std::string getPath2DisplaySettings() const {
    return absolute_path_to_settings + FILE_NAME_DISPLAY_SETTINGS;
  }

  std::string getMainVidowTitle() const {
    return MAIN_WINDOW_NAME + " | V." + VERSION + " - " + VERSION_NAME;
  }

  const std::string& getAbsPath2Resources() const {
    return absolute_path_to_resources;
  }

  const std::string& getAbsPath2Shaders() const {
    return absolute_path_to_shaders;
  }


  void getAllSavedWorldFiles(std::vector<std::filesystem::path>& files) const {
    for (const auto& entry : std::filesystem::directory_iterator(absolute_path_to_save_files))
      files.push_back(entry);
  }

 private:
  // Absolute paths to folders
  std::string absolute_path_to_base;
  std::string absolute_path_to_resources;
  std::string absolute_path_to_shaders;
  std::string absolute_path_to_executable;
  std::string absolute_path_to_save_files;
  std::string absolute_path_to_settings;

  std::string absolute_path_to_settings_menue;
  std::string absolute_path_to_settings_display;


  // Folder names
  const std::string REPRO_FOLDER_NAME = std::string("EvoSym");
  const std::string RESOURCES_FOLDER_NAME = std::string("resources");
  const std::string SHADERS_FOLDER_NAME = std::string("shaders");
  const std::string SAVE_FOLDER_NAME = std::string("simulated_data");
  const std::string SETTINGS_FOLDER_NAME = std::string("settings");

  // File names
  const std::string FILE_NAME_DISPLAY_SETTINGS =
      std::string("display_settings.txt");

  const std::string FILE_NAME_SIM_SETTINGS =
      std::string("simulation_settings.txt");

  const std::string FILE_NAME_LIGHT_SETTINGS =
      std::string("light_settings.txt");

  const std::string FILE_NAME_CAMERA_SETTINGS =
      std::string("camera_settings.txt");

  const std::string PATH_SEPERATOR =
#ifdef _WIN32
      std::string("\\");
#else
      std::string("/");
#endif

  // Strings
  const std::string MAIN_WINDOW_NAME = std::string("EvoSym");
  const std::string VERSION_NAME = std::string("Abyssinian");
  const std::string VERSION = std::string("0.1");
};

#endif
