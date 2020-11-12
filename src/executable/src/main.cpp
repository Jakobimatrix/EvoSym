
/*
#include <SFML/Graphics.hpp>
#include <eigen3/Eigen/Core>
#include <globals/globals.hpp>
#include <homography/homography.hpp>
#include <iostream>
#include <randomGenerator.hpp>
#include <settings.hpp>
#include <timer/timer.hpp>

class ExampleClass : public util::Settings {
 public:
  ExampleClass(const std::string& source_file_name)
      : Settings(source_file_name) {
    // Introduce all membervariables which shall be saved
    // or loaded from the given file.
    // This should be done in the constructor.
    // All the membervariables should have a default
    // value, otherwize whatever value they point to
    // will be safed in the file.
    put<bool>(exampleBool, BOOL_STRNG_ID);
    put<int>(exampleInt, INT_STRNG_ID);
    put<float>(exampleFloat, F_STRING_ID);
    put<double>(exampleDouble, S_STRING_ID);
    put<double, NUM_D_IN_ARRAY>(example_array[0], ARRAY_ID);
  }

  ~ExampleClass() {}

  void print() {
    std::cout << "------<ExampleClass>------\n"
              << BOOL_STRNG_ID << +": " << exampleBool << "\n"
              << INT_STRNG_ID << +": " << exampleInt << "\n"
              << F_STRING_ID << +": " << exampleFloat << "\n"
              << S_STRING_ID << +": " << exampleDouble << "\n"
              << ARRAY_ID << +": "
              << "\n";
    for (size_t i = 0; i < NUM_D_IN_ARRAY; i++) {
      std::cout << "\t[" << i << "] " << std::to_string(example_array[i]) << "\n";
    }
    std::cout << "------</ExampleClass>------" << std::endl;
  }

 private:
  bool exampleBool = true;
  int exampleInt = 42;
  float exampleFloat = 3.141592653589793f;
  double exampleDouble = 3.141592653589793;

  static constexpr int NUM_D_IN_ARRAY = 3;
  std::array<double, NUM_D_IN_ARRAY> example_array;

  // These must be unique
  // These names will be the identifiers in the xml.
  const std::string BOOL_STRNG_ID = "bool";
  const std::string INT_STRNG_ID = "integer";
  const std::string F_STRING_ID = "nearly_pi";
  const std::string S_STRING_ID = "even_more_nearly_pi";
  const std::string ARRAY_ID = "You_should_probably_choose_a_short_name";
};
*/
/*
bool testSettings() {

  const std::string FILE = "ExampleClass.xml";

  ExampleClass exampleClass(FILE);
  std::cout << "These are the values of the registered Members\n"
            << "of exampleClass after constructor was called:" << std::endl;
  exampleClass.print();
  // save the corrent values of all registered membervariables:
  exampleClass.save();
  std::cout << "Now you could look at " << FILE
            << " and change some values. Press Enter when finnished." <<
std::endl; std::getchar();
  // change something in the file and reload into class
  exampleClass.reloadAllFromFile();
  exampleClass.print();

  std::cout << "As you can see the values have changed!\n"
            << "If the file already exists at construction,\n"
            << "the membervariables will be overwritten.\n"
            << "If you run the programm again, you will see\n"
            << "that previous changed values will be loaded\n"
            << "instead of the default values." << std::endl;
  return true;
}

*/
/*
bool testRandGen() {
  tool::RandomGenerator* rg = &tool::RandomGenerator::getInstance();

  // *********** distributions ***********

  std::cout << " some random numbers:\n";
  std::cout << rg->normalDistribution(0., 1.) << std::endl;
  std::cout << rg->uniformDistribution(0, 1) << std::endl;
  std::cout << rg->zeroOne() << std::endl;
  std::cout << rg->trueFalse() << std::endl;

  constexpr int NUM_VALS = 10;
  constexpr int NUM_EXPERIMENTS = 10000;

  std::cout << "\na Uniform [0,9] distribution:\n";

  std::array<int, NUM_VALS> num_count;
  for (std::size_t i = 0; i < NUM_VALS; i++) {
    num_count[i] = 0;
  }

  tool::UniformDistribution<int> ud(0, NUM_VALS - 1);
  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    size_t random_number = static_cast<size_t>(ud.get());
    num_count[random_number]++;
  }

  for (int& v : num_count) {
    const int num_ticks = v / static_cast<int>(0.01 * NUM_EXPERIMENTS);
    std::cout << "|";
    for (int j = 0; j < num_ticks; j++) {
      std::cout << "#";
    }
    std::cout << v << std::endl;
    v = 0;
  }

  std::cout << "\na N(4,1.2) distribution:\n";
  tool::NormalDistribution nd(4., 1.2);
  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    int random_number = std::round(nd.get());
    if (random_number > -1 && random_number < 10) {
      num_count[random_number]++;
    }
  }

  for (int& v : num_count) {
    const int num_ticks = v / static_cast<int>(0.01 * NUM_EXPERIMENTS);
    std::cout << "|";
    for (int j = 0; j < num_ticks; j++) {
      std::cout << "#";
    }
    std::cout << v << std::endl;
  }


  // *********** rand iterators ***********
  std::array<int, 3> a = {{1, 2, 3}};
  std::vector<int> b;
  b.reserve(4);
  for (int i = 0; i < 4; i++) {
    b.emplace_back(ud.get());
  }

  std::cout << "and now a random element of an array:\n[";

  for (const auto& i : a) {
    std::cout << i << ", ";
  }
  std::cout << "]\nchoose [" << *(rg->get<std::array<int, 3>>(a)) << "]" << std::endl;

  std::cout << "and now a random element of an vector:\n[";

  for (const auto& i : b) {
    std::cout << i << ", ";
  }
  std::cout << "]\nchoose [" << *(rg->get<std::vector<int>>(b)) << "]" << std::endl;

  return true;
}

bool testSFMLCube() {

  const Globals* G = &Globals::getInstance();

  bool exit = false;
  bool sRgb = false;

  while (!exit) {
    // Request a 24-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.sRgbCapable = sRgb;

    // Create the main window
    sf::RenderWindow window(
        sf::VideoMode(800, 600), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    // Create a sprite for the background
    sf::Texture backgroundTexture;
    backgroundTexture.setSrgb(sRgb);
    if (!backgroundTexture.loadFromFile(G->absolute_path_to_resources +
                                        "background.jpg"))
      return EXIT_FAILURE;
    sf::Sprite background(backgroundTexture);

    // Create some text to draw on top of our OpenGL object
    sf::Font font;
    if (!font.loadFromFile(G->absolute_path_to_resources + "sansation.ttf"))
      return EXIT_FAILURE;
    sf::Text text("SFML / OpenGL demo", font);
    sf::Text sRgbInstructions("Press space to toggle sRGB conversion", font);
    sf::Text mipmapInstructions("Press return to toggle mipmapping", font);
    text.setFillColor(sf::Color(255, 255, 255, 170));
    sRgbInstructions.setFillColor(sf::Color(255, 255, 255, 170));
    mipmapInstructions.setFillColor(sf::Color(255, 255, 255, 170));
    text.setPosition(250.f, 450.f);
    sRgbInstructions.setPosition(150.f, 500.f);
    mipmapInstructions.setPosition(180.f, 550.f);

    // Load a texture to apply to our 3D cube
    sf::Texture texture;
    if (!texture.loadFromFile(G->absolute_path_to_resources + "texture.jpg"))
      return EXIT_FAILURE;

    // Attempt to generate a mipmap for our cube texture
    // We don't check the return value here since
    // mipmapping is purely optional in this example
    texture.generateMipmap();

    // Make the window the active window for OpenGL calls
    window.setActive(true);

    // Load OpenGL or OpenGL ES entry points using glad
#ifdef SFML_OPENGL_ES
    gladLoadGLES1(reinterpret_cast<GLADloadfunc>(sf::Context::getFunction));
#else
    gladLoadGL(reinterpret_cast<GLADloadfunc>(sf::Context::getFunction));
#endif

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
#ifdef SFML_OPENGL_ES
    glClearDepthf(1.f);
#else
    glClearDepth(1.f);
#endif

    // Disable lighting
    glDisable(GL_LIGHTING);

    // Configure the viewport (the same size as the window)
    glViewport(0, 0, window.getSize().x, window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
#ifdef SFML_OPENGL_ES
    glFrustumf(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#else
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#endif

    // Bind the texture
    glEnable(GL_TEXTURE_2D);
    sf::Texture::bind(&texture);

    // Define a 3D cube (6 faces made of 2 triangles composed by 3 vertices)
    static const GLfloat cube[] = {
        // positions    // texture coordinates
        -20, -20, -20, 0, 0, -20, 20,  -20, 1, 0, -20, -20, 20,  0, 1,
        -20, -20, 20,  0, 1, -20, 20,  -20, 1, 0, -20, 20,  20,  1, 1,

        20,  -20, -20, 0, 0, 20,  20,  -20, 1, 0, 20,  -20, 20,  0, 1,
        20,  -20, 20,  0, 1, 20,  20,  -20, 1, 0, 20,  20,  20,  1, 1,

        -20, -20, -20, 0, 0, 20,  -20, -20, 1, 0, -20, -20, 20,  0, 1,
        -20, -20, 20,  0, 1, 20,  -20, -20, 1, 0, 20,  -20, 20,  1, 1,

        -20, 20,  -20, 0, 0, 20,  20,  -20, 1, 0, -20, 20,  20,  0, 1,
        -20, 20,  20,  0, 1, 20,  20,  -20, 1, 0, 20,  20,  20,  1, 1,

        -20, -20, -20, 0, 0, 20,  -20, -20, 1, 0, -20, 20,  -20, 0, 1,
        -20, 20,  -20, 0, 1, 20,  -20, -20, 1, 0, 20,  20,  -20, 1, 1,

        -20, -20, 20,  0, 0, 20,  -20, 20,  1, 0, -20, 20,  20,  0, 1,
        -20, 20,  20,  0, 1, 20,  -20, 20,  1, 0, 20,  20,  20,  1, 1};

    // Enable position and texture coordinates vertex components
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), cube);
    glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), cube + 3);

    // Disable normal and color vertex components
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Make the window no longer the active window for OpenGL calls
    window.setActive(false);

    // Create a clock for measuring the time elapsed
    sf::Clock clock;

    // Flag to track whether mipmapping is currently enabled
    bool mipmapEnabled = true;

    // Start game loop
    while (window.isOpen()) {
      // Process events
      sf::Event event;
      while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
          exit = true;
          window.close();
        }

        // Escape key: exit
        if ((event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::Escape)) {
          exit = true;
          window.close();
        }

        // Return key: toggle mipmapping
        if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::E)) {
          if (mipmapEnabled) {
            // We simply reload the texture to disable mipmapping
            if (!texture.loadFromFile(G->absolute_path_to_resources +
                                      "texture.jpg"))
              return EXIT_FAILURE;

            mipmapEnabled = false;
          } else {
            texture.generateMipmap();

            mipmapEnabled = true;
          }
        }

        // Space key: toggle sRGB conversion
        if ((event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::Space)) {
          sRgb = !sRgb;
          window.close();
        }

        // Adjust the viewport when the window is resized
        if (event.type == sf::Event::Resized) {
          sf::Vector2u textureSize = backgroundTexture.getSize();

          // Make the window the active window for OpenGL calls
          window.setActive(true);

          glViewport(0, 0, event.size.width, event.size.height);
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          GLfloat ratio = static_cast<float>(event.size.width) / event.size.height;
#ifdef SFML_OPENGL_ES
          glFrustumf(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#else
          glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#endif

          // Make the window no longer the active window for OpenGL calls
          window.setActive(false);

          sf::View view;
          view.setSize(textureSize.x, textureSize.y);
          view.setCenter(textureSize.x / 2.f, textureSize.y / 2.f);
          window.setView(view);
        }
      }

      // Draw the background
      window.pushGLStates();
      window.draw(background);
      window.popGLStates();

      // Make the window the active window for OpenGL calls
      window.setActive(true);

      // Clear the depth buffer
      glClear(GL_DEPTH_BUFFER_BIT);

      // We get the position of the mouse cursor (or touch), so that we can move the box accordingly
      sf::Vector2i pos;

#ifdef SFML_SYSTEM_IOS
      pos = sf::Touch::getPosition(0);
#else
      pos = sf::Mouse::getPosition();
#endif

      float x = pos.x * 200.f / window.getSize().x - 100.f;
      float y = -pos.y * 200.f / window.getSize().y + 100.f;

      // Apply some transformations
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x, y, -100.f);
      glRotatef(clock.getElapsedTime().asSeconds() * 50.f, 1.f, 0.f, 0.f);
      glRotatef(clock.getElapsedTime().asSeconds() * 30.f, 0.f, 1.f, 0.f);
      glRotatef(clock.getElapsedTime().asSeconds() * 90.f, 0.f, 0.f, 1.f);

      // Draw the cube
      glDrawArrays(GL_TRIANGLES, 0, 36);

      // Make the window no longer the active window for OpenGL calls
      window.setActive(false);

      // Draw some text on top of our OpenGL object
      window.pushGLStates();
      window.draw(text);
      window.draw(sRgbInstructions);
      window.draw(mipmapInstructions);
      window.popGLStates();

      // Finally, display the rendered frame on screen
      window.display();
    }
  }

  return true;
}

*/



#include <display/display.h>
#include <display/menu.h>
#include <world/world.h>

#include <chrono>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

int main(int argc, char* argv[]) {

  DEBUG("a simple string");
  F_DEBUG("like printf %d", 4);
  F_WARNING("like printf %d", 2);
  F_ERROR("like printf %d", 3);

  SimulationSettings menu;
  World simulatedWorld;
  Display display(dynamic_cast<Menu*>(&menu));
  int mash_id = display.addMesh(simulatedWorld.getWorldMesh());

  using namespace std::chrono;
  using dsec = duration<double>;
  auto next_update = system_clock::now() +
                     round<system_clock::duration>(dsec{menu.get_target_update_rate()});


  while (display.isOpen()) {
    simulatedWorld.update();

    const auto now = system_clock::now();
    if (now > next_update) {
      next_update =
          now + round<system_clock::duration>(dsec{menu.get_target_update_rate()});
      display.draw();
    }
  }
  menu.save();
  simulatedWorld.save();
  return 0;
}
