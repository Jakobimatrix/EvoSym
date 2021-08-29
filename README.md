# EvoSym
- WIP: Porting [EvoSym_old](https://github.com/Jakobimatrix/EvoSym_old) from Windows to Linux




# dependencies
- eigen
  header only lib eg. via `sudo apt install libeigen3-dev`
- boost
  `sudo apt-get install libboost-all-dev`
- sfml
  `sudo apt-get install libsfml-dev`
- std::filesystem
  `needs gcc version 9 or you change the CMakeLists.txt and link against -lstdc++fs`
- Qt 5.9
  `sudo apt-get install qt5-default`

# third party submodule
- stb https://github.com/nothings/stb
- tinyxml2 (via settings) https://github.com/leethomason/tinyxml2
- warning_guards https://github.com/cwecht/warning_guards
- glm https://github.com/g-truc/glm
- glad (autogenerated) https://glad.dav1d.de/

## build Dokumentation

* `sudo apt install doxygen`
* `sudo apt install graphviz`
* run `doxywizard`, File > open > Doxyfile in the Doxygen folder.
* go to the run Tab and click run Doxygen
